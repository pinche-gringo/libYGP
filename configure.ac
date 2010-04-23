dnl $Id: configure.in,v 1.102 2008/06/08 12:41:28 markus Rel $
dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_REVISION($Revision: 1.102 $)
AC_INIT([libYGP],[0.9.40],[g17m0@users.sourceforge.net],[libYGP])

RELEASE=${PACKAGE_VERSION%.*}
AC_DEFINE_UNQUOTED(LIB_RELEASE, "$RELEASE")
AC_DEFINE_UNQUOTED(SYSTEM, UNIX)
AC_DEFINE_UNQUOTED(LIBYGP_NAME, "$PACKAGE_NAME", [Name of the libYGP-package])
AC_SUBST(PACKAGE_VERSION)
AC_SUBST(RELEASE)

LIBRELEASE=0.9.20
dnl LIBRELEASE=$PACKAGE_VERSION
AC_SUBST(LIBRELEASE)

MYSUBDIRS="YGP"

dnl AC_CANONICAL_TARGET([])

AM_INIT_AUTOMAKE(dist-bzip2 no-define)
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])
AC_CONFIG_HEADERS([ygp-cfg.h])

AC_LANG(C)
AC_CHECK_LIB(dl, main)

AC_LANG(C++)

AC_C_BIGENDIAN

ALL_LINGUAS="de en es"

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AM_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_MAKE_SET

AM_GNU_GETTEXT

AC_CHECK_PROG([HAVE_PERL], [perl], [yes], [no])
if test "$HAVE_PERL" = "no"; then
    AC_MSG_WARN([Program 'perl' not found - bin/mgeni.pl will not work!])
fi

AC_CHECK_PROG([HAVE_EPIPHANY], [epiphany], [yes], [no])
AC_CHECK_PROG([HAVE_FIREFOX], [firefox], [yes], [no])
AC_CHECK_PROG([HAVE_GALEON], [galeon], [yes], [no])
AC_CHECK_PROG([HAVE_SEAMONKEY], [seamonkey], [yes], [no])
AC_CHECK_PROG([HAVE_MOZILLA], [mozilla], [yes], [no])
AC_CHECK_PROG([HAVE_NETSCAPE], [netscape], [yes], [no])
AC_CHECK_PROG([HAVE_KONQUEROR], [konqueror], [yes], [no])
AC_CHECK_PROG([HAVE_OPERA], [opera], [yes], [no])
AC_CHECK_PROG([HAVE_DILLO], [dillo], [yes], [no])
AC_CHECK_PROG([HAVE_IEXPLORER], [iexplore], [yes], [no])

dnl Convert yes/no values to values understood by the pre-processor
if test "$HAVE_EPIPHANY" = "yes"; then
   HAVE_EPIPHANY=1
else
   HAVE_EPIPHANY=0
fi
if test "$HAVE_FIREFOX" = "yes"; then
   HAVE_FIREFOX=1
else
   HAVE_FIREFOX=0
fi
if test "$HAVE_GALEON" = "yes"; then
   HAVE_GALEON=1
else
   HAVE_GALEON=0
fi
if test "$HAVE_SEAMONKEY" = "yes"; then
   HAVE_SEAMONKEY=1
else
   HAVE_SEAMONKEY=0
fi
if test "$HAVE_MOZILLA" = "yes"; then
   HAVE_MOZILLA=1
else
   HAVE_MOZILLA=0
fi
if test "$HAVE_NETSCAPE" = "yes"; then
   HAVE_NETSCAPE=1
else
   HAVE_NETSCAPE=0
fi
if test "$HAVE_KONQUEROR" = "yes"; then
   HAVE_KONQUEROR=1
else
   HAVE_KONQUEROR=0
fi
if test "$HAVE_OPERA" = "yes"; then
   HAVE_OPERA=1
else
   HAVE_OPERA=0
fi
if test "$HAVE_DILLO" = "yes"; then
   HAVE_DILLO=1
