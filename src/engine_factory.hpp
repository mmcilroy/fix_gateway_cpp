#pragma once

namespace fix {

typedef int engine_id;

// ----------------------------------------------------------------------------
struct receiver
{
    virtual void recv( engine_id, session_id, const fix::message& ) = 0;
};

// ----------------------------------------------------------------------------
class engine_factory
{
public:
    engine_id make_engine( const std::string& plugin )
    {
        std::unique_ptr< fix::receiver > r( new my_receiver );
        std::unique_ptr< fix::engine > e( new my_engine( std::move( r ) ) );
        my_engines.push_back( std::move( e ) );
    }

    void accept( engine_id );

    session_id connect( engine_id, const header& );

private:
    std::unique_ptr< receiver > receiver_;
    std::vector< 
};

}
