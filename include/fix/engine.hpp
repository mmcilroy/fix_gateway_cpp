#pragma once

namespace fix {

typedef uint64_t session_id;

class session;

class engine
{
friend class session;
public:
    template< typename H >
    void acceptor( const std::string& conn, H handler );

    template< typename H >
    session_id initiator( const std::string& conn, const header& hdr, H handler );

    void send( session_id id, const std::string& type, const message& msg );

    void start();
};

}
