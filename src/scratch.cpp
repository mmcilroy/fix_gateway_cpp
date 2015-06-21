
#include "fix/scratch.hpp"

class my_engine : public fix::engine
{
public:
    my_engine( std::unique_ptr< fix::receiver > receiver ) :
        engine( std::move( receiver ) )
    {
    }

    virtual fix::session_id connect( const fix::header& )
    {
        fix::session_id id = make_session();
        recv( id, fix::message( "1=one|2=two|" ) );
        return id;
    }

    virtual void accept()
    {
        fix::session_id id = make_session();
        recv( id, fix::message( "1=one|2=two|" ) );
    }

    virtual void send( fix::session_id id, const fix::message& msg )
    {
        std::cout << "send " << id << ", " << msg << std::endl;
    }
};

std::vector< std::unique_ptr< fix::engine > > my_engines;

void on_init();

void on_event( fix::session_id, const fix::message& );

struct my_receiver : public fix::receiver
{
    virtual void recv( fix::session_id id, const fix::message& msg )
    {
        on_event( id, msg );
    }
};

void on_init()
{
    std::unique_ptr< fix::receiver > r( new my_receiver );
    std::unique_ptr< fix::engine > e( new my_engine( std::move( r ) ) );
    my_engines.push_back( std::move( e ) );
    my_engines[0]->connect( fix::header() );
}

void on_event( fix::session_id id, const fix::message& msg )
{
    std::cout << "got " << id << ", " << msg << std::endl;
}

void fix_accept()
{
    if( my_engines.size() == 0 )
    {
        // need to create a new engine
        std::unique_ptr< fix::receiver > r( new my_receiver );
        std::unique_ptr< fix::engine > e( new my_engine( std::move( r ) ) );
        my_engines.push_back( std::move( e ) );
    }

    my_engines[0]->accept();

}

void fix_connect()
{
}

void fix_send()
{
}

int main()
{
    on_init();
}
