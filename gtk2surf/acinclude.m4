dnl Some of this code is:
dnl Copyright (C) 1988 Eleftherios Gkioulekas <lf@amath.washington.edu>
dnl it is taken from the autotools distribution...

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
	AC_CHECK_LIB($1, main,, AC_MSG_ERROR([Sorry: can't link with library lib$1.\(a\|so\).]))
])

AC_DEFUN([RS_CHECK_HEADER],
[
	AC_CHECK_HEADER($1,, AC_MSG_ERROR([Sorry: can't find header file $1]))
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
AC_DEFUN(AC_CXX_FUNCTION_NONTYPE_PARAMETERS,
[AC_CACHE_CHECK(whether ${CXX} supports function templates with non-type parameters,
ac_cv_cxx_function_nontype_parameters,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([
template<class T, int N> class A {};
template<class T, int N> int f(const A<T,N>& x) { return 0; }
],[A<double, 17> z; return f(z);],
 ac_cv_cxx_function_nontype_parameters=yes, ac_cv_cxx_function_nontype_parameters=no)
 AC_LANG_RESTORE
])
dnl if test "$ac_cv_cxx_function_nontype_parameters" = yes; then
dnl  AC_DEFINE(HAVE_FUNCTION_NONTYPE_PARAMETERS,,
dnl             [define if the compiler supports function templates with non-type parameters])
dnl fi
])


dnl check if C++ compiler supports namespaces:
dnl (inspired by Gtk-- 1.2.1 configure.in)
dnl ------------------------------------------

AC_DEFUN(JOJO_CXX_NAMESPACES, [
	AC_MSG_CHECKING(whether ${CXX} supports namespaces)
	AC_TRY_COMPILE([
		namespace Foo { struct A {}; }
		using namespace Foo;
	],[
		A a;
	],[
		AC_MSG_RESULT(yes)
	],[
		AC_MSG_RESULT(no)
		AC_MSG_ERROR([Sorry: C++ compiler doesn't support namespaces])
	])
])

AC_DEFUN(JOJO_CXX_STDNAMESPACE, [
	AC_MSG_CHECKING(whether ${CXX} uses std namespace)
	AC_TRY_COMPILE([
		#include <iostream>
		namespace std{}
		using namespace std;
	],[
		cout << "test" << endl;
	],[
		AC_MSG_RESULT(yes)
	],[
		AC_TRY_COMPILE([
			#include <iostream.h>
			namespace std{}
			using namespace std;
		],[
			cout << "test" << endl;
		],[
			AC_MSG_RESULT(yes)
		],[
			AC_MSG_RESULT(no)
			AC_MSG_ERROR([Sorry: C++ compiler doesn't use namepsace std])
		])
	])
])

dnl JOJO_CHECK_STRINGSTREAM
dnl check if we have <sstream> header an stringstream (instead of strstream)

AC_DEFUN(JOJO_CXX_STRINGSTREAM, [
  AC_MSG_CHECKING(whether ${CXX} has <sstream>/stringstream)
  AC_TRY_COMPILE([
      #include<sstream>
    ], [
      std::string s = "test";
      std::istringstream iss(s);
    ],
    AC_DEFINE(HAVE_STRINGSTREAM)
     AC_MSG_RESULT(yes),
    AC_MSG_RESULT(no))
])

dnl JOJO_COMPILE_WARNINGS
dnl Turn on many useful compiler warnings
dnl For now, only works on GCC
dnl (this is copied from GNOME_COMPILE_WARNINGS..)

AC_DEFUN([JOJO_COMPILE_WARNINGS],[
  if test "x$GCC" = xyes; then
    AC_MSG_CHECKING(what warning flags to pass to the C compiler)

    warnCFLAGS=
    case " $CFLAGS " in
      *[\ \	]-Wall[\ \	]*) ;;
      *) warnCFLAGS="-Wall" ;;
    esac

    AC_MSG_RESULT($warnCFLAGS)

    CFLAGS="$CFLAGS $warnCFLAGS"
  fi
])

dnl For C++, do basically the same thing.

AC_DEFUN([JOJO_CXX_WARNINGS],[
  if test "x$GCC" = xyes; then
    AC_MSG_CHECKING(what warning flags to pass to the C++ compiler)
  
    warnCXXFLAGS=
    
    case " $CXXFLAGS " in
      *[\ \	]-Wall[\ \	]*) ;;
      *) warnCXXFLAGS="-Wall" ;;
    esac

    AC_MSG_RESULT($warnCXXFLAGS)

    CXXFLAGS="$CXXFLAGS $warnCXXFLAGS"
  fi
])
