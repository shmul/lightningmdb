require "lightningmdb"

print("Lightning MDB version:",lightningmdb.version())
print("Lightning error:",lightningmdb.strerror(0))

-- env
local e = lightningmdb.env_create()
print(e)
--print(e:open())
