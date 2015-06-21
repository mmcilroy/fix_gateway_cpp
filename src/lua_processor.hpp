#pragma once

#include "event.hpp"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

extern fix::session_id connect( const std::string&, const fix::header& );
extern void accept( const std::string& );
extern void send( fix::session_id, const std::string&, const fix::message& );

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

int l_accept( lua_State* l )
{
    accept( luaL_checkstring( l, 1 ) );
    return 0;
}

int l_connect( lua_State* l )
{
    fix::header hdr;
    hdr.protocol_ = "FIX.4.4";
    hdr.sender_ = "SENDER";
    hdr.target_ = "TARGET";

    lua_pushinteger( l, connect( luaL_checkstring( l, 1 ), hdr ) );

    return 1;
}

int l_send( lua_State* l )
{
    fix::message msg;
    int session = luaL_checknumber( l, 1 );
    const char* type = luaL_checkstring( l, 2 );
    l_pop_message( l, msg );
    send( session, type, msg );

    return 0;
}

void l_register( lua_State* l )
{
    luaL_Reg fix_reg[] = {
        { "accept", l_accept },
        { "connect", l_connect },
        { "send", l_send },
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
    fix::event_publisher& out_;

public:
    lua_processor( const std::string& file, fix::event_publisher& out ) :
        lua_( luaL_newstate() ),
        out_( out )
    {
        luaL_openlibs( lua_ );
        l_register( lua_ );
        int err = luaL_dofile( lua_, file.c_str() );
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
        lua_getglobal( lua_, "on_init" );
        lua_call( lua_, 0, 0 );
    }

    void on_event( const fix::event& ev )
    {
        lua_getglobal( lua_, "on_event" );
        lua_pushnumber( lua_, ev.session_ );
        l_push_message( lua_, ev.message_ );
        lua_call( lua_, 2, 0 );
    }
};
