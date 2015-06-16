
#include "fix/session.hpp"

int main()
{
    fix::message m;
    m.add( 128, "XLON" );
    m.add( 146, 3 );
    m.add( 55, "AAA" );
    m.add( 55, "BBB" );
    m.add( 55, "CCC" );

    fix::session s( 1 );
    s.recv( m );
    s.send( "D", m );
}
