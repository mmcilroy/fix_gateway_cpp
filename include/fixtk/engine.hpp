#pragma once

#include "fixtk/message.hpp"

#include <boost/bind.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

typedef uint64_t session_id;

class session;

class engine
{
public:
    template< typename H >
    void acceptor( const std::string& conn, H handler );

    template< typename H >
    session_id initiator( const std::string& conn, H handler );

    void send( session_id, const message& );

    void start();

private:
    template< typename H >
    void start_accept( tcp::acceptor*, H handler );

    template< typename H >
    void handle_accept( tcp::acceptor*, session*, H handler, const boost::system::error_code& );

    boost::asio::io_service io_;
};

#include "fixtk/engine.inl"
