dnl Some of this code is:
dnl Copyright (C) 1988 Eleftherios Gkioulekas <lf@amath.washington.edu>
dnl it is taken from the autotools distribution...

# Configure paths for GLIB
# Owen Taylor     97-11-3

dnl AM_PATH_GLIB([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND [, MODULES]]]])
dnl Test for GLIB, and define GLIB_CFLAGS and GLIB_LIBS, if "gmodule" or 
dnl gthread is specified in MODULES, pass to glib-config
dnl
AC_DEFUN(AM_PATH_GLIB,
[dnl 
dnl Get the cflags and libraries from the glib-config script
dnl
AC_ARG_WITH(glib-prefix,[  --with-glib-prefix=PFX   Prefix where GLIB is installed (optional)],
            glib_config_prefix="$withval", glib_config_prefix="")
AC_ARG_WITH(glib-exec-prefix,[  --with-glib-exec-prefix=PFX Exec prefix where GLIB is installed (optional)],
            glib_config_exec_prefix="$withval", glib_config_exec_prefix="")
AC_ARG_ENABLE(glibtest, [  --disable-glibtest       Do not try to compile and run a test GLIB program],
		    , enable_glibtest=yes)

  if test x$glib_config_exec_prefix != x ; then
     glib_config_args="$glib_config_args --exec-prefix=$glib_config_exec_prefix"
     if test x${GLIB_CONFIG+set} != xset ; then
        GLIB_CONFIG=$glib_config_exec_prefix/bin/glib-config
     fi
  fi
  if test x$glib_config_prefix != x ; then
     glib_config_args="$glib_config_args --prefix=$glib_config_prefix"
     if test x${GLIB_CONFIG+set} != xset ; then
        GLIB_CONFIG=$glib_config_prefix/bin/glib-config
     fi
  fi

  for module in . $4
  do
      case "$module" in
         gmodule) 
             glib_config_args="$glib_config_args gmodule"
         ;;
         gthread) 
             glib_config_args="$glib_config_args gthread"
         ;;
      esac
  done

  AC_PATH_PROG(GLIB_CONFIG, glib-config, no)
  min_glib_version=ifelse([$1], ,0.99.7,$1)
  AC_MSG_CHECKING(for GLIB - version >= $min_glib_version)
  no_glib=""
  if test "$GLIB_CONFIG" = "no" ; then
    no_glib=yes
  else
    GLIB_CFLAGS=`$GLIB_CONFIG $glib_config_args --cflags`
    GLIB_LIBS=`$GLIB_CONFIG $glib_config_args --libs`
    glib_config_major_version=`$GLIB_CONFIG $glib_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    glib_config_minor_version=`$GLIB_CONFIG $glib_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    glib_config_micro_version=`$GLIB_CONFIG $glib_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_glibtest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $GLIB_CFLAGS"
      LIBS="$GLIB_LIBS $LIBS"
dnl
dnl Now check if the installed GLIB is sufficiently new. (Also sanity
dnl checks the results of glib-config to some extent
dnl
      rm -f conf.glibtest
      AC_TRY_RUN([
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

int 
main ()
{
  int major, minor, micro;
  char *tmp_version;

  system ("touch conf.glibtest");

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = g_strdup("$min_glib_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_glib_version");
     exit(1);
   }

  if ((glib_major_version != $glib_config_major_version) ||
      (glib_minor_version != $glib_config_minor_version) ||
      (glib_micro_version != $glib_config_micro_version))
    {
      printf("\n*** 'glib-config --version' returned %d.%d.%d, but GLIB (%d.%d.%d)\n", 
             $glib_config_major_version, $glib_config_minor_version, $glib_config_micro_version,
             glib_major_version, glib_minor_version, glib_micro_version);
      printf ("*** was found! If glib-config was correct, then it is best\n");
      printf ("*** to remove the old version of GLIB. You may also be able to fix the error\n");
      printf("*** by modifying your LD_LIBRARY_PATH enviroment variable, or by editing\n");
      printf("*** /etc/ld.so.conf. Make sure you have run ldconfig if that is\n");
      printf("*** required on your system.\n");
      printf("*** If glib-config was wrong, set the environment variable GLIB_CONFIG\n");
      printf("*** to point to the correct copy of glib-config, and remove the file config.cache\n");
      printf("*** before re-running configure\n");
    } 
  else if ((glib_major_version != GLIB_MAJOR_VERSION) ||
	   (glib_minor_version != GLIB_MINOR_VERSION) ||
           (glib_micro_version != GLIB_MICRO_VERSION))
    {
      printf("*** GLIB header files (version %d.%d.%d) do not match\n",
	     GLIB_MAJOR_VERSION, GLIB_MINOR_VERSION, GLIB_MICRO_VERSION);
      printf("*** library (version %d.%d.%d)\n",
	     glib_major_version, glib_minor_version, glib_micro_version);
    }
  else
    {
      if ((glib_major_version > major) ||
        ((glib_major_version == major) && (glib_minor_version > minor)) ||
        ((glib_major_version == major) && (glib_minor_version == minor) && (glib_micro_version >= micro)))
      {
        return 0;
       }
     else
      {
        printf("\n*** An old version of GLIB (%d.%d.%d) was found.\n",
               glib_major_version, glib_minor_version, glib_micro_version);
        printf("*** You need a version of GLIB newer than %d.%d.%d. The latest version of\n",
	       major, minor, micro);
        printf("*** GLIB is always available from ftp://ftp.gtk.org.\n");
        printf("***\n");
        printf("*** If you have already installed a sufficiently new version, this error\n");
        printf("*** probably means that the wrong copy of the glib-config shell script is\n");
        printf("*** being found. The easiest way to fix this is to remove the old version\n");
        printf("*** of GLIB, but you can also set the GLIB_CONFIG environment to point to the\n");
        printf("*** correct copy of glib-config. (In this case, you will have to\n");
        printf("*** modify your LD_LIBRARY_PATH enviroment variable, or edit /etc/ld.so.conf\n");
        printf("*** so that the correct libraries are found at run-time))\n");
      }
    }
  return 1;
}
],, no_glib=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_glib" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$GLIB_CONFIG" = "no" ; then
       echo "*** The glib-config script installed by GLIB could not be found"
       echo "*** If GLIB was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the GLIB_CONFIG environment variable to the"
       echo "*** full path to glib-config."
     else
       if test -f conf.glibtest ; then
        :
       else
          echo "*** Could not run GLIB test program, checking why..."
          CFLAGS="$CFLAGS $GLIB_CFLAGS"
          LIBS="$LIBS $GLIB_LIBS"
          AC_TRY_LINK([
#include <glib.h>
#include <stdio.h>
],      [ return ((glib_major_version) || (glib_minor_version) || (glib_micro_version)); ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding GLIB or finding the wrong"
          echo "*** version of GLIB. If it is not finding GLIB, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"
          echo "***"
          echo "*** If you have a RedHat 5.0 system, you should remove the GTK package that"
          echo "*** came with the system with the command"
          echo "***"
          echo "***    rpm --erase --nodeps gtk gtk-devel" ],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means GLIB was incorrectly installed"
          echo "*** or that you have moved GLIB since it was installed. In the latter case, you"
          echo "*** may want to edit the glib-config script: $GLIB_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     GLIB_CFLAGS=""
     GLIB_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(GLIB_CFLAGS)
  AC_SUBST(GLIB_LIBS)
  rm -f conf.glibtest
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

# --------------------------------------------------------------------------
# Check whether the C compiler accepts a certain flag
# If it does it adds the flag to CFLAGS
# If it does not then it returns an error to lf_ok
# Usage:
#  LF_CHECK_CC_FLAG(-flag1 -flag2 -flag3 ...)
# -------------------------------------------------------------------------

AC_DEFUN(LF_CHECK_CC_FLAG,[
  echo 'void f(){}' > conftest.c
  for i in $1
  do
    AC_MSG_CHECKING([whether $CC accepts $i])
    if test -z "`${CC} $i -c conftest.c 2>&1`"
    then
      CFLAGS="${CFLAGS} $i"
      AC_MSG_RESULT(yes)
    else
      AC_MSG_RESULT(no)
    fi
  done
  rm -f conftest.c conftest.o
])


AC_DEFUN([RS_CHECK_LIB], 
[
  AC_CHECK_LIB($1, main,,
    AC_MSG_ERROR([Sorry: Can't link with library lib$1.\(a\|so\).]))
])

AC_DEFUN([RS_CHECK_HEADER],
[
  AC_CHECK_HEADER($1,,
    AC_MSG_ERROR([Sorry: Can't find header file $1.]))
])

dnl Use AC_VERBOSE w/o the warnings
dnl
AC_DEFUN([CF_VERBOSE],
[test -n "$verbose" && echo "   $1" 1>&AC_FD_MSG
])dnl


dnl Autoconf support for C++
dnl Copyright (C) 1988 Eleftherios Gkioulekas <lf@amath.washington.edu>
dnl  
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl 
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl 
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software 
dnl Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
dnl 
dnl As a special exception to the GNU General Public License, if you 
dnl distribute this file as part of a program that contains a configuration 
dnl script generated by Autoconf, you may include it under the same 
dnl distribution terms that you use for the rest of that program.

# -----------------------------------------------------------------------
# This macro determines hardware-vendor-os information and 
# sets the variable ``canonical_host_type'' to that information
# ------------------------------------------------------------------------

dnl ACCONFIG TEMPLATE
dnl #undef YOUR_OS
dnl END ACCONFIG

AC_DEFUN(LF_HOST_TYPE, [
  AC_CANONICAL_HOST
  if test -z "$host"
  then
    host=unknown
  fi
  canonical_host_type=$host
  if test "$host" = unknown
  then
    AC_MSG_WARN(configuring for unknown system type)
  fi
  AC_SUBST(canonical_host_type)
  AC_DEFINE_UNQUOTED(YOUR_OS,"$canonical_host_type")
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
if test "$ice_cv_have_explicit_template_instantiation" = yes; then
AC_DEFINE(HAVE_EXPLICIT_TEMPLATE_INSTANTIATION)
fi
])dnl

dnl @synopsis AC_CXX_FUNCTION_NONTYPE_PARAMETERS
dnl
dnl If the compiler supports function templates with non-type parameters,
dnl define HAVE_FUNCTION_NONTYPE_PARAMETERS.
dnl
dnl @author Luc Maisonobe
dnl
AC_DEFUN(AC_CXX_FUNCTION_NONTYPE_PARAMETERS, [
  AC_CACHE_CHECK([whether ${CXX} supports function templates with non-type parameters],
    ac_cv_cxx_function_nontype_parameters, [
      AC_LANG_SAVE
      AC_LANG_CPLUSPLUS
      AC_TRY_COMPILE([
          template<class T, int N> class A {};
          template<class T, int N> int f(const A<T,N>& x) { return 0; }
        ],[
          A<double, 17> z; return f(z);
        ],
        ac_cv_cxx_function_nontype_parameters=yes,
        ac_cv_cxx_function_nontype_parameters=no)
      AC_LANG_RESTORE
    ])

  if test "$ac_cv_cxx_function_nontype_parameters" = no; then
    AC_MSG_ERROR([Sorry your C++ compiler isn't smart enough to build surf.])
  fi
])


dnl check if C++ compiler supports namespaces:
dnl (inspired by Gtk-- 1.2.1 configure.in)
dnl ------------------------------------------

AC_DEFUN(JOJO_CXX_NAMESPACES, [
  AC_MSG_CHECKING(whether ${CXX} supports namespaces)
  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  AC_TRY_COMPILE([
      namespace Foo { struct A {}; }
      using namespace Foo;
    ],[
      A a;
    ],
    AC_MSG_RESULT(yes),
    AC_MSG_RESULT(no)
    AC_MSG_ERROR([Sorry: C++ compiler doesn't support namespaces.]))
  AC_LANG_RESTORE
])

AC_DEFUN(JOJO_CXX_STDNAMESPACE, [
  AC_MSG_CHECKING(whether ${CXX} uses std namespace)
  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  AC_TRY_COMPILE([
    #include<iostream>
    namespace std{}
    using namespace std;
  ],[
    cout << "test" << endl;
  ],
  AC_MSG_RESULT(yes),
  AC_MSG_RESULT(no)
  AC_MSG_ERROR([Sorry: C++ compiler doesn't use namepsace std.]))
  AC_LANG_RESTORE
])

dnl JOJO_CHECK_STRINGSTREAM
dnl check if we have <sstream> header an stringstream (instead of strstream)

AC_DEFUN(JOJO_CXX_STRINGSTREAM, [
  AC_MSG_CHECKING(whether ${CXX} has <sstream>/stringstream)
  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  AC_TRY_COMPILE([
      #include<sstream>
    ], [
      std::string s = "test";
      std::istringstream iss(s);
    ],
    AC_DEFINE(HAVE_STRINGSTREAM)
     AC_MSG_RESULT(yes),
    AC_MSG_RESULT(no))
  AC_LANG_RESTORE
])

dnl
dnl checks for STLport:
dnl
dnl @author Johannes Beigel <jojo@beigel.de>
dnl
AC_DEFUN(JOJO_STLPORT, [
  AC_ARG_WITH(stlport-prefix,
    [  --with-stlport-prefix=PFX Prefix where STLport is installed (optional)],
    stlport_prefix="$withval",
    stlport_prefix="")
  AC_ARG_ENABLE(stlport,
    [  --enable-stlport        Try to find STLport includes and library],
    , enable_stlport=no)

  if test "x$enable_stlport" = "xyes" ; then
    AC_MSG_CHECKING(for STLport includes)
    stlportinclude=""

    if test "x$stlport_prefix" != x ; then
      if test -d "$stlport_prefix/include/stlport"; then
	stlportinclude="-I$stlport_prefix/include/stlport"
        LDFLAGS="-L$stlport_prefix/lib $LDFLAGS"
      fi
    else
      if test -d "/usr/local/include/stlport"; then
        stlportinclude="-I/usr/local/include/stlport"
      elif test -d "/usr/include/stlport"; then
        stlportinclude="-I/usr/include/stlport"
      fi
    fi

    if test "x$stlportinclude" != x ; then
      CPPFLAGS="$stlportinclude $CPPFLAGS"
      AC_MSG_RESULT($stlportinclude)
      AC_CHECK_LIB(stlport, main,,
        AC_CHECK_LIB(stlport_gcc, main))
    else
      AC_MSG_RESULT(no)
    fi
  fi
])

dnl
dnl checks for GNU MP (gmp)
dnl
dnl @author Johannes Beigel
dnl
AC_DEFUN(JOJO_GMP, [
  AC_ARG_WITH(gmp-prefix,
    [  --with-gmp-prefix=PFX   Prefix where GMP is installed],
    gmp_prefix="$withval",
    gmp_prefix="")

  if test x"$gmp_prefix" != x ; then
    LDFLAGS="-L$gmp_prefix/lib $LDFLAGS"
    CPPFLAGS="-I$gmp_prefix/include $CPPFLAGS"
  fi

  AC_CHECK_HEADER(gmp.h,,
    AC_CHECK_HEADER(gmp2/gmp.h,
      AC_DEFINE(GMP2_PREFIX),
      AC_MSG_ERROR([Sorry: Can't find header file gmp.h])))

  AC_CHECK_LIB(gmp, main,,
    AC_CHECK_LIB(gmp2, main,, AC_MSG_ERROR([Sorry: can't find gmp])))
])

dnl
dnl checks for GTS (the GNU Triangulated Surfaces library)
dnl
dnl   sets have_gts to "yes" if GTS has been found
dnl
dnl @author Johannes Beigel
dnl
AC_DEFUN(JOJO_GTS, [
  AC_ARG_WITH(gts-prefix,
    [  --with-gts-prefix=PFX   Prefix where GTS is installed],
    gts_prefix="$withval",
    gts_prefix="")
  AC_ARG_ENABLE(gts,
    [  --disable-gts           Do not try to find GTS includes and library],
    , enable_gts=yes)

  if test "x$enable_gts" = "xyes" ; then
    if test x"$gts_prefix" != x ; then
      LDFLAGS="-L$gts_prefix/lib $LDFLAGS"
      CPPFLAGS="-I$gts_prefix/include $CPPFLAGS"
    fi

    dnl GTS needs GLIB...:
    echo "**** If the following test for GLIB fails you may want to install GTS"
    echo "**** (and GLIB) or re-run configure with the --disable-gts option."
    echo "**** However, without GTS you won't be able to use the triangulation"
    echo "**** feature of surf!"

    AM_PATH_GLIB(1.2.0)
    LIBS="$GLIB_LIBS $LIBS"
    CPPFLAGS="$CPPFLAGS $GLIB_CFLAGS"

    AC_CHECK_HEADER(gts.h,
      AC_CHECK_LIB(gts, main,
        AC_DEFINE(HAVE_LIBGTS)
         LIBS="-lgts $LIBS"
         have_gts="yes",
        AC_MSG_ERROR([Couldn't find libgts. Please install GTS or try --disable-gts.])),
      AC_MSG_ERROR([Couldn't find gts.h. Please install GTS or try --diasble-gts.]))
  fi
])

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