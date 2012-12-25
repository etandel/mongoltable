#include <stdio.h>
#include "lua.h"
#include "lauxlib.h"
#include "mongo.h"

//key of the connections table on the registry
#define CONNSKEY "mongoconns"

//connection params key names
#define HOSTKEY "host"
#define PORTKEY "port"
#define DBNAMEKEY "database"
#define COLLNAMEKEY "collection"

#define PARAMSIZE 15 //TODO: get a proper size

typedef struct _mongot {
    mongo *conn;
    char dbname[PARAMSIZE];
    char collname[PARAMSIZE];
} mongot_t;

static int con_get(lua_State *L){
    return 0;
}

static int con_set(lua_State *L){
    return 0;
}

static void get_strparam(lua_State *L, char *key, char *dest){
    lua_pushstring(L, key);
    lua_gettable(L, 2); //TODO: fix ugly magic number
    snprintf(dest, PARAMSIZE, "%s", lua_tostring(L, lua_gettop(L)));
    lua_pop(L, 1);
    if (!dest){
        lua_pushstring(L, "Invalid argument!"); //TODO: use lua buffer
        lua_error(L);
    }
}

static void get_conn_params(lua_State *L, char *host, int *port,
                            char *dbname, char *collname){
    int ok;

    luaL_checktype(L, 1, LUA_TTABLE);
    luaL_checktype(L, 2, LUA_TTABLE);

    get_strparam(L, HOSTKEY, host);
    get_strparam(L, DBNAMEKEY, dbname);
    get_strparam(L, COLLNAMEKEY, collname);
    
    lua_pushstring(L, PORTKEY);
    lua_gettable(L, 2);
    *port = (int)lua_tointegerx(L, lua_gettop(L), &ok);
    if (!ok) luaL_error(L, "invalid port!");
    lua_pop(L, 1);
}

static int con_bind(lua_State *L){
    int port, status;;
    char host[PARAMSIZE];
    mongot_t *mongot = (mongot_t*)malloc(sizeof(mongot_t));
    mongo *conn = (mongo*)malloc(sizeof(mongo)); 
    mongot->conn = conn;

    get_conn_params(L, host, &port, mongot->dbname, mongot->collname);
    mongo_init(conn);
    status = mongo_client(conn, host, port);
    if (status != MONGO_OK){
        char *errmsg;
        switch (conn->err){
            case MONGO_CONN_NO_SOCKET:
                errmsg = "no socket"; break;
            case MONGO_CONN_FAIL:
                errmsg = "connection failed"; break;
            case MONGO_CONN_NOT_MASTER:
                errmsg = "not master"; break;
        }
        mongo_destroy(conn);
        luaL_error(L, "mongo could not connect to %s at port %d: %s\n",
                   host, port, errmsg);
    }

    //set registry[conns][newmongoltable] = connection
    lua_pushstring(L, CONNSKEY);
    lua_gettable(L, LUA_REGISTRYINDEX);
    lua_pushvalue(L, 1);
    lua_pushlightuserdata(L, mongot);
    lua_settable(L, -3);

    return 0;
}

static int con_unbind(lua_State *L){
    mongot_t *mongot;
    luaL_checktype(L, 1, LUA_TTABLE);

    //close connection (close(reg[conns][tbl]))
    lua_pushstring(L, CONNSKEY);
    lua_gettable(L, LUA_REGISTRYINDEX);
    lua_pushvalue(L, 1);
    lua_gettable(L, -2);
    mongot = (mongot_t*)lua_touserdata(L, lua_gettop(L));
    mongo_destroy(mongot->conn);
    lua_pop(L, 1);

    //removes that table from entry
    lua_pushvalue(L, 1);
    lua_pushnil(L);
    lua_settable(L, -3);

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

