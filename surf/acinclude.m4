# JB_USR_LOCAL
# ------------
AC_DEFUN([JB_USR_LOCAL], [
  AC_MSG_CHECKING([whether directoy /usr/local/lib/ exists])
  if test -d "/usr/local/lib/"; then
    LDFLAGS="-L/usr/local/lib/ $LDFLAGS"
    AC_MSG_RESULT([yes])
  else
    AC_MSG_RESULT([no])
  fi
  AC_MSG_CHECKING([whether directory /usr/local/include/ exists])
  if test -d "/usr/local/include/"; then
    CPPFLAGS="-I/usr/local/include/ $CPPFLAGS"
    AC_MSG_RESULT([yes])
  else
    AC_MSG_RESULT([no])
  fi
])

# JB_PATH(PACKAGE, HEADER, LIBRARY, SYMBOL)
# ------------------------------------------
AC_DEFUN([JB_PATH], [
 AC_ARG_ENABLE($1, AC_HELP_STRING([--disable-$1], [don't try to find $1]),, enable_pkg=yes)

 AC_ARG_WITH($1-prefix, AC_HELP_STRING([--with-$1-prefix=DIR], [search $1 header files in DIR/include, libraries in DIR/lib]),
             pkg_prefix=$withval, pkg_prefix="")
 if test "x$pkg_prefix" != x ; then
  LDFLAGS="-L$pkg_prefix/lib $LDFLAGS"
  CPPFLAGS="-I$pkg_prefix/include $CPPFLAGS"
 fi

 AC_ARG_WITH($1-include, AC_HELP_STRING([--with-$1-include=DIR], [search $1 header files in DIR]),
	     pkg_include=$withval, pkg_include="")
 if test "x$pkg_include" != x ; then
  CPPFLAGS="-I$pkg_include $CPPFLAGS"
 fi

 AC_ARG_WITH($1-lib, AC_HELP_STRING([--with-$1-lib=DIR], [search $1 libraries in DIR]),
	     pkg_lib=$withval, pkg_lib="")
 if test "x$pkg_lib" != x ; then
  LDFLAGS="-L$pkg_lib $LDFLAGS"
 fi

 AH_TEMPLATE(AS_TR_CPP(HAVE_[$1]), [Define to 1 if package $1 is available.])

 if test "x$enable_pkg" = "xyes" ; then
  AC_CHECK_HEADER($2, [
   AC_CHECK_LIB($3, $4, AC_DEFINE(AS_TR_CPP(HAVE_[$1])) LIBS="-l$3 $LIBS")
  ])
 fi
])

# --------------------------------------------------------------------------
# Check whether the C++ compiler accepts a certain flag
# If it does it adds the flag to CXXFLAGS
# If it does not then it returns an error to lf_ok
# Usage:
#   LF_CHECK_CXX_FLAG(-flag1 -flag2 -flag3 ...)
# -------------------------------------------------------------------------

AC_DEFUN(LF_CHECK_CXX_FLAG,[
  echo 'void f(){}' > conftest.cc
  for i in $1
  do
    AC_MSG_CHECKING([whether $CXX accepts $i])
    if test -z "`${CXX} $i -c conftest.cc 2>&1`"
    then
      CXXFLAGS="${CXXFLAGS} $i"
      AC_MSG_RESULT(yes)
    else
      AC_MSG_RESULT(no)
    fi
  done
  rm -f conftest.cc conftest.o
])

# JB_CHECK_FUNC
# -------------
AC_DEFUN([JB_CHECK_FUNC],
[
  AC_CHECK_FUNC($1,,
    AC_MSG_ERROR([Sorry: Can't find $1() function.]))
])

# RS_CHECK_LIB:
# -------------
AC_DEFUN([RS_CHECK_LIB], 
[
  AC_CHECK_LIB($1, main,,
    AC_MSG_ERROR([Sorry: Can't link with library lib$1.\(a\|so\).]))
])

# RS_CHECK_HEADER:
# ----------------
AC_DEFUN([RS_CHECK_HEADER],
[
  AC_CHECK_HEADER($1,,
    AC_MSG_ERROR([Sorry: Can't find header file $1.]))
])

dnl this is from:
dnl To: autoconf-collection@peti.gmd.de 
dnl Subject: ice_cxx_explicit_template_instantiation.m4 
dnl From: Andreas Zeller <zeller@ips.cs.tu-bs.de> 
dnl Date: Sat, 29 Nov 1997 17:13:23 +0100 (MET) 
dnl Sender: autoconf-collection-owner@peti.gmd.de 

dnl ICE_CXX_EXPLICIT_TEMPLATE_INSTANTIATION
dnl ---------------------------------------
dnl
dnl If the C++ compiler supports explicit template instantiation,
dnl define `HAVE_EXPLICIT_TEMPLATE_INSTANTIATION'.
dnl
AC_DEFUN(ICE_CXX_EXPLICIT_TEMPLATE_INSTANTIATION,
[
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING(whether ${CXX} supports explicit template instantiation)
AC_CACHE_VAL(ice_cv_have_explicit_template_instantiation,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE([
template <class T> class Pointer { public: T *value; };
template class Pointer<char>;
], [/* empty */],
ice_cv_have_explicit_template_instantiation=yes,
ice_cv_have_explicit_template_instantiation=no)
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_have_explicit_template_instantiation)
AH_TEMPLATE(HAVE_EXPLICIT_TEMPLATE_INSTANTIATION, [define to 1 if we have explicit...])
if test "$ice_cv_have_explicit_template_instantiation" = yes; then
AC_DEFINE(HAVE_EXPLICIT_TEMPLATE_INSTANTIATION)
fi
])dnl

dnl
dnl checks for OpenGL/Mesa and GLU
dnl
dnl  If everything was found
dnl   - sets have_opengl to "yes"
dnl   - defines HAVE_OPENGL
dnl   - GL_LIBS, GL_LDFLAGS and GL_CPPFLAGs contain the needed flags
dnl
dnl @author Johannes Beigel
dnl
AC_DEFUN(JOJO_OPENGL, [
  save_cppflags=$CPPFLAGS
  save_libs=$LIBS
  save_ldflags=$LDFLAGS

  AC_ARG_WITH(gl-prefix,
    [  --with-gl-prefix=PFX    Prefix where OpenGL/Mesa & GLU are installed],
    gl_prefix="$withval",
    gl_prefix="")
  AC_ARG_ENABLE(opengl,
    [  --disable-opengl        Do not try to find any GL/GLU includes and libraries],
    , enable_opengl=yes)

  if test "x$enable_opengl" = "xyes" ; then

    if test "x$gl_prefix" != "x" ; then
      GL_CPPFLAGS="-I$gl_prefix/include"
      GL_LDFLAGS="-L$gl_prefix/lib"
      CPPFLAGS="$CPPFLAGS $GL_CPPFLAGS"
      LDFLAGS="$LDFLAGS $GL_LDFLAGS"
    fi

    have_opengl=yes

    dnl libGL/libMesaGL:
    AC_CHECK_LIB(GL, glBegin,
      GL_LIBS="-lGL",
      AC_CHECK_LIB(MesaGL, glBegin,
        GL_LIBS="-lMesaGL",
        have_opengl=no))

    LIBS="$LIBS $GL_LIBS"

    if test "x$have_opengl" = "xyes"; then
      dnl libGLU/libMesaGLU:
      AC_CHECK_LIB(GLU, gluLookAt,
        GL_LIBS="$GL_LIBS -lGLU",
        AC_CHECK_LIB(MesaGLU, gluLookAt,
          GL_LIBS="$GL_LIBS -lMesaGLU",
          have_opengl=no))
    fi

    if test "x$have_opengl" = "xyes"; then
      dnl GL & GLU header files:
      AC_CHECK_HEADERS(GL/gl.h GL/glu.h,
        AC_DEFINE(HAVE_OPENGL),
        have_opengl=no)
    fi
  fi

  CPPFLAGS=$save_cppflags
  LIBS=$save_libs
  LDFLAGS=$save_ldflags
])

AC_DEFUN([JOJO_INVENTOR],[
  save_cppflags=$CPPFLAGS
  save_libs=$LIBS
  save_ldflags=$LDFLAGS

  AC_ARG_WITH(inventor-prefix,
    [  --with-inventor-prefix=PFX Prefix where OpenInventor is installed],
    inventor_prefix="$withval",
    inventor_prefix="")
  AC_ARG_ENABLE(inventor,
    [  --disable-inventor      Do not try to find Inventor includes and libraries],
    , enable_inventor=yes)

  if test "x$enable_inventor" = "xyes" ; then

    dnl OpenGL & GLU:
    JOJO_OPENGL

    if test "x$have_opengl" = "xyes"; then

      if test "x$inventor_prefix" != "x" ; then
        INVENTOR_CPPFLAGS="-I$inventor_prefix/include"
        INVENTOR_LDFLAGS="-L$inventor_prefix/lib"
      fi

      dnl X
      AC_PATH_X

      if test x"$no_x" != "xyes"; then
        CPPFLAGS="$CPPFLAGS $GL_CPPFLAGS -I$x_includes"
        LDFLAGS="$LDFLAGS $GL_LDFLAGS -L$x_libraries"
        LIBS="$LIBS $GL_LIBS -lX11 -lInventor"

        AC_LANG_SAVE
        AC_LANG_CPLUSPLUS

        AC_MSG_CHECKING([for SoDB::init in -lInventor])

        AC_TRY_COMPILE([
          #include <Inventor/SoDB.h>
        ], [
          SoDB::init();
        ],
        have_inventor=yes,
        have_inventor=no)

        AC_MSG_RESULT($have_inventor)

        AC_LANG_RESTORE

      fi

      if test x"$have_inventor" = "xno"; then
        CPPFLAGS=$save_cppflags
        LDFLAGS=$save_ldflags
        LIBS=$save_libs
      else
        AC_DEFINE(HAVE_INVENTOR)
      fi
    fi
  else
    have_inventor=no
  fi
])

# VL_LIB_READLINE
#
# Description: Searches for a readline compatible library. If found,
#  defines `HAVE_LIBREADLINE'. If the found library has the `add_history'
#  function, sets also `HAVE_READLINE_HISTORY'. Also checks for the
#  locations of the necessary include files and sets `HAVE_READLINE_H' or
#  `HAVE_READLINE_READLINE_H' and `HAVE_READLINE_HISTORY_H' or
#  'HAVE_HISTORY_H' if the corresponding include files exists.
#
#  The libraries that may be readline compatible are `libedit',
#  `libeditline' and `libreadline'. Sometimes we need to link a termcap
#  library for readline to work, this macro tests these cases too by trying
#  to link with `libtermcap', `libcurses' or `libncurses' before giving up.
#
# Author: Ville Laurikari <vl@iki.fi>
# License: GPL

AC_DEFUN([VL_LIB_READLINE], [
  AC_CACHE_CHECK([for a readline compatible library],
                 vl_cv_lib_readline, [
    ORIG_LIBS="$LIBS"
    for readline_lib in readline edit editline; do
      for termcap_lib in "" termcap curses ncurses; do
        if test -z "$termcap_lib"; then
          TRY_LIB="-l$readline_lib"
        else
          TRY_LIB="-l$readline_lib -l$termcap_lib"
        fi
        LIBS="$ORIG_LIBS $TRY_LIB"
        AC_TRY_LINK_FUNC(readline, vl_cv_lib_readline="$TRY_LIB")
        if test -n "$vl_cv_lib_readline"; then
          break
        fi
      done
      if test -n "$vl_cv_lib_readline"; then
        break
      fi
    done
    if test -z "$vl_cv_lib_readline"; then
      vl_cv_lib_readline="no"
      LIBS="$ORIG_LIBS"
    fi
  ])

  if test "$vl_cv_lib_readline" != "no"; then
    AC_DEFINE(HAVE_LIBREADLINE, 1,
              [Define if you have a readline compatible library])
    AC_CHECK_HEADERS(readline.h readline/readline.h)
    AC_CACHE_CHECK([whether readline supports history],
                   vl_cv_lib_readline_history, [
      vl_cv_lib_readline_history="no"
      AC_TRY_LINK_FUNC(add_history, vl_cv_lib_readline_history="yes")
    ])
    if test "$vl_cv_lib_readline_history" = "yes"; then
      AC_DEFINE(HAVE_READLINE_HISTORY, 1,
                [Define if your readline library has \`add_history'])
      AC_CHECK_HEADERS(history.h readline/history.h)
    fi
  fi
])dnl
