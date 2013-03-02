/*
 * Shmulik Regev <shmulbox@gmail.com>
 */

#include <ctype.h>
#include <string.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "lmdb.h"

#define LIGHTNING "lightningmdb"
#define ENV "lightningmdb_env"
#define TXN "lightningmdb_txn"
#define CURSOR "lightningmdb_cursor"

#define setfield_enum(x) lua_pushnumber(L,x); lua_setfield(L,-2,#x)

static int str_error_and_out(lua_State* L,const char* err) {
    lua_pushnil(L);
    lua_pushstring(L,err);
    return 2;
}

static int error_and_out(lua_State* L,int err) {
    return str_error_and_out(L,mdb_strerror(err));
}

static int success_or_err(lua_State* L,int err) {
    if ( err ) {
        return error_and_out(L,err);
    }
    lua_settop(L,1);
    return 1;
}

static int unimplemented(lua_State* L) {
    return str_error_and_out(L,"method unimplemented");
}

#define DEFINE_check(x,NAME)                              \
    static MDB_##x* check_##x(lua_State *L, int index) {  \
        MDB_##x* y;                                       \
        luaL_checktype(L, index, LUA_TUSERDATA);          \
        y = *(MDB_##x**)luaL_checkudata(L, index, NAME);  \
        if (y == NULL) luaL_typerror(L,index,NAME);       \
        return y;                                         \
    }

DEFINE_check(env,ENV)
DEFINE_check(txn,TXN)
DEFINE_check(cursor,CURSOR)

static int stat_to_table(lua_State *L,MDB_stat *stat) {
    lua_newtable(L);
    lua_pushnumber(L,stat->ms_psize);
    lua_setfield(L,-2,"ms_psize");
    lua_pushnumber(L,stat->ms_depth);
    lua_setfield(L,-2,"ms_depth");
    lua_pushnumber(L,stat->ms_branch_pages);
    lua_setfield(L,-2,"ms_branch_pages");
    lua_pushnumber(L,stat->ms_leaf_pages);
    lua_setfield(L,-2,"ms_leaf_pages");
    lua_pushnumber(L,stat->ms_overflow_pages);
    lua_setfield(L,-2,"ms_overflow_pages");
    lua_pushnumber(L,stat->ms_entries);
    lua_setfield(L,-2,"ms_entries");
    return 1;
}


static void pop_val(lua_State* L,int index,MDB_val* val) {
    val->mv_data = (void*)luaL_checklstring(L,index,&val->mv_size);
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
    return success_or_err(L,err);
}

static int env_copy(lua_State *L) {
    MDB_env* env = check_env(L,1);
    const char* path = luaL_checkstring(L,2);
    int err;

    if ( !path ) {
        return str_error_and_out(L,"path required");
    }
    err = mdb_env_copy(env,path);
    return success_or_err(L,err);
}

static int env_stat(lua_State *L) {
    MDB_env* env = check_env(L,1);
    MDB_stat stat;
    mdb_env_stat(env,&stat);
    return stat_to_table(L,&stat);
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
    return success_or_err(L,err);
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
    return success_or_err(L,err);
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
    return unimplemented(L);
}

static int env_set_maxreaders(lua_State *L) {
// TODO
    return unimplemented(L);
}

static int env_get_maxreaders(lua_State *L) {
// TODO
    return unimplemented(L);
}

