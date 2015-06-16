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
    header hdr_;
    sequence seq_;
};

// ----------------------------------------------------------------------------
class session
{
public:
    session( session_id );

    void send( const std::string& type, const fix::message& body );

    void recv( const fix::message& body );

private:
    session_id id_;
    std::unique_ptr< state > state_;
};

// ----------------------------------------------------------------------------
session::session( session_id id ) :
    id_( id )
{
}

void session::send( const std::string& type, const fix::message& body )
{
    if( state_ )
    {
        std::string send_time;
        set_utc_time( send_time );

        std::stringstream body_ss;
        body_ss << msg_type << "=" << type << delim_char;
        body_ss << msg_seq_num << "=" << state_->hdr_.sequence_ << delim_char;
        body_ss << sender_comp_id << "=" << state_->hdr_.sender_ << delim_char;
        body_ss << target_comp_id << "=" << state_->hdr_.target_ << delim_char;
        body_ss << sending_time << "=" << send_time << delim_char;
        body_ss << body.str();

        std::stringstream msg_ss;
        msg_ss << begin_string << "=" << state_->hdr_.protocol_ << delim_char;
        msg_ss << body_length << "=" << body_ss.str().length() << delim_char;
        msg_ss << body_ss.str();

        int checksum = 0;
        std::string s = msg_ss.str();
        for( int i=0; i<s.size(); i++ ) {
            checksum += (int)s[i];
        }

        char buf[4];
        sprintf( buf, "%03d", checksum % 256 );

        msg_ss << check_sum << "=" << buf << delim_char;

        std::cout << "send: " << msg_ss.str() << std::endl;
    }
}

void session::recv( const fix::message& body )
{
    if( !state_ )
    {
        state_ = std::move( std::unique_ptr< state >( new state() ) );

        // get protocol, sender and target
        body.parse( [&
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
