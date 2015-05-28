#include "fixtk/engine.hpp"

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <mutex>
#include <thread>
#include <iostream>

fixtk::engine engine;
std::thread engine_thread;
std::mutex lua_mutex;

void engine_thread_fn()
{
    std::cout << "enter engine_thread_fn" << std::endl;
    engine.start();
    std::cout << "exit engine_thread_fn" << std::endl;
}

void push_fix( lua_State* l, fixtk::field_vector& flds )
{
    lua_newtable( l );
    for( int i=0; i<flds.size(); i++ )
    {
        fixtk::value& val = flds[i].value_;
        lua_pushinteger( l, flds[i].tag_ );
        lua_pushlstring( l, val.c_str(), val.size() );
        lua_settable( l, -3 );
    }
}

void pop_fix( lua_State* l, fixtk::field_vector& flds )
{
    lua_pushnil( l );
    while( lua_next( l, -2 ) != 0 )
    {
        flds.push_back( fixtk::field( lua_tointeger( l, -2 ), lua_tostring( l, -1 ) ) );
        lua_pop( l, 1 );
    }
}

int l_fix_acceptor( lua_State* l )
{
    std::lock_guard< std::mutex > lock( lua_mutex );

    const char* conn = luaL_checkstring( l, 1 );
    int handler = luaL_ref( l, LUA_REGISTRYINDEX );

    engine.acceptor( "", [=]( fixtk::session_id id, const std::string& message ) {
        std::lock_guard< std::mutex > lock( lua_mutex );
        lua_rawgeti( l, LUA_REGISTRYINDEX, handler );
        lua_pushnumber( l, id );
        lua_pushstring( l, message.c_str() );
        lua_pcall( l, 2, 0, 0 );
    } );

    if( !engine_thread.joinable() ) {
        engine_thread = std::thread( engine_thread_fn );
    }

    return 0;
}

int l_fix_initiator( lua_State* l )
{
    std::lock_guard< std::mutex > lock( lua_mutex );

    const char* conn = luaL_checkstring( l, 1 );
    int handler = luaL_ref( l, LUA_REGISTRYINDEX );

    fixtk::session_id id = engine.initiator( "", [=]( fixtk::session_id id, const std::string& message ) {
        ;
    } );

    if( !engine_thread.joinable() ) {
        engine_thread = std::thread( engine_thread_fn );
    }

    lua_pushinteger( l, id );

    return 1;
}

int l_fix_send( lua_State* l )
{
    std::lock_guard< std::mutex > lock( lua_mutex );

    int session = luaL_checknumber( l, 1 );
    fixtk::field_vector flds;
    pop_fix( l, flds );
    engine.send( session, flds );
    return 0;
}

int l_fix_recv( lua_State* l )
{
    std::lock_guard< std::mutex > lock( lua_mutex );
    return 0;
}

void l_register( lua_State* l )
{
    luaL_Reg fix_reg[] = {
        { "acceptor", l_fix_acceptor },
        { "initiator", l_fix_initiator },
        { "send", l_fix_send },
        { "recv", l_fix_recv },
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

    if( engine_thread.joinable() ) {
        engine_thread.join();
    }

    lua_close( l );
}
