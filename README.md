# General

Lightningmdb is a thin wrapper around [OpenLDAP Lightning Memory-Mapped Database (LMDB)](http://symas.com/mdb/) .


# Installation

## Prerequisites

* Lua 5.1.x or greater. It should be compatible with Luajit though it wasn't thoroughly tested.
* [OpenLDAP Lightning Memory-Mapped Database (LMDB)](http://symas.com/mdb/). A clone of just the LMDB code is also available on [Github](https://github.com/LMDB/lmdb)

## Building
### LMDB
* Get LMDB by `git clone https://github.com/LMDB/lmdb`
* Building LMDB is a simple
```
cd lmdb/libraries/liblmdb/
make
make install
```
### Lightningmdb
#### Luarocks
`luarocks` can be used to build and install the files from source.

#### Manually
* edit the Lightningmdb `Makefile` and set the Lua and Lightningmdb paths.
* run `make` to generate the library.

#### Docker
A set of docker files are provided also, primarily for building the library against multiple Lua versions. Run `./docker/build_container.sh lua5.1` to test that the library successfully builds with Lua 5.1 (versions 5.2 and 5.3 are supported as well).

# Usage
Every attempt was made to honor the original naming convention. The documentation is therefore scarce and the [database's documentation](http://symas.com/mdb/doc/) should be used.

5 lua _objects_ are wrapping the access to the DB. Their mappings to the LMDB functions/constants is provided below.

The (Lua) tests files provide usage reference. Some of them are direct translation of LMDB's test files.

## lightningmdb
All the LMDB enums and defines are available through this table as well as the following functions:

* `version` - `lmdb_version`
* `strerror` - `mdb_strerror`
* `env_create` - `mdb_env_create`

## env
* `open` - `mdb_env_open`
* `copy` - `mmddbb__env_copy`
* `stat` - `mdb_env_stat`
* `info` - `mdb_env_info`
* `sync` - `mdb_env_sync`
* `close` - `mdb_env_close`
* `set_flags` - `mdb_env_set_flags`
* `get_flags` - `mdb_env_get_flags`
* `get_path` - `mdb_env_get_path`
* `set_mapsize` - `mdb_env_set_mapsize`
* `set_maxreaders` - `mdb_env_set_maxreaders`
* `get_maxreaders` - `mdb_env_get_maxreaders`
* `set_maxdbs` - `mdb_env_set_maxdbs`
* `txn_begin` - `mdb_env_txn_begin`
* `dbi_close` - `mdb_env_dbi_close`

## txn
* `commit` - `mdb_txn_commit`
* `abort` - `mdb_txn_abort`
* `reset` - `mdb_txn_reset`
* `renew` - `mdb_txn_renew`
* `dbi_open` - `mdb_txn_dbi_open`
* `stat` - `mdb_txn_stat`
* `dbi_drop` - `mdb_txn_dbi_drop`
* `get` - `mdb_txn_get`
* `put` - `mdb_txn_put`
* `del` - `mdb_txn_del`
* `cmp` - `mdb_txn_cmp`
* `dcmp` - `mdb_txn_dcmp`
* `cursor_open` - `mdb_txn_cursor_open`
* `cursor_renew` - `mdb_txn_cursor_renew`

## cursor
* `close` - `mdb_cursor_close`
* `txn` - `mdb_cursor_txn`
* `dbi` - `mdb_cursor_dbi`
* `get` - `mdb_cursor_get`
* `get_key` - `mdb_cursor_get` but the data is not returned (this isn't a part of the original API).
* `put` - `mdb_cursor_put`
* `del` - `mdb_cursor_del`
* `count` - `mdb_cursor_count`


## lpack
As a utility, [LHF's lpack](http://www.tecgraf.puc-rio.br/~lhf/ftp/lua/index.html#lpack) is included in the library for Lua versions lower than 5.3.

### Usage
The following is copied verbatim from lpack's original documentation.

The library adds two functions to the string library: pack and unpack.

pack is called as follows: pack(F,x1,x2,...), where F is a string describing
how the values x1, x2, ... are to be interpreted and formatted. Each letter
in the format string F consumes one of the given values. Only values of type
number or string are accepted. pack returns a (binary) string containing the
values packed as described in F. The letter codes understood by pack are listed
in lpack.c (they are inspired by Perl's codes but are not the same). Numbers
following letter codes in F indicate repetitions.

unpack is called as follows: unpack(s,F,[init]), where s is a (binary) string
containing data packed as if by pack, F is a format string describing what is
to be read from s, and the optional init marks where in s to begin reading the
values. unpack returns one value per letter in F until F or s is exhausted
(the letters codes are the same as for pack, except that numbers following `A'
are interpreted as the number of characters to read into the string, not as
repetitions).

The first value returned by unpack is the next unread position in s, which can
be used as the init position in a subsequent call to unpack. This allows you to
unpack values in a loop or in several steps. If the position returned by unpack
is beyond the end of s, then s has been exhausted; any calls to unpack starting
beyond the end of s will always return nil values.



# License

* Lightningmdb is distributed under the MIT license.
* [OpenLDAP Lightning Memory-Mapped Database (LMDB)](http://symas.com/mdb/) is distributed under the OpenLDAP Public license.
* lpack's code was placed under the public domain by its author.

## MIT License
Copyright (c) 2012,2015 Trusteer Ltd.

Author: Shmulik Regev

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, sub}}`

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

## The OpenLDAP Public License
  Version 2.8, 17 August 2003

Redistribution and use of this software and associated documentation
("Software"), with or without modification, are permitted provided
that the following conditions are met:

1. Redistributions in source form must retain copyright statements
   and notices,

2. Redistributions in binary form must reproduce applicable copyright
   statements and notices, this list of conditions, and the following
   disclaimer in the documentation and/or other materials provided
   with the distribution, and

3. Redistributions must contain a verbatim copy of this document.

The OpenLDAP Foundation may revise this license from time to time.
Each revision is distinguished by a version number.  You may use
this Software under terms of this license revision or under the
terms of any subsequent revision of the license.

THIS SOFTWARE IS PROVIDED BY THE OPENLDAP FOUNDATION AND ITS
CONTRIBUTORS ``AS IS'' AND ANY EXPRESSED OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT
SHALL THE OPENLDAP FOUNDATION, ITS CONTRIBUTORS, OR THE AUTHOR(S)
OR OWNER(S) OF THE SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

The names of the authors and copyright holders must not be used in
advertising or otherwise to promote the sale, use or other dealing
in this Software without specific, written prior permission.  Title
to copyright in this Software shall at all times remain with copyright
holders.

OpenLDAP is a registered trademark of the OpenLDAP Foundation.

Copyright 1999-2003 The OpenLDAP Foundation, Redwood City,
California, USA.  All Rights Reserved.  Permission to copy and
distribute verbatim copies of this document is granted.
