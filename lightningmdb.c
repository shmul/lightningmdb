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
#define TXN "lightningmdb_txn"

static int str_error_and_out(lua_State* L,const char* err) {
    lua_pushnil(L);
    lua_pushstring(L,err);
    return 2;
}

static int error_and_out(lua_State* L,int err) {
    return str_error_and_out(L,mdb_strerror(err));
}

static MDB_txn* check_txn(lua_State *L, int index) {
    MDB_txn* txn;
    luaL_checktype(L, index, LUA_TUSERDATA);
    txn = *(MDB_txn**)luaL_checkudata(L, index, TXN);
    if (txn == NULL) luaL_typerror(L,index,TXN);
    return txn;
}


static MDB_env* check_env(lua_State *L, int index) {
    MDB_env* env;
    luaL_checktype(L, index, LUA_TUSERDATA);
    env = *(MDB_env**)luaL_checkudata(L, index, ENV);
    if (env == NULL) luaL_typerror(L,index,ENV);
    return env;
}

/* env */
static int env_open(lua_State *L) {
    MDB_env* env = check_env(L,1);
    const char* path = luaL_checkstring(L,2);
    mode_t flags = (mode_t)luaL_checkinteger(L,3);
    int mode = luaL_checkinteger(L,4);
    int err;

    if ( !path ) {
        return str_error_and_out(L,"path required");
    }
    err = mdb_env_open(env,path,flags,mode);
    if ( err ) {
        return error_and_out(L,err);
    }
    lua_pushboolean(L,1);
    return 1;
}

static int env_copy(lua_State *L) {
    MDB_env* env = check_env(L,1);
    const char* path = luaL_checkstring(L,2);
    int err;

    if ( !path ) {
        return str_error_and_out(L,"path required");
    }
    err = mdb_env_copy(env,path);
    if ( err ) {
        return error_and_out(L,err);
    }
    lua_pushboolean(L,1);
    return 1;
}

static int env_stat(lua_State *L) {
    MDB_env* env = check_env(L,1);
    MDB_stat stat;
    mdb_env_stat(env,&stat);
    lua_newtable(L);
    lua_pushnumber(L,stat.ms_psize);
    lua_setfield(L,-2,"ms_psize");
    lua_pushnumber(L,stat.ms_depth);
    lua_setfield(L,-2,"ms_depth");
    lua_pushnumber(L,stat.ms_branch_pages);
    lua_setfield(L,-2,"ms_branch_pages");
    lua_pushnumber(L,stat.ms_leaf_pages);
    lua_setfield(L,-2,"ms_leaf_pages");
    lua_pushnumber(L,stat.ms_overflow_pages);
    lua_setfield(L,-2,"ms_overflow_pages");
    lua_pushnumber(L,stat.ms_entries);
    lua_setfield(L,-2,"ms_entries");
    return 1;
}

static int env_info(lua_State *L) {
    MDB_env* env = check_env(L,1);
    MDB_envinfo info;
    mdb_env_info(env,&info);
    lua_newtable(L);

    lua_pushnumber(L,info.me_mapsize);
    lua_setfield(L,-2,"me_mapsize");
    lua_pushnumber(L,info.me_last_pgno);
    lua_setfield(L,-2,"ms_last_pgno");
    lua_pushnumber(L,info.me_last_txnid);
    lua_setfield(L,-2,"me_last_txnid");
    lua_pushnumber(L,info.me_maxreaders);
    lua_setfield(L,-2,"me_maxreaders");
    lua_pushnumber(L,info.me_numreaders);
    lua_setfield(L,-2,"me_numreaders");
    return 1;

}

static int env_sync(lua_State *L) {
    MDB_env* env = check_env(L,1);
    int force = luaL_checkinteger(L,2);
    int err = mdb_env_sync(env,force);
    if ( err ) {
        return error_and_out(L,err);
    }
    lua_pushboolean(L,1);
    return 1;
}

static int env_close(lua_State *L) {
    MDB_env* env = check_env(L,1);
    mdb_env_close(env);
    lua_pushnil(L);
    lua_setmetatable(L,1);
    return 0;
}

static int env_set_flags(lua_State *L) {
    MDB_env* env = check_env(L,1);
    unsigned int flags = luaL_checkinteger(L,2);
    int onoff = luaL_checkinteger(L,3);
    int err = mdb_env_set_flags(env,flags,onoff);
    if ( err ) {
        return error_and_out(L,err);
    }
    lua_pushboolean(L,1);
    return 1;
}

