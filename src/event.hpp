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

}
