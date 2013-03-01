/*
* Shmulik Regev <shmulbox@gmail.com>
*/

#include <ctype.h>
#include <string.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "lmdb.h"

static int lmdb_version(lua_State *L) {
    const char* ver = mdb_version(NULL,NULL,NULL);
    lua_pushstring(L,ver);
    return 1;
}


static const luaL_reg globals[] = {
	{"version", lmdb_version},
	{NULL,	NULL}
};



int luaopen_lightningmdb(lua_State *L) {
    luaL_register(L,
                  "lightningmdb",
                  globals);
    return 0;
}
