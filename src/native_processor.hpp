#pragma once

#include "event.hpp"

extern void accept( const std::string& );

class native_processor
{
public:
    native_processor( fix::event_publisher& )
    {
    }

    void on_init()
    {
        accept( ":8080" );
    }

    void on_event( const fix::event& )
    {
    }
};
