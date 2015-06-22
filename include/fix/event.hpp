#pragma once

#include "fix/message.hpp"
#include "publisher.hpp"

namespace fix {

struct event
{
    session_id session_;
    message message_;
};

typedef publisher< event, blocking_sequence > event_publisher;
typedef subscriber< event, blocking_sequence > event_subscriber;

class event_processor
{
private:
    fix::event_publisher& out_;

protected:
    void publish( const fix::event& ei )
    {
        out_.publish( 1, [&]( fix::event& eo, size_t n ) {
            eo = ei;
        } );
    }

public:
    event_processor( fix::event_publisher& out ) :
        out_( out )
    {
    }

    virtual ~event_processor() {}

    virtual void on_init() = 0;

    virtual void on_event( const fix::event& ev ) = 0;
};

}
