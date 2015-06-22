#include "fix/message.hpp"

template< typename V >
void from_string( V& v, const std::string& s )
{
    std::stringstream ss;
    ss << s;
    ss >> v;
}

void acceptor( const fix::message& request )
{
    int id;
    std::string symbol;

    request.parse( [&]( fix::tag tag, const std::string& val ) {
        if( tag == 11 ) {
            from_string( id, val );
        } else if( tag == 55 ) {
            from_string( symbol, val );
        }
    } );

    std::cout << id << ", " << symbol << std::endl;

    fix::message response;
    response.add( 11, id );
    response.add( 55, symbol );
    response.add( 39, 0 );
    response.add( 150, 0 );
}

int main()
{
    fix::message request;
    request.add( 11, 12345 );
    request.add( 55, "AAA" );
    acceptor( request );
}
