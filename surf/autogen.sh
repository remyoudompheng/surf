#!/bin/sh

aclocal
autoheader
automake -a --foreign
autoconf
