#include <stdio.h>
#include "lua.h"
#include "lauxlib.h"

//key of the connections table on the registry
#define CONNSKEY "mongoconns"

//connection params key names
#define HOSTKEY "host"
#define PORTKEY "port"
#define DBNAMEKEY "database"
#define COLLNAMEKEY "collection"

#define PARAMSIZE 15 //TODO: get a proper size

static int con_get(lua_State *L){
    return 0;
}

static int con_set(lua_State *L){
    return 0;
}

static void get_strparam(lua_State *L, char *key, char *dest){
    int top;

    lua_pushstring(L, key);
    lua_gettable(L, 2); //TODO: fix ugly magic number
    top = lua_gettop(L);
    snprintf(dest, PARAMSIZE, "%s", lua_tostring(L, top));
    if (!dest){
        lua_pushstring(L, "Invalid argument!"); //TODO: use lua buffer
        lua_error(L);
    }
    lua_remove(L, top);
}

static void get_conn_params(lua_State *L, char *host, unsigned int *port,
                          char *dbname, char *collname){
    int top = lua_gettop(L);

    luaL_checktype(L, 1, LUA_TTABLE);
    luaL_checktype(L, 2, LUA_TTABLE);

    get_strparam(L, HOSTKEY, host);
    get_strparam(L, DBNAMEKEY, dbname);
    get_strparam(L, COLLNAMEKEY, collname);

    lua_pushstring(L, PORTKEY);
    lua_gettable(L, 2);
    *port = (unsigned int) lua_tounsigned(L, top);
    if (!port){
        lua_pushstring(L, "Invalid port!");
        lua_error(L);
    }
    lua_remove(L, top);
}

static int con_bind(lua_State *L){
    unsigned int port;
    char host[PARAMSIZE], dbname[PARAMSIZE], collname[PARAMSIZE];

    get_conn_params(L, host, &port, dbname, collname);

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

