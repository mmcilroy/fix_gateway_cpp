
#include "fix/session.hpp"
#include "native_processor.hpp"

const size_t Q = 8;

fix::event_publisher inp_pub( Q );
fix::event_publisher out_pub( Q );

fix::event_subscriber& biz_sub = inp_pub.subscribe();

native_processor processor( out_pub );

void biz_thr_fn( fix::event_subscriber* sub, fix::event_publisher* pub )
{
    sub->dispatch( [&]( const fix::event& ei, size_t rem )
    {
        processor.on_event( ei );
        return false;
    } );
}

void out_thr_fn( fix::event_subscriber* sub )
{
    sub->dispatch( [&]( const fix::event& ev, size_t rem )
    {
        return false;
    } );
}

int main()
{
    std::thread out_thr( out_thr_fn, &out_pub.subscribe() );
    std::thread biz_thr( biz_thr_fn, &biz_sub, &out_pub );

    biz_thr.join();
    out_thr.join();
}
