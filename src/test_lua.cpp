#include "fix/engine.hpp"

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <mutex>
#include <thread>
#include <iostream>

std::thread engine_thread;
std::recursive_mutex lua_mutex;
fix::engine engine;

void engine_thread_fn()
{
    engine.start();
}

void l_push_message( lua_State* l, const fix::message& msg )
{
    int i=0;

    lua_newtable( l );
    msg.parse( [&]( fix::tag tag, const std::string& val ) {
        lua_pushinteger( l, ++i );
        lua_newtable( l );
        lua_pushinteger( l, 1 );
        lua_pushinteger( l, tag );
        lua_settable( l, -3 );
        lua_pushinteger( l, 2 );
        lua_pushstring( l, val.c_str() );
        lua_settable( l, -3 );
        lua_settable( l, -3 );
    } );
}

void l_pop_message( lua_State* l, fix::message& msg )
{
    int i=0;
    int tag;

    lua_pushnil( l );
    while( lua_next( l, -2 ) != 0 )
    {
        lua_pushnil( l );
        while( lua_next( l, -2 ) != 0 )
        {
            if( ++i % 2 == 1 ) {
                tag = (int)lua_tonumber( l, -1 );
            } else {
                msg.add( tag, lua_tostring( l, -1 ) );
            }
            lua_pop( l, 1 );
        }
        lua_pop( l, 1 );
    }
}

int l_acceptor( lua_State* l )
{
    std::lock_guard< std::recursive_mutex > lock( lua_mutex );

    const char* conn = luaL_checkstring( l, 1 );
    int handler = luaL_ref( l, LUA_REGISTRYINDEX );

    engine.acceptor( conn, [=]( fix::session_id id, const fix::message& msg ) {
        std::lock_guard< std::recursive_mutex > lock( lua_mutex );
        lua_rawgeti( l, LUA_REGISTRYINDEX, handler );
        lua_pushnumber( l, 1 );
        l_push_message( l, msg );
        lua_pcall( l, 2, 0, 0 );
    } );

    if( !engine_thread.joinable() ) {
        engine_thread = std::thread( engine_thread_fn );
    }

    return 0;
}

int l_send( lua_State* l )
{
    std::lock_guard< std::recursive_mutex > lock( lua_mutex );

    fix::message msg;

    int session = luaL_checknumber( l, 1 );
    const char* type = luaL_checkstring( l, 2 );
    l_pop_message( l, msg );

    engine.send( session, type, msg );

    return 0;
}

void l_register( lua_State* l )
{
    luaL_Reg fix_reg[] = {
        { "acceptor", l_acceptor },
        { "send", l_send },
        { NULL, NULL }
    };

    luaL_newmetatable( l, "fix" );
    luaL_setfuncs( l, fix_reg, 0 );
    lua_pushvalue( l, -1 );
    lua_setglobal( l, "fix" );
}

int main( int argc, char** argv )
{
    lua_State* l = luaL_newstate();
    luaL_openlibs( l );
    l_register( l );

    int err = luaL_dofile( l, argv[1] );
    if( err ) {
        std::cerr << "lua error: " << luaL_checkstring( l, -1 ) << std::endl; return 1;
    }

    lua_close( l );
}
