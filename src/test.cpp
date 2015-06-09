
#include "fixtk/message.hpp"

int main()
{
    fixtk::message m;
    m.add( 1, "one" );
    m.add( 2, '2' );
    m.add( 3, 3 );
    std::cout << m.str() << std::endl;
}
