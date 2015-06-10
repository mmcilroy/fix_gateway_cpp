#include "fix/message.hpp"

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <iostream>

int l_test( lua_State* L )
{
    fix::message msg;

    const char* val;
    int tag;
    int i=0;

    lua_pushnil( L );
    while( lua_next( L, -2 ) != 0 )
    {
        lua_pushnil( L );
        while( lua_next( L, -2 ) != 0 )
        {
            if( ++i % 2 == 1 ) {
                tag = (int)lua_tonumber( L, -1 );
            } else {
                val = lua_tostring( L, -1 );
                msg.add( tag, val );
            }
            lua_pop( L, 1 );
        }
        lua_pop( L, 1 );
    }

    std::cout << msg.str() << std::endl;

    return 0;
}

void l_register( lua_State* l )
{
    luaL_Reg fix_reg[] = {
        { "test", l_test },
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
