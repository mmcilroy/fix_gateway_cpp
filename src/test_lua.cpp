#include "fix/message.hpp"

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <iostream>

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
    const char* conn = luaL_checkstring( l, 1 );
    int handler = luaL_ref( l, LUA_REGISTRYINDEX );

    lua_rawgeti( l, LUA_REGISTRYINDEX, handler );
    lua_pushnumber( l, 1 );
    l_push_message( l, fix::message( "1=one|2=two|3=three|" ) );
    lua_pcall( l, 2, 0, 0 );
}

int l_send( lua_State* l )
{
    int session = luaL_checknumber( l, 1 );
    const char* type = luaL_checkstring( l, 2 );
    fix::message msg;
    l_pop_message( l, msg );

    std::cout << "send: " << session << ", " << type << ", " << msg.str() << std::endl;
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
