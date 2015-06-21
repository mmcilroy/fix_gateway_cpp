#pragma once

#include "fix/message.hpp"
#include "publisher.hpp"

namespace fix {

struct source
{
    engine_id engine_;
    session_id session_;
};

struct event
{
    source source_;
    message message_;
};

typedef publisher< event, blocking_sequence > event_publisher;
typedef subscriber< event, blocking_sequence > event_subscriber;

}