else
   HAVE_DILLO=0
fi
if test "$HAVE_IEXPLORER" = "yes"; then
   HAVE_IEXPLORER=1
else
   HAVE_IEXPLORER=0
fi

AC_DEFINE_UNQUOTED([HAVE_EPIPHANY], $HAVE_EPIPHANY, [Set to 1 if Epiphany exists, 0 if not])
AC_DEFINE_UNQUOTED([HAVE_FIREFOX], $HAVE_FIREFOX, [Set to 1 if Firefox exists, 0 if not])
AC_DEFINE_UNQUOTED([HAVE_GALEON], $HAVE_GALEON, [Set to 1 if Galeon exists, 0 if not])
AC_DEFINE_UNQUOTED([HAVE_SEAMONKEY], $HAVE_SEAMONKEY, [Set to 1 if Seamonkey exists, 0 if not])
AC_DEFINE_UNQUOTED([HAVE_MOZILLA], $HAVE_MOZILLA, [Set to 1 if Mozilla exists, 0 if not])
AC_DEFINE_UNQUOTED([HAVE_NETSCAPE], $HAVE_NETSCAPE, [Set to 1 if Netscape exists, 0 if not])
AC_DEFINE_UNQUOTED([HAVE_KONQUEROR], $HAVE_KONQUEROR, [Set to 1 if Konqueror exists, 0 if not])
AC_DEFINE_UNQUOTED([HAVE_OPERA], $HAVE_OPERA, [Set to 1 if Opera exists, 0 if not])
AC_DEFINE_UNQUOTED([HAVE_DILLO], $HAVE_DILLO, [Set to 1 if Dillo exists, 0 if not])
AC_DEFINE_UNQUOTED([HAVE_IEXPLORER], $HAVE_IEXPLORER, [Set to 1 if Internet Explorer exists, 0 if not])


