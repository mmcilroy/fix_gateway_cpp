

int main()
{
    fix::engine engine;

    engine.acceptor( ":14000", []( fix::session_id session, const fix::message& request )
    {
        int id;
        std::string symbol;

        request.parse( []( fix::tag tag, fix::value val ) {
            if( tag == 11 ) {
                copy_string( val, id );
            } else if( tag == 11 ) {
                copy_string( val, symbol );
            }
        } );

        fix::message response;
        response.add( 11, id );
        response.add( 55, symbol );
        response.add( 39, 0 );
        response.add( 150, 0 );

        engine.send( session, response );
    } );

    engine.start();
}
