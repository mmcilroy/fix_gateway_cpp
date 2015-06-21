#pragma once

#include "fix/session.hpp"
#include <memory>

namespace fix {

// ----------------------------------------------------------------------------
struct receiver
{
    virtual void recv( session_id, const fix::message& ) = 0;
};

// ----------------------------------------------------------------------------
class engine
{
public:
    engine( std::unique_ptr< receiver > );

    virtual void accept() = 0;

    virtual session_id connect( const header& ) = 0;

    virtual void send( session_id, const fix::message& ) = 0;

protected:
    session_id make_session();

    void recv( session_id, const fix::message );

private:
    std::vector< std::unique_ptr< session > > sessions_;
    std::unique_ptr< receiver > receiver_;
};

// ----------------------------------------------------------------------------
engine::engine( std::unique_ptr< receiver > receiver ) :
    receiver_( std::move( receiver ) )
{
}

session_id engine::make_session()
{
    return 1;
}

void engine::recv( session_id id, const fix::message msg )
{
    receiver_->recv( id, msg  );
}

}
