
#include "fix/message.hpp"

int main()
{
    fix::message m;
    m.add( 128, "XLON" );
    m.add( 146, 3 );
    m.add( 55, "AAA" );
    m.add( 55, "BBB" );
    m.add( 55, "CCC" );

    std::cout << m.str() << std::endl;

    m.parse( []( fix::tag tag, const std::string& val ) {
        std::cout << tag << "=" << val << std::endl;
    } );
}