dnl Checks for libraries.
AC_CHECK_LIB(stdc++, main,, AC_MSG_ERROR([Required library libstdc++ not found!]))
AC_CHECK_LIB(pthread, main)
AC_CHECK_LIB(socket, main)
AC_CHECK_LIB(ws2_32, main)
AC_CHECK_LIB(boost_filesystem, main,, AC_MSG_ERROR([Required library boost_filesystem not found!
Either it can't be found in your default link-path (check the LIBS-environment
variable) or install it using your package-manager or from its website at
http://www.boost.org]))

dnl Checks for header files.
AC_HEADER_DIRENT

AC_CHECK_HEADERS(regex.h syslog.h sys/socket.h sys/socket.h winsock2.h windows.h sys/wait.h)

AC_CHECK_HEADERS(boost/filesystem/path.hpp boost/shared_ptr.hpp \
                 boost/tokenizer.hpp boost/scoped_ptr.hpp       \
                 boost/spirit/include/classic_core.hpp,,
		 AC_MSG_ERROR([Required headers not found]))


dnl Check gtkmm version
PKG_CHECK_MODULES(GTK, gtk+-2.0 >= 2.0.0 gthread-2.0 > 2.2.0,
                  [AC_DEFINE(HAVE_GTK, 1, [Define if GTK is installed (in a fitting version)])],
		  [AC_MSG_NOTICE(No GTK+: Checks will be only textual)])
PKG_CHECK_MODULES(GTKMM, gtkmm-2.4 >= 2.4.0,
                  [[MYSUBDIRS="$MYSUBDIRS XGP"]],
                  [AC_MSG_WARN([Subdir XGP will not be made])])

if $PKG_CONFIG --exists "gtkmm-2.4 >= 2.6.0" ; then
   GTKMM_VERSION=`$PKG_CONFIG --modversion gtkmm-2.4`
   AC_DEFINE_UNQUOTED([HAVE_GTKMM26], $GTKMM_VERSION,
                      [Set to version if GTKMM is version 2.6 or above])
fi

PKG_CHECK_MODULES(GIOMM, giomm-2.4 >= 2.4.0,
                  [AC_DEFINE(HAVE_GIOMM, 1, [Define if Giomm is installed (in a fitting version)])],
                  [AC_MSG_WARN([Using own icon-handling])])

dnl Checking for GTKHTML which can be used
for version in 3.14 3.8 3.5 3.1; do
   PKG_CHECK_MODULES(GTKHTML, libgtkhtml-$version >= $version.0, [GTKHTML="yes"], [GTKHTML="no"])
   if test "$GTKHTML" = "yes"; then
      AC_DEFINE_UNQUOTED(HAVE_GTKHTML, "$version", [Define to API-version, if GTKHTML is installed])
      break;
   fi
done

dnl Doesn't seem to work with xulrunner-1.9, so disable it
dnl Checking for mozilla-embeded which can be used
dnl for variant in firefox seamonkey mozilla; do
dnl    PKG_CHECK_MODULES(GTKMOZEMBED, $variant-gtkmozembed, [GTKMOZEMBED="yes"], [GTKMOZEMBED="no"])
dnl    if test "$GTKMOZEMBED" = "yes"; then
dnl       gtkmoz=`pkg-config --modversion $variant-gtkmozembed`
dnl       AC_DEFINE_UNQUOTED(HAVE_GTKMOZEMBED, "$gtkmoz", [Set to the version of the GTKMOZEMBED library])
dnl       break;
dnl    fi
dnl done

dnl Checking for Webkit 1.0 which can be used (Webkit needs also gthread)
PKG_CHECK_MODULES(WEBKIT, webkit-1.0 >= 1.0.0,
                  [AC_DEFINE(HAVE_WEBKIT, 1, [Define if Webkit is installed])],
		  [WEBKIT="no"])

VIEWER_CFLAGS="$GTKHTML_CFLAGS $GTKMOZEMBED_CFLAGS $WEBKIT_CFLAGS"
AC_SUBST(MYSUBDIRS)
AC_SUBST(GTKMM_CFLAGS)
AC_SUBST(GIOMM_CFLAGS)
AC_SUBST(VIEWER_CFLAGS)
AC_SUBST(GIOMM_LIBS)
AC_SUBST(GTKMM_LIBS)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STAT
AC_STRUCT_TM
AC_CHECK_TYPES([ssize_t])
AC_CHECK_TYPES([socklen_t], [AC_DEFINE([HAVE_SOCKLEN_T], [1],
                                       [Whether we have socklen_t])],
               [], [AC_INCLUDES_DEFAULT
#include <sys/socket.h>])
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)
AC_DEFINE_UNQUOTED(SHAREOBJ_EXT, "$shrext_cmds", ["Extension of shared objects"])


dnl Checks for library functions.
AC_FUNC_FNMATCH
AC_FUNC_CLOSEDIR_VOID

AC_CHECK_FUNCS(timegm strfmon beginthread _beginthread pipe _pipe fork \
spawnvp sleep _sleep getlogin strftime strptime backtrace getpid _getpid \
wcscoll mbtowc tzset _tzset)

AC_MSG_CHECKING(if signal.h defines SIGBUS)
AC_CACHE_VAL(ac_cv_HAVE_SIGBUS, ac_cv_HAVE_SIGBUS=$ac_HAVE_SIGBUS)
AC_COMPILE_IFELSE([#include <signal.h>
                   int main () { signal (SIGBUS, SIG_DFL); return 0; }],
                  [AC_DEFINE(HAVE_SIGBUS, 1, [Define for defined SIGBUS])
                   HAVE_SIGBUS=yes])
AC_MSG_RESULT($HAVE_SIGBUS)

dnl Testing if strftime returns needed buffersize
dnl Do this at end, as this program traps under Windows (MSys) and so blocks
dnl the created executeable and then influences the next tests
AC_MSG_CHECKING(if strftime returns the needed length)
AC_RUN_IFELSE(
    [#include <cstdio>
     #include <ctime>
     int main () {
        time_t time (time (NULL));
        return strftime (NULL, 10, "%d", gmtime (&time)) > 0 ? 0 : 1; }],
    [AC_MSG_RESULT(yes)
     AC_DEFINE(STRFTIME_RETURNS_LENGTH, 1, [Defined if strftime returns the length it would need, when passed a NULL-buffer])],
    [AC_MSG_RESULT(no)],
    [AC_MSG_RESULT(no)
     AC_MSG_WARN([Disabling dynamic calculation of buffersize for strftime])])

dnl Enable special features
AC_ARG_ENABLE(debug,
              AS_HELP_STRING([[[[--enable-debug[=LEVEL]]]]],Enables additional traces),
              if test "$enableval" != "no"; then
	      	  test "$enableval" == "yes" && enableval=0
                  CXXFLAGS="-g -O2 -DTRACELEVEL=$enableval";
              else
                  CXXFLAGS="-O2 -UTRACELEVEL"; fi)
AC_ARG_ENABLE(checks,
              AS_HELP_STRING([[[[--enable-checks[=LEVEL]]]]],Enables additional (internal) checks),
              if test "$enableval" != "no"; then
	      	  test "$enableval" == "yes" && enableval=0
                  CXXFLAGS="$CXXFLAGS -DCHECK=$enableval";
              else
                  CXXFLAGS="$CXXFLAGS -DNDEBUG -UCHECK"; fi)
AC_ARG_WITH(corba,
            AC_HELP_STRING([--with-corba], [Enables the check for an CORBA-implementation]),
	    [with_corba=yes])
if test "$with_corba" == "yes"; then
   AC_CHECK_HEADERS([CORBA.h], [ac_cv_header_CORBA_h=yes],
                    [ac_cv_header_CORBA_h=no])
   if test "$ac_cv_header_CORBA_h" == "yes"; then
      MYSUBDIRS="$MYSUBDIRS CGP"
   else
      AC_MSG_WARN([CORBA is not installed; subdir CGP will not be made])
   fi
fi

LIB_GMP=""
AC_ARG_WITH(gmp,
            AC_HELP_STRING([--with-gmp], [Use GNU's multiple precision library (if available)]),
            [with_gmp=yes])
if test "$with_gmp" == "yes"; then
   AC_CHECK_LIB(gmp, main)
   if test $ac_cv_lib_gmp_main = yes; then
      LIB_GMP=-lgmp
   fi
fi
AC_SUBST(LIB_GMP)

AC_CONFIG_FILES([Makefile YGP/Makefile YGP/Tests/Makefile XGP/Makefile \
	   intl/Makefile XGP/Icons/Makefile CGP/Makefile po/Makefile.in \
           doc/Makefile CGP/Tests/Makefile bin/Makefile XGP/Samples/Makefile \
           YGP/YGP.pc XGP/XGP.pc doc/YGP.doxy doc/XGP.doxy CGP/CGP.pc])
AC_OUTPUT

dnl Settings for autoheader
AH_TOP([#ifndef YGP_CFG_H
#define YGP_CFG_H

// This file is part of libYGP.
//
// libYGP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libYGP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libYGP.  If not, see <http://www.gnu.org/licenses/>.

// Detect common platforms
#define WINDOWS 0
#define UNIX    1
#if defined(_WIN32)
#  if defined(__CYGWIN__)
#     define SYSTEM UNIX
#  else
#     define SYSTEM WINDOWS
#  endif
#else
#  define SYSTEM UNIX
#endif /* _WIN32 */

/* Define the name of the package */
#ifndef PACKAGE_NAME
#undef PACKAGE_NAME
#endif

/* Define the version number */
#ifndef PACKAGE_VERSION
#undef PACKAGE_VERSION
#endif

/* Define the release number */
#ifndef LIB_RELEASE
#undef LIB_RELEASE
#endif])

AH_BOTTOM([#endif])
