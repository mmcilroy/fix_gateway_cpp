
int main()
{
    fix::engine engine;
    fix::header header( "FIX.4.4", "SENDER", "TARGET" );
    fix::session session = engine.initiator( "localhost:14000", header, []( fix::session_id session, const fix::message& response ) {
        std::cout << response << std::endl;
    } );

    fix::message request;
    request[11] = 12345;
    request[38] = 100;
    request[40] = 2;
    request[44] = 100;
    request[54] = 1;
    request[55] = "VOD.L";

    engine.send( session, request );
    engine.start();
}
