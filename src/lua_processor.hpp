#pragma once

#include "event.hpp"

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

int l_acceptor( lua_State* l )
{
    std::lock_guard< std::recursive_mutex > lock( lua_mutex );

    const char* conn = luaL_checkstring( l, 1 );
    int handler = luaL_ref( l, LUA_REGISTRYINDEX );

    engine.acceptor( conn, [=]( fix::session_id id, const fix::message& msg ) {
        std::lock_guard< std::recursive_mutex > lock( lua_mutex );
        lua_rawgeti( l, LUA_REGISTRYINDEX, handler );
        lua_pushnumber( l, id );
        l_push_message( l, msg );
        lua_pcall( l, 2, 0, 0 );
    } );

    if( !engine_thread.joinable() ) {
        engine_thread = std::thread( engine_thread_fn );
    }

    return 0;
}

int l_initiator( lua_State* l )
{
    std::lock_guard< std::recursive_mutex > lock( lua_mutex );

    fix::message msg;
    fix::header hdr;
    hdr.protocol_ = "FIX.4.4";
    hdr.sender_ = "SENDER";
    hdr.target_ = "TARGET";

    const char* conn = luaL_checkstring( l, 1 );
    //l_pop_message( l, msg );

    fix::session_id id = engine.initiator( conn, hdr, [=]( fix::session_id id, const fix::message& msg ) {
        msg.parse( [&]( fix::tag t, const std::string& v ) {
            std::cout << "parsed " << t << "=" << v << std::endl;
        } );
    } );

    if( !engine_thread.joinable() ) {
        engine_thread = std::thread( engine_thread_fn );
    }

    lua_pushinteger( l, id );

    return 1;
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
        /*
        { "accept", l_accept },
        { "connect", l_connect },
        { "send", l_send },
        { "time", l_time },
        */
        { NULL, NULL }
    };

    luaL_newmetatable( l, "fix" );
    luaL_setfuncs( l, fix_reg, 0 );
    lua_pushvalue( l, -1 );
    lua_setglobal( l, "fix" );
}

class lua_processor
{
private:
    lua_State* lua_;
    event_publisher& out_;

public:
    lua_processor( fix::event_publisher& out ) :
        lua_( luaL_newstate() ),
        out_( out )
    {
        luaL_openlibs( lua_ );
        l_register( lua_ );

        int err = luaL_dofile( lua_, argv[1] );
        if( err ) {
            std::cerr << "lua error: " << luaL_checkstring( lua_, -1 ) << std::endl;
        }
    }

    ~lua_processor()
    {
        lua_close( lua_ );
    }

    void on_init()
    {
    }

    void on_event( const fix::event& )
    {
    }
};
