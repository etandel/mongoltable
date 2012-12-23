#include "lua.h"
#include "lauxlib.h"

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

/*
static const luaL_Reg R[] = {
    {"get",	con_get},
    {"set", con_set},
    {"bind", con_bind},
    {"unbind", con_unbind}
};
*/

LUALIB_API int luaopen_connections(lua_State *L)
{
/*
    luaL_newlib(L, R);
*/


    lua_createtable(L, 0, 4);

    lua_pushstring(L, "get");
    lua_pushcfunction(L, con_get);
    lua_settable(L, 3);

    lua_pushstring(L, "set");
    lua_pushcfunction(L, con_set);
    lua_settable(L, 3);

    lua_pushstring(L, "bind");
    lua_pushcfunction(L, con_bind);
    lua_settable(L, 3);

    lua_pushstring(L, "unbind");
    lua_pushcfunction(L, con_unbind);
    lua_settable(L, 3);


    return 1;
}
