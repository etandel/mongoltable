#include <stdio.h>
#include <stdlib.h>

#include "lua.h"
#include "lauxlib.h"
#include "mongo.h"
#include "bson.h"


#define PARAMSIZE 15 //TODO: get a proper size


static int con_open(lua_State *L){
    int port, status;
    const char *host;
    mongo *conn;

    //mongoc copies value, so no worries about string being collected
    host =  luaL_checkstring(L, 1);
    port = luaL_checkint(L, 2);

    conn = (mongo*)malloc(sizeof(mongo)); 
    mongo_init(conn);
    status = mongo_client(conn, host, port);
    if (status != MONGO_OK){
        const char *errmsg;
        switch (conn->err){
            case MONGO_CONN_NO_SOCKET:
                errmsg = "no socket"; break;
            case MONGO_CONN_FAIL:
                errmsg = "connection failed"; break;
            case MONGO_CONN_NOT_MASTER:
                errmsg = "not master"; break;
            default:
                errmsg = ""; break;
        }
        mongo_destroy(conn);
        luaL_error(L, "mongo could not connect to %s at port %d: %s\n",
                   host, port, errmsg);
    }
    lua_pushlightuserdata(L, conn);

    return 1;
}


static int con_close(lua_State *L){
    mongo_destroy((mongo*)lua_touserdata(L, 1));
    return 0;
}


static int con_get(lua_State *L){
    const char *collpath, *key;
    mongo *conn;
    mongo_cursor cursor[1];
    bson_iterator iter[1];
    bson q[1];
    bson_type rtype;

    conn = (mongo*) lua_touserdata(L, 1);
    collpath = luaL_checkstring(L, 2);
    key = luaL_checkstring(L, 3);

    bson_init(q);
        bson_append_string(q, "_id", key);
    bson_finish(q);

    mongo_cursor_init(cursor, conn, collpath);
    mongo_cursor_set_query(cursor, q);
    if (mongo_cursor_next(cursor) != MONGO_OK){
        lua_pushnil(L);
    }
    else{
        //collection it should've been created using mongoltables, so other types
        //cannot exist
        switch (bson_find(iter, mongo_cursor_bson(cursor), key)){
            case BSON_DOUBLE:
                lua_pushnumber(L, (lua_Number)bson_iterator_double(iter));
                break;

            case BSON_OBJECT:
                //TODO: properly implement this
                lua_newtable(L);
                break;

            case BSON_STRING:
                lua_pushstring(L, bson_iterator_string(iter));
                break;

            case BSON_BOOL:
                lua_pushboolean(L, (int)bson_iterator_bool_raw(iter));
                break;
        }
    }

    return 1;
}


static int con_set(lua_State *L){
    const char *collpath, *key;
    mongo *conn;
    bson cond[1], op[1];

    conn = (mongo*) lua_touserdata(L, 1);
    collpath = luaL_checkstring(L, 2);
    key = luaL_checkstring(L, 3);

    bson_init(cond);
        bson_append_string(cond, "_id", key);
    bson_finish(cond);

    bson_init(op);
        bson_append_string(op, "_id", key);
        switch(lua_type(L, 4)){
            case LUA_TSTRING:
                bson_append_string(op, key, lua_tostring(L, 4));
                break;

            case LUA_TNUMBER:
                bson_append_double(op, key, (double)lua_tonumber(L, 4));
                break;

            case LUA_TTABLE:
                //TODO: implement this
                bson_append_start_object(op, key);
                bson_append_finish_object(op);
                break;

            case LUA_TBOOLEAN:
                bson_append_bool(op, key, (bson_bool_t)lua_toboolean(L, 4));
                break;
        }
    bson_finish(op); //TODO: Handle possible errors

    mongo_update(conn, collpath, cond, op, MONGO_UPDATE_UPSERT, NULL);
    bson_destroy(op);

    return 0;
}


static const luaL_Reg R[] = {
    {"get",	con_get},
    {"set", con_set},
    {"open", con_open},
    {"close", con_close},
    {NULL, NULL},
};


LUALIB_API int luaopen_mongoconn(lua_State *L)
{
    luaL_checkversion(L);
    luaL_newlib(L, R);
    return 1;
}

