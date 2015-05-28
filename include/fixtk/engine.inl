
#define BEGIN_STRING 8
#define BODY_LENGTH 9
#define CHECK_SUM 10
#define MSG_TYPE 35
#define MSG_SEQ_NUM 34
#define SENDING_TIME 52
#define SENDER_COMP_ID 49
#define TARGET_COMP_ID 56
#define DELIM (char)1

void set_utc_time( std::string& s, time_t* t )
{
    struct tm* utc = gmtime( t );

    char buf[32];
    sprintf( buf, "%04d%02d%02d-%02d:%02d:%02d",
        utc->tm_year+1900,
        utc->tm_mon+1,
        utc->tm_mday,
        utc->tm_hour,
        utc->tm_min,
        utc->tm_sec );

    s = buf;
}

void encode( message& out, const header& hdr, const field_vector& flds )
{
    std::string send_time;
    time_t t = time( 0 );
    set_utc_time( send_time, &t );

    std::stringstream body_str;
    body_str << MSG_TYPE << "=" << hdr.type_ << DELIM;
    body_str << MSG_SEQ_NUM << "=" << hdr.sequence_ << DELIM;
    body_str << SENDER_COMP_ID << "=" << hdr.sender_ << DELIM;
    body_str << TARGET_COMP_ID << "=" << hdr.target_ << DELIM;
    body_str << SENDING_TIME << "=" << send_time << DELIM;

    for( int i=0; i<flds.size(); i++ ) {
        body_str << flds[i].tag_ << "=" << flds[i].value_ << DELIM;
    }

    int length = body_str.str().length();

    std::stringstream msg_str;
    msg_str << BEGIN_STRING << "=" << hdr.protocol_ << DELIM;
    msg_str << BODY_LENGTH << "=" << length << DELIM;
    msg_str << body_str.str();

    int checksum = 0;
    std::string s = msg_str.str();
    for( int i=0; i<s.size(); i++ ) {
        checksum += (int)s[i];
    }

    char buf[4];
    sprintf( buf, "%03d", checksum % 256 );

    msg_str << CHECK_SUM << "=" << buf << DELIM;
    out = msg_str.str();
}



// ----------------------------------------------------------------------------
class session
{
public:
    session( engine&, boost::asio::io_service& );

    ~session();

    template< typename H >
    void read( H handler );

    template< typename H >
    void handle_read( H handler, const boost::system::error_code& error, size_t bytes_transferred );

    void send( const message& );

    tcp::socket socket_;

    session_id id_;

    enum { max_length = 1024 };
    char data_[max_length];
};



// ----------------------------------------------------------------------------
session::session( engine& engine, boost::asio::io_service& io ) :
    socket_( io ),
    id_( engine.alloc_id() )
{
    std::cout << this << " new session" << std::endl;
}

session::~session()
{
    std::cout << this << " del session" << std::endl;
}

template< typename H >
void session::read( H handler )
{
    std::cout << this << " session.read" << std::endl;
    socket_.async_read_some(
        boost::asio::buffer( data_, max_length ),
        boost::bind(
            &session::handle_read< H >,
            this,
            handler,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred ) );
}

template< typename H >
void session::handle_read( H handler, const boost::system::error_code& err, size_t bytes_transferred )
{
    std::cout << this << " session.handle_read: " << err.message() << ", " << bytes_transferred << std::endl;
    if( !err )
    {
        data_[ bytes_transferred ] = 0;
        handler( 1, data_ );
        read( handler );
    }
    else
    {
        delete this;
    }
}

void session::send( const message& msg )
{
    boost::asio::write(
        socket_,
        boost::asio::buffer( msg.c_str(), msg.size() ) );
}



// ----------------------------------------------------------------------------
engine::engine()
{
    std::cout << this << " new engine" << std::endl;
}

engine::~engine()
{
    std::cout << this << " del engine" << std::endl;
}

template< typename H >
void engine::acceptor( const std::string& conn, H handler )
{
    std::cout << this << " engine.acceptor: " << conn << std::endl;
    start_accept( new tcp::acceptor( io_, tcp::endpoint( tcp::v4(), 14002 ) ), handler );
}

template< typename H >
session_id engine::initiator( const std::string& conn, H handler )
{
    std::cout << this << " engine.initiator: " << conn << std::endl;

    tcp::resolver resolver( io_ );
    tcp::resolver::query query( tcp::v4(), "ln3-lgma01c", "9107" );
    tcp::resolver::iterator it = resolver.resolve( query );
    session* sess = new session( *this, io_ );
    sessions_[ sess->id_ ] = sess;
    boost::asio::connect( sess->socket_, it );
    sess->read( handler );
    return sess->id_;
}

void engine::send( session_id id, const fixtk::field_vector& flds )
{
    std::cout << this << " engine.send: " << id << ", " << flds << std::endl;

    header hdr;
    hdr.type_ = "D";
    hdr.protocol_ = "FIX.4.4";
    hdr.sender_ = "S";
    hdr.target_ = "T";
    hdr.sequence_ = 1;

    message msg;
    encode( msg, hdr, flds );

    io_.dispatch( [ this, id, msg ]()
    {
        auto it = sessions_.find( id );
        if( it != sessions_.end() ) {
            it->second->send( msg );
        } else {
            std::cerr << "unknown session_id " << id << std::endl;
        }
    } );
}

void engine::start()
{
    std::cout << this << " engine.start" << std::endl;
    io_.run();
}

template< typename H >
void engine::start_accept( tcp::acceptor* acc, H handler )
{
    std::cout << this << " engine.start_accept" << std::endl;
    session* sess = new session( *this, io_ );
    sessions_[ sess->id_ ] = sess;
    acc->async_accept(
        sess->socket_,
        boost::bind(
            &engine::handle_accept< H >,
            this,
            acc,
            sess,
            handler,
            boost::asio::placeholders::error ) );
}

template< typename H >
void engine::handle_accept( tcp::acceptor* acc, session* sess, H handler, const boost::system::error_code& err )
{
    std::cout << this << " engine.handle_accept: " << err.message() << std::endl;
    if( !err ) {
        sess->read( handler );
    } else {
      delete sess;
    }
    start_accept( acc, handler );
}

void engine::closed( session_id id )
{
    std::cout << this << " engine.closed: " << id << std::endl;
    sessions_.erase( id );
}

session_id engine::alloc_id()
{
    static session_id id = 0;
    return id++;
}
