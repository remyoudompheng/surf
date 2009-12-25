dnl ------------------------
dnl configure.in for gtksurf
dnl ------------------------

AC_PREREQ(2.13)
AC_INIT(src/main.cc)
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE(gtk2surf, 0.1)

dnl some standard checks:
dnl ---------------------

AC_PROG_CC
AC_PROG_CXX
AC_AIX
AC_ISC_POSIX
AC_MINIX
AC_HEADER_STDC

dnl check if C++ compiler really works:
dnl -----------------------------------

AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_LANG_RESTORE

if test "$GCC" = "no" ; then
	case `uname` in
	IRIX*)
		CXXFLAGS="-LANG:std $CXXFLAGS"
		;;
	esac
fi

dnl include /usr/local/lib and /usr/local/include in search paths:
dnl --------------------------------------------------------------

if test -d "/usr/local/lib/"; then
	LDFLAGS="-L/usr/local/lib/ $LDFLAGS"
fi
if test -d "/usr/local/include/"; then
	CPPFLAGS="-I/usr/local/include/ $CPPFLAGS"
fi

dnl standard headers:
dnl =================

AC_CHECK_HEADERS(unistd.h)
AC_LANG_SAVE
AC_LANG(C++)
AC_CHECK_HEADERS(sstream)
AC_LANG_RESTORE

dnl do we have getopt() and (optionally) GNU getopt_long()?
dnl =======================================================

AC_CHECK_FUNC(getopt,, AC_MSG_ERROR([Sorry: can't find getopt() function]))
AC_CHECK_HEADER(getopt.h)
AC_CHECK_LIB(gnugetopt, main)
AC_CHECK_FUNCS(getopt_long)

dnl check for pkg-config:
dnl =====================

m4_ifndef([PKG_PROG_PKG_CONFIG],
    [m4_fatal([Could not locate the pkg-config autoconf macros.
  These are usually located in /usr/share/aclocal/pkg.m4. If your macros
  are in a different location, try setting the environment variable
  ACLOCAL="aclocal -I/other/macro/dir" before running autoreconf.])])
PKG_PROG_PKG_CONFIG()

dnl check for gtkmm:
dnl ===================

if test -n "$PKG_CONFIG"; then
  PKG_CHECK_MODULES([GTK2], [gtk+-2.0 >= 2.12])
  CPPFLAGS="$CPPFLAGS $GTK2_CFLAGS"
  LIBS="$LIBS $GTK2_LIBS"
fi

if test -n "$PKG_CONFIG"; then
  PKG_CHECK_MODULES([GTKMM], [gtkmm-2.4 >= 2.12])
  CPPFLAGS="$CPPFLAGS $GTKMM_CFLAGS"
  LIBS="$LIBS $GTKMM_LIBS"
fi

AM_MISSING_PROG(HELP2MAN, help2man, $missing_dir)
AM_MISSING_PROG(SED, sed, $missing_dir)

AC_SUBST(CPPFLAGS)

AC_OUTPUT(Makefile
          src/Makefile
          pkgdata/Makefile)
