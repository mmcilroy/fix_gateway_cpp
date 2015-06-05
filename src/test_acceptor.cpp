
int main()
{
    fix::engine engine;
    engine.acceptor( ":14000", []( fix::session_id session, const fix::message& request ) {
        fix::message response;
        response[11] = request[11];
        response[55] = request[55];
        response[39] = 0;
        response[150] = 0;
        engine.send( session, response );
    } );
    engine.start();
}
