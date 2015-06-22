#pragma once

#include "fix/engine.hpp"
#include "fix/event.hpp"

namespace app {

fix::session_id connect( const std::string&, const fix::header& );
void accept( const std::string& );
void send( fix::session_id, const std::string&, const fix::message& );

}
