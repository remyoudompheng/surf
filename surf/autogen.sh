#!/bin/sh

if ! test -x autogen.sh; then 
    echo "ERROR: autogen.sh must be run in directory surf"
    exit 10
fi

echo "creating yaccsrc/addDefaultSymbols.cc from defaults.pic"
perl tools/convertScript.pl defaults.pic >yaccsrc/addDefaultSymbols.cc

if test -x configure; then
    echo "configure exists... exiting"
    exit 0
fi

automake -a --foreign
aclocal
autoconf

echo "running ./configure $*"
./configure $*
