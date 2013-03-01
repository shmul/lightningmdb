require "lightningmdb"

print("Lightning MDB version:",lightningmdb.version())
print("Lightning error:",lightningmdb.strerror(0))

-- env
local e = lightningmdb.env_create()
print(e)
print(e:open("./temp/foo",0,436)) -- 436== octal 664
print("fixedmap",lightningmdb_env.MDB_FIXEDMAP)
print("read only",lightningmdb_env.MDB_RDONLY)

print("-- stats --")
for k,v in pairs(e:stat()) do
  print(k,v)
end

print("-- info --")
for k,v in pairs(e:info()) do
  print(k,v)
end

print("get_path",e:get_path())


--txn
local t = e:txn_begin(nil,0)
print("txn",t)
t:commit()
t = e:txn_begin(nil,0)
print("txn",t)
t:reset()
t:renew()
--t:abort()
local db = t:dbi_open(nil,0)
print("-- txn stat --")
for k,v in pairs(t:stat(db)) do
  print(k,v)
end
