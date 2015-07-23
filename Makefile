# makefile for lightning library for Lua
# based on lpack's

# change these to reflect your Lua installation
LUAINC= /usr/local/include/
LUALIB= /usr/local/lib/
LUABIN= /usr/local/bin

LMDBINC= /usr/local/include
LMDBLIB= /usr/local/lib

# probably no need to change anything below here
platform=$(shell uname)

ifeq ($(platform),Linux)
  PLATFORM_CFLAGS= -fPIC
else
  PLATFORM_CFLAGS=
endif

ifeq ($(platform),Darwin)
  PLATFORM_LDFLAGS= -undefined dynamic_lookup
else
  PLATFORM_LDFLAGS=
endif

WARN= -pedantic -Wall
CFLAGS= $(INCS) $(WARN) $G -g -O2 $(PLATFORM_CFLAGS)
LDFLAGS= -L$(LUALIB) -L$(LMDBLIB) -llmdb $(PLATFORM_LDFLAGS)
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

D= $(MYNAME)
A= $(MYLIB).tar.gz
TOTAR= Makefile,README.md,$(MYLIB).c,test*.lua

tar:	clean
	tar zcvf $A -C .. $D/{$(TOTAR)}

distr:	tar
	touch -r $A .stamp

diff:	clean
	tar zxf $(FTP)/$A
	diff $D .

# eof
