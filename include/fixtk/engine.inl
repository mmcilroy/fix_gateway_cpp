
class session
{
public:
    session( boost::asio::io_service& );

    template< typename H >
    void read( H handler );

    template< typename H >
    void handle_read( H handler, const boost::system::error_code& error, size_t bytes_transferred );

    tcp::socket socket_;

    enum { max_length = 1024 };
    char data_[max_length];
};



// ----------------------------------------------------------------------------
template< typename H >
void engine::acceptor( const std::string& conn, H handler )
{
    start_accept( new tcp::acceptor( io_, tcp::endpoint( tcp::v4(), 14002 ) ), handler );
}

void engine::send( session_id, const message& )
{
}

void engine::start()
{
    io_.run();
}

template< typename H >
void engine::start_accept( tcp::acceptor* acc, H handler )
{
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
}

template< typename H >
void session::read( H handler )
{
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
