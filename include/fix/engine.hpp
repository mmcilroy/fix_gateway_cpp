#pragma once

#include "fix/session.hpp"
#include "fix/decoder.hpp"

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using boost::asio::ip::tcp;

namespace fix {

class tcp_session;

class engine
{
friend class tcp_session;
public:
    engine();

    ~engine();

    template< typename H >
    void acceptor( const std::string& conn, H handler );

    template< typename H >
    session_id initiator( const std::string& conn, const header& hdr, H handler );

    void send( session_id, const std::string& type, const message& msg );

    void start();

private:
    template< typename H >
    void start_accept( tcp::acceptor*, H handler );

    template< typename H >
    void handle_accept( tcp::acceptor*, tcp_session*, H handler, const boost::system::error_code& );

    void closed( session_id );

    session_id alloc_id();

    typedef std::map< session_id, tcp_session* > session_map;
    session_map sessions_;

    boost::asio::io_service io_;
};

void set_utc_time( std::string&, time_t* );

#include "fix/engine.inl"

}
