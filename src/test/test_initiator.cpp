
int main()
{
    fix::engine engine;
    fix::header header( "FIX.4.4", "SENDER", "TARGET" );
    fix::session session = engine.initiator( "localhost:14000", header, []( fix::session_id session, const fix::message& response ) {
        std::cout << response << std::endl;
    } );

    fix::message request;
    request.add( 11, 12345 );
    request.add( 38, 100 );
    request.add( 40, 2 );
    request.add( 44, 100 );
    request.add( 54, 1 );
    request.add( 55, "VOD.L" );

    engine.send( session, request );
    engine.start();
}
