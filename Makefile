# makefile for lightning library for Lua
# based on lpack's

# change these to reflect your Lua installation
LUA=/usr/local
LUAINC= $(LUA)/include
LUALIB= $(LUA)/lib
LUABIN= $(LUA)/bin

LMDB=/Users/shmul/dev/remotes/mdb/libraries/liblmdb/
LMDBINC= $(LMDB)
LMDBLIB= $(LMDB)

# probably no need to change anything below here
WARN= -pedantic -Wall
CFLAGS= $(INCS) $(WARN) -O2 $G
LDFLAGS= -L$(LUALIB) -L$(LMDBLIB) -llmdb -undefined dynamic_lookup
INCS= -I$(LUAINC) -I$(LMDBINC)

MYNAME= lightningmdb
MYLIB= $(MYNAME)
T= $(MYNAME).so
OBJS= $(MYLIB).o
TEST= test.lua

all:	test

test:	$T
	$(LUABIN)/lua $(TEST)

o:	$(MYLIB).o

so:	$T

$T:	$(OBJS)
	$(CC) -o $@ -shared $(OBJS) $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(OBJS) $T core core.* a.out

doc:
	@echo "$(MYNAME) library:"
	@fgrep '/**' $(MYLIB).c | cut -f2 -d/ | tr -d '*' | sort | column

# distribution

FTP= $(HOME)/public/ftp/lua/5.1
D= $(MYNAME)
A= $(MYLIB).tar.gz
TOTAR= Makefile,README,$(MYLIB).c,test.lua

tar:	clean
	tar zcvf $A -C .. $D/{$(TOTAR)}

distr:	tar
	touch -r $A .stamp
	mv $A $(FTP)

diff:	clean
	tar zxf $(FTP)/$A
	diff $D .

# eof
