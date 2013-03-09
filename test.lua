local lightningmdb_lib=require "lightningmdb"

local lightningmdb = _VERSION=="Lua 5.2" and lightningmdb_lib or lightningmdb

local function pt(t)
  for k,v in pairs(t) do
    print(k,v)
  end
end

print("Lightning MDB version:",lightningmdb.version())
print("Lightning error:",lightningmdb.strerror(0))

print("-- globals --")
pt(lightningmdb)

-- env
local e = lightningmdb.env_create()
print(e)
os.execute("mkdir ./temp/foo")
print(e:open("./temp/foo",0,420))
print("fixedmap",e.MDB_FIXEDMAP)
print("read only",e.MDB_RDONLY)

print("-- stats --")
pt(e:stat())

print("-- info --")
pt(e:info())
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
pt(t:stat(db))