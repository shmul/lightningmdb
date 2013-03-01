require "lightningmdb"

print("Lightning MDB version:",lightningmdb.version())
print("Lightning error:",lightningmdb.strerror(0))

-- env
local e = lightningmdb.env_create()
print(e)
print(e:open("./temp/foo",0,436)) -- 436== octal 664
print("fixedmap",lightningmdb_env.MDB_FIXEDMAP)
print("read only",lightningmdb_env.MDB_RDONLY)
