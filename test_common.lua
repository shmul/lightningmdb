lightningmdb_lib=require "lightningmdb"

lightningmdb = _VERSION>="Lua 5.2" and lightningmdb_lib or lightningmdb
MDB = setmetatable({}, {__index = function(t, k)
  return lightningmdb["MDB_" .. k]
end})

function pt(t)
  for k,v in pairs(t) do
    print(k,v)
  end
end

function ps(e)
  print("--- env stat")
  pt(e:stat())
  print("---")
end

function test_setup(dir_)
  local dir = "./temp/"..dir_
  os.execute("mkdir -p "..dir)
  os.execute("rm -rf "..dir.."/data.mdb")
  os.execute("rm -rf "..dir.."/lock.mdb")
  return dir
end
