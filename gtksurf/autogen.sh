#!/bin/sh

automake -a --foreign
aclocal
autoconf
autoheader

echo "running ./configure $*"
./configure $*
