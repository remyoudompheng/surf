# configure.ac for surf (http://surf.sourceforge.net)
#
# Author: Johannes Beigel <jojo@users.sourceforge.net>
# License: GPL

AC_INIT(surf, 1.1.0, jojo@users.sourceforge.net)
AC_PREREQ(2.53)
AC_CONFIG_SRCDIR([src/main.cc])
AM_INIT_AUTOMAKE(surf, 1.1.0)
AM_CONFIG_HEADER(config.h)

# put /usr/local/lib and /usr/local/include in search paths:
JB_USR_LOCAL

# some standard checks:
AC_PROG_CC
AC_PROG_CXX
AM_PROG_LEX
LIBS="$LIBS $LEXLIB"
AC_PROG_YACC
AC_PROG_RANLIB

# check if C++ compiler really works:
ICE_CXX_EXPLICIT_TEMPLATE_INSTANTIATION
LF_CHECK_CXX_FLAG(-Wall)

# use -LANG:std with IRIX compiler:
if test "$GCC" = "no" ; then
  case `uname` in
  IRIX*)
    CXXFLAGS="-LANG:std $CXXFLAGS"
    ;;
  esac
fi

# checks for header files:
AC_CHECK_HEADERS([unistd.h netinet/in.h sys/param.h])

# getopt stuff:
JB_CHECK_FUNC(getopt)
AC_CHECK_HEADERS(getopt.h)
AC_CHECK_LIB(gnugetopt, main)
AC_CHECK_FUNCS(getopt_long)

# math libs:
RS_CHECK_LIB(m, main)
JB_PATH(gmp, gmp.h, gmp, main)

# image libraries:
JB_PATH(jpeg, jpeglib.h, jpeg, jpeg_std_error)
JB_PATH(png, png.h, png, png_set_rows)
JB_PATH(tiff, tiffio.h, tiff, TIFFSetField)

# readline:
VL_LIB_READLINE

# check for GTS:
AM_PATH_GLIB(1.2.0)
#PKG_CHECK_MODULES(PACKAGE, ["glib-2.0"])
LIBS="$PACKAGE_LIBS $LIBS"
CPPFLAGS="$CPPFLAGS $PACKAGE_CFLAGS"
JB_PATH(gts, gts.h, gts, gts_surface_new)

AM_MISSING_PROG(HELP2MAN, help2man, $missing_dir)
AM_MISSING_PROG(SED, sed, $missing_dir)

AC_CONFIG_FILES([Makefile \
          docs/Makefile \
          src/Makefile \
	  image-formats/Makefile \
	  drawfunc/Makefile \
	  yaccsrc/Makefile \
	  curve/Makefile \
	  draw/Makefile \
	  misc/Makefile \
	  debug/Makefile \
          dither/Makefile \
          triangulator/Makefile])
AC_OUTPUT

