#pragma once

#include "event.hpp"

class native_processor
{
public:
    native_processor( fix::event_publisher& )
    {
    }

    void on_init()
    {
    }

    void on_event( const fix::event& )
    {
    }
};
