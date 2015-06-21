
#include "fix/engine.hpp"
#include "native_processor.hpp"
#include "lua_processor.hpp"

const size_t Q = 8;

fix::engine engine;
fix::event_publisher inp_pub( Q );
fix::event_publisher out_pub( Q );
fix::event_subscriber& biz_sub = inp_pub.subscribe();

lua_processor* processor;

void accept( const std::string& conn )
{
    engine.acceptor( conn, [&]( fix::session_id id, const fix::message& msg ) {
        inp_pub.publish( 1, [&]( fix::event& ev, size_t n ) {
            ev.session_ = id;
            ev.message_ = msg;
        } );
    } );
}

fix::session_id connect( const std::string& conn, const fix::header& hdr )
{
    return engine.initiator( conn, hdr, [&]( fix::session_id id, const fix::message& msg ) {
        inp_pub.publish( 1, [&]( fix::event& ev, size_t n ) {
            ev.session_ = id;
            ev.message_ = msg;
        } );
    } );
}

void send( fix::session_id id, const std::string& type, const fix::message& msg )
{
    engine.send( id, type, msg );
}

void biz_thr_fn( fix::event_subscriber* sub, fix::event_publisher* pub )
{
    sub->dispatch( [&]( const fix::event& ei, size_t rem )
    {
        processor->on_event( ei );
        return false;
    } );
}

int main( int argc, char** argv )
{
    processor = new lua_processor( argv[1], out_pub );
    processor->on_init();

    std::thread biz_thr( biz_thr_fn, &biz_sub, &out_pub );

    engine.start();

    biz_thr.join();
}
