#include "lua.h"
#include "lauxlib.h"

//key of the connections table on the registry
#define CONNSKEY "mongoconns"

static int con_get(lua_State *L){
    return 0;
}

static int con_set(lua_State *L){
    return 0;
}

static int con_bind(lua_State *L){
    return 0;
}

static int con_unbind(lua_State *L){
    return 0;
}

static const luaL_Reg R[] = {
    {"get",	con_get},
    {"set", con_set},
    {"bind", con_bind},
    {"unbind", con_unbind},
    {NULL, NULL},
};

LUALIB_API int luaopen_connections(lua_State *L)
{
    luaL_checkversion(L);

    //create connections table
    lua_pushstring(L, CONNSKEY);
    lua_newtable(L);
    lua_settable(L, LUA_REGISTRYINDEX);
    
    luaL_newlib(L, R);
    return 1;
}
