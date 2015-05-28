
class session
{
public:
    session( boost::asio::io_service& );

    ~session();

    template< typename H >
    void read( H handler );

    template< typename H >
    void handle_read( H handler, const boost::system::error_code& error, size_t bytes_transferred );

    tcp::socket socket_;

    enum { max_length = 1024 };
    char data_[max_length];
};



// ----------------------------------------------------------------------------
engine::engine()
{
    std::cout << this << " new engine" << std::endl;
}

engine::~engine()
{
    std::cout << this << " new engine" << std::endl;
}

template< typename H >
void engine::acceptor( const std::string& conn, H handler )
{
    std::cout << this << " engine.acceptor: " << conn << std::endl;
    start_accept( new tcp::acceptor( io_, tcp::endpoint( tcp::v4(), 14002 ) ), handler );
}

void engine::send( session_id id, const message& msg )
{
    std::cout << this << " engine.send: " << id << ", " << msg << std::endl;
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
    session* sess = new session( io_ );
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



// ----------------------------------------------------------------------------
session::session( boost::asio::io_service& io ) :
    socket_( io )
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