static int env_set_maxdbs(lua_State *L) {
// TODO
    return unimplemented(L);
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

static int env_dbi_close(lua_State* L) {
    MDB_env* env = check_env(L,1);
    MDB_dbi dbi = luaL_checkinteger(L,2);

    mdb_dbi_close(env,dbi);
    return 0;
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
    {"dbi_close",env_dbi_close},
    {0,0}
};


int env_register(lua_State* L) {
    luaL_newmetatable(L,ENV);
    lua_setglobal(L,ENV);
    luaL_register(L,ENV,env_methods);
    lua_settable(L,-1);

    luaL_getmetatable(L,ENV);
    lua_setfield(L,-1,"__index");

    luaL_getmetatable(L,ENV);
    setfield_enum(MDB_FIXEDMAP);
    setfield_enum(MDB_NOSUBDIR);
    setfield_enum(MDB_RDONLY);
    setfield_enum(MDB_WRITEMAP);
    setfield_enum(MDB_NOMETASYNC);
    setfield_enum(MDB_NOSYNC);
    setfield_enum(MDB_MAPASYNC);

    return 1;
}

/* cursor */
static int cursor_close(lua_State *L) {
    MDB_cursor* cursor = check_cursor(L,1);
    mdb_cursor_close(cursor);
    lua_pushnil(L);
    lua_setmetatable(L,1);
    return 0;
}

static int cursor_txn(lua_State *L) {
    MDB_cursor* cursor = check_cursor(L,1);
    MDB_txn* txn = mdb_cursor_txn(cursor);
    (void)txn;
    // TODO - we'll need a mapping from MDB_txn* to the lua userdata
    return unimplemented(L);
}

static int cursor_dbi(lua_State *L) {
    MDB_cursor* cursor = check_cursor(L,1);
    MDB_dbi dbi = mdb_cursor_dbi(cursor);
    lua_pushinteger(L,dbi);
    return 1;
}

static int cursor_get(lua_State *L) {
    MDB_cursor* cursor = check_cursor(L,1);
    MDB_val k,v;
    MDB_cursor_op op = luaL_checkinteger(L,3);
    int err;
    pop_val(L,2,&k);

    err = mdb_cursor_get(cursor,&k,&v,op);
    switch (err) {
    case MDB_NOTFOUND:
        lua_pushnil(L);
        return 1;
    case 0:
        lua_pushlstring(L,v.mv_data,v.mv_size);
        return 1;
    }
    return error_and_out(L,err);
}

static int cursor_put(lua_State *L) {
    MDB_cursor* cursor = check_cursor(L,1);
    MDB_val k,v;
    unsigned int flags = luaL_checkinteger(L,4);
    int err;
    pop_val(L,2,&k);
    pop_val(L,3,&v);

    err = mdb_cursor_put(cursor,&k,&v,flags);
    return success_or_err(L,err);
}

static int cursor_del(lua_State *L) {
    MDB_cursor* cursor = check_cursor(L,1);
    unsigned int flags = luaL_checkinteger(L,2);
    int err = mdb_cursor_del(cursor,flags);
    return success_or_err(L,err);
}

static int cursor_count(lua_State *L) {
    MDB_cursor* cursor = check_cursor(L,1);
    size_t count = 0;
    int err = mdb_cursor_count(cursor,&count);
    if ( err ) {
        return error_and_out(L,err);
    }
    lua_pushinteger(L,count);
    return 1;
}

static const luaL_reg cursor_methods[] = {
    {"__gc",cursor_close},
    {"close",cursor_close},
    {"txn",cursor_txn},
    {"dbi",cursor_dbi},
    {"get",cursor_get},
    {"put",cursor_put},
    {"del",cursor_del},
    {"count",cursor_count},

    {0,0}
};

int cursor_register(lua_State* L) {

    luaL_newmetatable(L,CURSOR);
    lua_setglobal(L,CURSOR);
    luaL_register(L,CURSOR,cursor_methods);
    lua_settable(L,-1);

    luaL_getmetatable(L,CURSOR);
    lua_setfield(L,-1,"__index");

    luaL_getmetatable(L,CURSOR);

    setfield_enum(MDB_FIRST);
    setfield_enum(MDB_FIRST_DUP);
    setfield_enum(MDB_GET_BOTH);
    setfield_enum(MDB_GET_BOTH_RANGE);
    setfield_enum(MDB_GET_CURRENT);
    setfield_enum(MDB_GET_MULTIPLE);
    setfield_enum(MDB_LAST);
    setfield_enum(MDB_LAST_DUP);
    setfield_enum(MDB_NEXT);
    setfield_enum(MDB_NEXT_DUP);
    setfield_enum(MDB_NEXT_MULTIPLE);
    setfield_enum(MDB_NEXT_NODUP);
    setfield_enum(MDB_PREV);
    setfield_enum(MDB_PREV_DUP);
    setfield_enum(MDB_PREV_NODUP);
    setfield_enum(MDB_SET);
    setfield_enum(MDB_SET_KEY);
    setfield_enum(MDB_SET_RANGE );

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

static int txn_dbi_open(lua_State* L) {
    MDB_txn* txn = check_txn(L,1);
    const char* name = !lua_isnil(L,2) ? luaL_checkstring(L,2) : NULL;
    unsigned int flags = luaL_checkinteger(L,3);
		MDB_dbi dbi;
    int err = mdb_dbi_open(txn,name,flags,&dbi);
    if ( err ) {
        return error_and_out(L,err);
    }

    lua_pushinteger(L,dbi);
    return 1;
}

static int txn_stat(lua_State* L) {
    MDB_txn* txn = check_txn(L,1);
    MDB_dbi dbi = luaL_checkinteger(L,2);
    MDB_stat stat;
    mdb_stat(txn,dbi,&stat);
    return stat_to_table(L,&stat);
}

static int txn_dbi_drop(lua_State* L) {
    MDB_txn* txn = check_txn(L,1);
    MDB_dbi dbi = luaL_checkinteger(L,2);
    int del = luaL_checkinteger(L,1);
    int err = mdb_drop(txn,dbi,del);
    return success_or_err(L,err);
}

static int txn_get(lua_State* L) {
    MDB_txn* txn = check_txn(L,1);
    MDB_dbi dbi = luaL_checkinteger(L,2);
    MDB_val k,v;
    pop_val(L,3,&k);

    int err;
    err = mdb_get(txn,dbi,&k,&v);
    switch (err) {
    case MDB_NOTFOUND:
        lua_pushnil(L);
        return 1;
    case 0:
        lua_pushlstring(L,v.mv_data,v.mv_size);
        return 1;
    }
    return error_and_out(L,err);
}

static int txn_put(lua_State* L) {
    MDB_txn* txn = check_txn(L,1);
    MDB_dbi dbi = luaL_checkinteger(L,2);
    MDB_val k,v;
    unsigned int flags = luaL_checkinteger(L,5);
    int err;
    pop_val(L,3,&k);
    pop_val(L,4,&v);

    err = mdb_put(txn,dbi,&k,&v,flags);
    return success_or_err(L,err);
}

static int txn_del(lua_State* L) {
    MDB_txn* txn = check_txn(L,1);
    MDB_dbi dbi = luaL_checkinteger(L,2);
    MDB_val k,v;
    int err;
    pop_val(L,3,&k);
    pop_val(L,4,&v);

    err = mdb_del(txn,dbi,&k,&v);
    return success_or_err(L,err);
}

static int txn_cursor_open(lua_State* L) {
    MDB_txn* txn = check_txn(L,1);
    MDB_dbi dbi = luaL_checkinteger(L,2);
    MDB_cursor* cursor;
    int err = mdb_cursor_open(txn,dbi,&cursor);
    if ( err ) {
        return error_and_out(L,err);
    }

    *(MDB_cursor**)(lua_newuserdata(L,sizeof(MDB_cursor*))) = cursor;
    luaL_getmetatable(L,CURSOR);
    lua_setmetatable(L,-2);
    return 1;
}

static int txn_cursor_renew(lua_State *L) {
    MDB_txn* txn = check_txn(L,1);
    MDB_cursor* cursor = check_cursor(L,2);
    int err = mdb_cursor_renew(txn,cursor);
    return success_or_err(L,err);
}

static const luaL_reg txn_methods[] = {
    {"__gc",txn_abort}, /* if the transaction is properly committed, the sensible thing
                         * would be to abort it in gc */
    {"commit",txn_commit},
    {"abort",txn_abort},
    {"reset",txn_reset},
    {"renew",txn_renew},
    {"dbi_open",txn_dbi_open},
    {"stat",txn_stat},
    {"dbi_drop",txn_dbi_drop},
    {"get",txn_get},
    {"put",txn_put},
    {"del",txn_del},
    {"cursor_open",txn_cursor_open},
    {"cursor_renew",txn_cursor_renew},
    {0,0}
};


int txn_register(lua_State* L) {

    luaL_newmetatable(L,TXN);
    lua_setglobal(L,TXN);
    luaL_register(L,TXN,txn_methods);
    lua_settable(L,-1);

    luaL_getmetatable(L,TXN);
    lua_setfield(L,-1,"__index");


    luaL_getmetatable(L,TXN);
    setfield_enum(MDB_REVERSEKEY);
    setfield_enum(MDB_DUPSORT);
    setfield_enum(MDB_INTEGERKEY);
    setfield_enum(MDB_DUPFIXED);
    setfield_enum(MDB_INTEGERDUP);
    setfield_enum(MDB_REVERSEDUP);
    setfield_enum(MDB_CREATE);
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
                  LIGHTNING,
                  globals);

    setfield_enum(MDB_NOOVERWRITE);
    setfield_enum(MDB_NODUPDATA);
    setfield_enum(MDB_CURRENT);
    setfield_enum(MDB_RESERVE);
    setfield_enum(MDB_APPEND);
    setfield_enum(MDB_APPENDDUP);
    setfield_enum(MDB_MULTIPLE);

    env_register(L);
    txn_register(L);
    cursor_register(L);
    return 0;
}
