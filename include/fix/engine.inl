
const size_t buffer_size = 1024;

class tcp_session
{
public:
    tcp_session( engine& );

    tcp_session( engine&, const header& );

    ~tcp_session();

    template< typename H >
    void read( H handler );

    template< typename H >
    void handle_read( H handler, const boost::system::error_code& error, size_t bytes_transferred );

    void send( const message& );

    tcp::socket socket_;

    session session_;
    decoder decoder_;

    char buf_[ buffer_size ];
};



// ----------------------------------------------------------------------------
tcp_session::tcp_session( engine& engine ) :
    socket_( engine.io_ ),
    session_( engine.alloc_id() )
{
    std::cout << this << " new tcp_session" << std::endl;
}

tcp_session::tcp_session( engine& engine, const header& hdr ) :
    socket_( engine.io_ ),
    session_( engine.alloc_id(), hdr )
{
    std::cout << this << " new tcp_session" << std::endl;
}

tcp_session::~tcp_session()
{
    std::cout << this << " del tcp_session" << std::endl;
}

template< typename H >
void tcp_session::read( H handler )
{
    std::cout << this << " tcp_session.read" << std::endl;
    socket_.async_read_some(
        boost::asio::buffer( buf_, buffer_size ),
        boost::bind(
            &tcp_session::handle_read< H >,
            this,
            handler,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred ) );
}

template< typename H >
void tcp_session::handle_read( H handler, const boost::system::error_code& err, size_t bytes_transferred )
{
    std::cout << this << " tcp_session.handle_read: " << err.message() << ", " << bytes_transferred << std::endl;
    if( !err )
    {
        decoder_.decode( buf_, bytes_transferred, [&]( const fix::message& msg ) {
            session_.recv( msg );
            handler( session_.id_, msg );
        } );
        read( handler );
    }
    else
    {
        delete this;
    }
}

void tcp_session::send( const message& msg )
{
    boost::asio::write(
        socket_,
        boost::asio::buffer( msg.str(), msg.size() ) );
}



// ----------------------------------------------------------------------------
void parse_conn( const std::string& conn, std::string& host, std::string& port )
{
    std::vector< std::string > strs;
    boost::split( strs, conn, boost::is_any_of( ":" ) );
    if( strs.size() == 2 ) {
        host = strs[0]; port = strs[1];
    } else {
        std::cerr << "invalid tcp connection - " << conn << std::endl;
    }
}

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

    std::string host;
    std::string port;
    parse_conn( conn, host, port );
    start_accept( new tcp::acceptor( io_, tcp::endpoint( tcp::v4(), boost::lexical_cast< int >( port ) ) ), handler );
}

template< typename H >
session_id engine::initiator( const std::string& conn, const header& hdr, H handler )
{
    std::cout << this << " engine.initiator: " << conn << std::endl;

    std::string host;
    std::string port;
    parse_conn( conn, host, port );

    tcp::resolver resolver( io_ );
    tcp::resolver::query query( tcp::v4(), host, port );
    tcp::resolver::iterator it = resolver.resolve( query );
    tcp_session* sess = new tcp_session( *this, hdr );
    sessions_[ sess->session_.id_ ] = sess;
    boost::asio::connect( sess->socket_, it );

    sess->read( handler );

    return sess->session_.id_;
}
void engine::send( session_id id, const std::string& type, const message& body )
{
    std::cout << this << " engine.send: " << id << ", " << body << std::endl;

    auto it = sessions_.find( id );
    if( it != sessions_.end() )
    {
        message msg;
        it->second->session_.encode( msg, type, body );

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
    else
    {
        std::cerr << "unknown session_id " << id << std::endl;
    }
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
    tcp_session* sess = new tcp_session( *this );
    sessions_[ sess->session_.id_ ] = sess;
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
void engine::handle_accept( tcp::acceptor* acc, tcp_session* sess, H handler, const boost::system::error_code& err )
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
