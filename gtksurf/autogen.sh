#!/bin/sh

automake -a --foreign
aclocal
autoconf

echo "running ./configure $*"
./configure $*
