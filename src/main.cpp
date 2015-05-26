extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <iostream>

typedef int fix_session_id;

class fix_engine
{
public:
    fix_engine()
    {
        std::cout << this << " new engine" << std::endl;
    }

    ~fix_engine()
    {
        std::cout << this << " del engine" << std::endl;
    }

    template< typename H >
    void acceptor( const std::string& conn, H handler )
    {
        std::cout << this << " acceptor" << std::endl;
        handler( 1, "hi" );
    }

    void send( fix_session_id id, const std::string& message )
    {
        std::cout << this << " send " << id << ", " << message << std::endl;
    }

    void start()
    {
        std::cout << this << " start" << std::endl;
    }
};

const char* engine_id = "luaL_engine";

fix_engine* l_check_fix_engine( lua_State* l, int n )
{
    return *(fix_engine**)luaL_checkudata( l, n, engine_id );
}

int l_new_engine( lua_State* l )
{
    fix_engine** udata = (fix_engine**)lua_newuserdata( l, sizeof( fix_engine* ) );
    *udata = new fix_engine();
    luaL_getmetatable( l, engine_id );
    lua_setmetatable( l, -2 );
    return 1;
}

int l_del_engine( lua_State* l )
{
    delete l_check_fix_engine( l, -1 );
    return 0;
}

int l_engine_acceptor( lua_State* l )
{
    fix_engine* engine = l_check_fix_engine( l, 1 );
    const char* conn = luaL_checkstring( l, 2 );
    int handler = luaL_ref( l, LUA_REGISTRYINDEX );

    engine->acceptor( "", [&]( fix_session_id id, const std::string& message ) {
        lua_rawgeti( l, LUA_REGISTRYINDEX, handler );
        lua_pushnumber( l, id );
        lua_pushstring( l, message.c_str() );
        lua_pcall( l, 2, 0, 0 );
    } );

    return 0;
}

int l_engine_send( lua_State* l )
{
    fix_engine* engine = l_check_fix_engine( l, 1 );
    int sess = luaL_checknumber( l, 2 );
    const char* message = luaL_checkstring( l, 3 );
    engine->send( sess, message );
    return 0;
}

int l_engine_start( lua_State* l )
{
    l_check_fix_engine( l, 1 )->start();
    return 0;
}

void l_register( lua_State* l )
{
    luaL_Reg engine_reg[] = {
        { "engine", l_new_engine },
        { "__gc", l_del_engine },
        { "acceptor", l_engine_acceptor },
        { "send", l_engine_send },
        { "start", l_engine_start },
        { NULL, NULL }
    };

    luaL_newmetatable( l, engine_id );
    luaL_setfuncs( l, engine_reg, 0 );
    lua_pushvalue( l, -1 );
    lua_setfield( l, -1, "__index" );
    lua_setglobal( l, "fix" );
}

int main( int argc, char** argv )
{
    lua_State* l = luaL_newstate();
    luaL_openlibs( l );
    l_register( l );

    int err = luaL_dofile( l, argv[1] );
    if( err ) {
        std::cerr << "lua error: " << luaL_checkstring( l, -1 ) << std::endl;
    }

    lua_close( l );
}
