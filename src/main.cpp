#include "fixtk/engine.hpp"

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <iostream>

const char* engine_id = "luaL_engine";

engine* l_check_engine( lua_State* l, int n )
{
    return *(engine**)luaL_checkudata( l, n, engine_id );
}

int l_new_engine( lua_State* l )
{
    engine** udata = (engine**)lua_newuserdata( l, sizeof( engine* ) );
    *udata = new engine();
    luaL_getmetatable( l, engine_id );
    lua_setmetatable( l, -2 );
    return 1;
}

int l_del_engine( lua_State* l )
{
    delete l_check_engine( l, -1 );
    return 0;
}

int l_engine_acceptor( lua_State* l )
{
    engine* engine = l_check_engine( l, 1 );
    const char* conn = luaL_checkstring( l, 2 );
    int handler = luaL_ref( l, LUA_REGISTRYINDEX );

    engine->acceptor( "", [&]( session_id id, const std::string& message ) {
        lua_rawgeti( l, LUA_REGISTRYINDEX, handler );
        lua_pushnumber( l, id );
        lua_pushstring( l, message.c_str() );
        lua_pcall( l, 2, 0, 0 );
    } );

    return 0;
}

int l_engine_send( lua_State* l )
{
    engine* engine = l_check_engine( l, 1 );
    int sess = luaL_checknumber( l, 2 );
    const char* message = luaL_checkstring( l, 3 );
    engine->send( sess, message );
    return 0;
}

int l_engine_start( lua_State* l )
{
    l_check_engine( l, 1 )->start();
    return 0;
}

void l_register( lua_State* l )
{
    luaL_Reg engine_reg[] = {
        { "engine", l_new_engine },
        { "acceptor", l_engine_acceptor },
        { "send", l_engine_send },
        { "start", l_engine_start },
        { "__gc", l_del_engine },
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