static int env_get_flags(lua_State *L) {
    MDB_env* env = check_env(L,1);
    unsigned int flags = 0;
    int err = mdb_env_get_flags(env,&flags);
    if ( err ) {
        return error_and_out(L,err);
    }
    lua_pushinteger(L,flags);
    return 1;
}

static int env_get_path(lua_State *L) {
    MDB_env* env = check_env(L,1);
    const char* path;
    int err = mdb_env_get_path(env,&path);
    if ( err ) {
        return error_and_out(L,err);
    }
    lua_pushstring(L,path);
    return 1;
}

static int env_set_mapsize(lua_State *L) {
    // TODO
    return 0;
}

static int env_set_maxreaders(lua_State *L) {
    // TODO
    return 0;
}

static int env_get_maxreaders(lua_State *L) {
    // TODO
    return 0;
}

static int env_set_maxdbs(lua_State *L) {
    // TODO
    return 0;
}

static int env_txn_begin(lua_State* L) {
    MDB_env* env = check_env(L,1);
    MDB_txn* parent = !lua_isnil(L,2) ? check_txn(L,2) : NULL;
    unsigned int flags = luaL_checkinteger(L,3);
    int err;
    MDB_txn* txn;
    if ( !env ) {
        return str_error_and_out(L,"bad params");
    }
    err = mdb_txn_begin(env,parent,flags,&txn);
    if ( err ) {
        return error_and_out(L,err);
    }

    *(MDB_txn**)(lua_newuserdata(L,sizeof(MDB_txn*))) = txn;
    luaL_getmetatable(L,TXN);
    lua_setmetatable(L,-2);
    return 1;
}

static const luaL_reg env_methods[] = {
    {"__gc",env_close},
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
    {"txn_begin",env_txn_begin},
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
    lua_pushnumber(L,MDB_RDONLY);
    lua_setfield(L,-2,"MDB_RDONLY");
    lua_pushnumber(L,MDB_NOMETASYNC);
    lua_setfield(L,-2,"MDB_NOMETASYNC");
    lua_pushnumber(L,MDB_WRITEMAP);
    lua_setfield(L,-2,"MDB_WRITEMAP");
    lua_pushnumber(L,MDB_MAPASYNC);
    lua_setfield(L,-2,"MDB_MAPASYNC");

    lua_pushnumber(L,MDB_FIXEDMAP);
    lua_setfield(L,-2,"MDB_FIXEDMAP");
    lua_pushnumber(L,MDB_NOSUBDIR);
    lua_setfield(L,-2,"MDB_NOSUBDIR");
    lua_pushnumber(L,MDB_NOSYNC);
    lua_setfield(L,-2,"MDB_NOSYNC");


    return 1;
}

/* txn */

static int txn_commit(lua_State* L) {
    MDB_txn* txn = check_txn(L,1);
    int err = mdb_txn_commit(txn);

    if ( err ) {
        return error_and_out(L,err);
    }

    lua_pushnil(L);
    lua_setmetatable(L,1);
    lua_pushboolean(L,1);
    return 1;
}

static int txn_abort(lua_State* L) {
    MDB_txn* txn = check_txn(L,1);
    mdb_txn_abort(txn);

    lua_pushnil(L);
    lua_setmetatable(L,1);
    return 0;
}

static int txn_reset(lua_State* L) {
    MDB_txn* txn = check_txn(L,1);
    mdb_txn_reset(txn);
    return 0;
}

static int txn_renew(lua_State* L) {
    MDB_txn* txn = check_txn(L,1);
    mdb_txn_renew(txn);
    return 0;
}


static const luaL_reg txn_methods[] = {
    {"__gc",txn_abort}, /* if the transaction is properly committed, the sensible thing
                         * would be to abort it in gc */
    {"commit",txn_commit},
    {"abort",txn_abort},
    {"reset",txn_reset},
    {"renew",txn_renew},
    {0,0}
};


int txn_register(lua_State* L) {

    luaL_newmetatable(L,TXN);
    lua_setglobal(L,TXN);
    luaL_register(L,TXN,txn_methods);
    lua_settable(L,-1);

    luaL_getmetatable(L,TXN);
    lua_setfield(L,-1,"__index");

    return 1;
}
/* globals */
static int lmdb_version(lua_State *L) {
    const char* ver = mdb_version(NULL,NULL,NULL);
    lua_pushstring(L,ver);
    return 1;
}

static int lmdb_strerror(lua_State *L) {
    int err = luaL_checkinteger(L,1);
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
    txn_register(L);
    return 0;
}
