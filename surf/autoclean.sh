#!/bin/sh
test -e Makefile && make distclean
rm -f install-sh
rm -f mkinstalldirs
rm -f missing
rm -f config.guess
rm -f config.sub
rm -f configure
rm -f Makefile.in */Makefile.in
rm -f aclocal.m4
rm -rf config.log config.cache autom4te.cache
rm -f stamp-h.in
rm -f config.h.in
find . -name \*~ -exec rm \{\} \;
(cd docs && rm -f mdate-sh stamp-vti texinfo.tex version.texi surf.info surf.1)
