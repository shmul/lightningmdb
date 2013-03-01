/*
 * Shmulik Regev <shmulbox@gmail.com>
 */

#include <ctype.h>
#include <string.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "lmdb.h"
#define ENV "lightningmdb_env"

static int str_error_and_out(lua_State* L,const char* err) {
    lua_pushnil(L);
    lua_pushstring(L,err);
    return 2;
}

static int error_and_out(lua_State* L,int err) {
    return str_error_and_out(L,mdb_strerror(err));
}

/* env */
static MDB_env* check_env(lua_State *L, int index) {
    MDB_env* env;
    luaL_checktype(L, index, LUA_TUSERDATA);
    env = *(MDB_env**)luaL_checkudata(L, index, ENV);
    if (env == NULL) luaL_typerror(L,index,ENV);
    return env;
}


static int env_gc(lua_State *L) {
    MDB_env* env = check_env(L,1);
    if ( !env ) {
        return str_error_and_out(L,"bad userdata");
    }
    mdb_env_close(env);
    lua_pushnil(L);
    lua_setmetatable(L,1);
    return 0;
}

static int env_open(lua_State *L) {
    MDB_env* env = check_env(L,1);
    const char* path = luaL_checkstring(L,2);
    mode_t flags = (mode_t)luaL_checknumber(L,3);
    int mode = (int)luaL_checknumber(L,4);
    int err;

    if ( !env || !path ) {
        return str_error_and_out(L,"bad params");
    }
    err = mdb_env_open(env,path,flags,mode);
    if ( err ) {
        return error_and_out(L,err);
    }
    lua_pushboolean(L,1);
    return 1;
}

static int env_copy(lua_State *L) {
    return 0;
}

static int env_stat(lua_State *L) {
    return 0;
}

static int env_info(lua_State *L) {
    return 0;
}

static int env_sync(lua_State *L) {
    return 0;
}

static int env_close(lua_State *L) {
    return 0;
}

static int env_set_flags(lua_State *L) {
    return 0;
}

static int env_get_flags(lua_State *L) {
    return 0;
}

static int env_get_path(lua_State *L) {
    return 0;
}

static int env_set_mapsize(lua_State *L) {
    return 0;
}

static int env_set_maxreaders(lua_State *L) {
    return 0;
}

static int env_get_maxreaders(lua_State *L) {
    return 0;
}

static int env_set_maxdbs(lua_State *L) {
    return 0;
}


static const luaL_reg env_methods[] = {
    {"__gc",env_gc},
    {"open",env_open},
    {"copy",env_copy},
    {"stat",env_stat},
    {"info",env_info},
    {"sync",env_sync},
    {"close",env_close},
    {"set_flags",env_set_flags},
    {"get_flags",env_get_flags},
    {"get_path",env_get_path},
    {"set_mapsize",env_set_mapsize},
    {"set_maxreaders",env_set_maxreaders},
    {"get_maxreaders",env_get_maxreaders},
    {"set_maxdbs",env_set_maxdbs},
    {0,0}
};


int env_register(lua_State* L) {
/*
  luaL_openlib(L,ENV,env_methods,0);
  luaL_newmetatable(L,ENV);
  luaL_openlib(L,0,env_meta,0);
  lua_pushliteral(L,"__index");
  lua_pushvalue(L,-3);
  lua_rawset(L,-3);
  lua_pushliteral(L,"__metatable");
  lua_pushvalue(L,-3);
  lua_rawset(L,-3);
  lua_pop(L,1);
*/

    luaL_newmetatable(L,ENV);
    lua_setglobal(L,ENV);
    luaL_register(L,ENV,env_methods);
    lua_settable(L,-1);

    luaL_getmetatable(L,ENV);
    lua_setfield(L,-1,"__index");

    luaL_getmetatable(L,ENV);
    lua_pushnumber(L,MDB_FIXEDMAP);
    lua_setfield(L,-2,"MDB_FIXEDMAP");

    lua_pushnumber(L,MDB_NOSUBDIR);
    lua_setfield(L,-2,"MDB_NOSUBDIR");
    lua_pushnumber(L,MDB_NOSYNC);
    lua_setfield(L,-2,"MDB_NOSYNC");
    lua_pushnumber(L,MDB_RDONLY);
    lua_setfield(L,-2,"MDB_RDONLY");
    lua_pushnumber(L,MDB_NOMETASYNC);

                   lua_setfield(L,-2,"MDB_NOMETASYNC");
                   lua_pushnumber(L,MDB_WRITEMAP);
                   lua_setfield(L,-2,"MDB_WRITEMAP");
                   lua_pushnumber(L,MDB_MAPASYNC);
                   lua_setfield(L,-2,"MDB_MAPASYNC");

                   return 1;
}

/* globals */
static int lmdb_version(lua_State *L) {
    const char* ver = mdb_version(NULL,NULL,NULL);
    lua_pushstring(L,ver);
    return 1;
}

static int lmdb_strerror(lua_State *L) {
    int err = (int)luaL_checknumber(L,1);
    lua_pushstring(L,mdb_strerror(err));
    return 1;
}

static int lmdb_env_create(lua_State *L) {
    MDB_env* env = NULL;
    int err = mdb_env_create(&env);
    if ( err ) {
        lua_pushnil(L);
        lua_pushstring(L,mdb_strerror(err));
        return 2;
    }
    *(MDB_env**)(lua_newuserdata(L,sizeof(MDB_env*))) = env;
    luaL_getmetatable(L,ENV);
    lua_setmetatable(L,-2);
    return 1;
}

static const luaL_reg globals[] = {
    {"version",lmdb_version},
    {"strerror",lmdb_strerror},
    {"env_create",lmdb_env_create},
    {NULL,	NULL}
};



int luaopen_lightningmdb(lua_State *L) {
    luaL_register(L,
                  "lightningmdb",
                  globals);
    env_register(L);
    return 0;
}
