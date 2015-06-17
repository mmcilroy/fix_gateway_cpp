#pragma once

#include "fix/message.hpp"
#include "fix/util.hpp"

namespace fix {

// ----------------------------------------------------------------------------
typedef int session_id;
typedef int sequence;

// ----------------------------------------------------------------------------
struct state
{
    state();

    header hdr_;
    sequence seq_;
};

// ----------------------------------------------------------------------------
class session
{
public:
    session( session_id );

    session( session_id, const header& );

    void encode( fix::message& out, const std::string& type, const fix::message& body );

    void recv( const fix::message& body );

//private:
    session_id id_;
    std::unique_ptr< state > state_;
};

// ----------------------------------------------------------------------------
state::state() :
    seq_( 1 )
{
}

session::session( session_id id ) :
    id_( id )
{
}

session::session( session_id id, const header& hdr ) :
    id_( id ),
    state_( new state() )
{
    state_->hdr_ = hdr;
}

void session::encode( fix::message& out, const std::string& type, const fix::message& body )
{
    if( state_ )
    {
        std::string send_time;
        set_utc_time( send_time );

        fix::message hdr;
        hdr.add( msg_type, type );
        hdr.add( msg_seq_num, state_->seq_++ );
        hdr.add( sender_comp_id, state_->hdr_.sender_ );
        hdr.add( target_comp_id, state_->hdr_.target_ );
        hdr.add( sending_time, send_time );

        out.add( begin_string, state_->hdr_.protocol_ );
        out.add( body_length, hdr.size() );
        out.add( hdr );

        int checksum = 0;
        for( int i=0; i<out.size(); i++ ) {
            checksum += (int)out[i];
        }

        char buf[4];
        sprintf( buf, "%03d", checksum % 256 );

        out.add( check_sum, buf );

        std::cout << "encoded " << out << std::endl;
    }
}

void session::recv( const fix::message& msg )
{
    std::cout << "received " << msg << std::endl;

    if( !state_ )
    {
        state_ = std::move( std::unique_ptr< state >( new state() ) );

        // get protocol, sender and target
        msg.parse( [&
            ]( fix::tag tag, const std::string& val ) {
            if( tag == 8 ) {
                state_->hdr_.protocol_ = val;
            } else if( tag == 49 ) {
                state_->hdr_.sender_ = val;
            } else if( tag == 56 ) {
                state_->hdr_.target_ = val;
            }
        } );

        // recover sequence number
        state_->seq_ = 1;

        // validate
    }
}

}
