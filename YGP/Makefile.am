## Process this file with automake to produce Makefile.in

SUBDIRS=. Tests

lib_LTLIBRARIES=libYGP.la

# Settings for gettext
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@
AM_CXXFLAGS = -Woverloaded-virtual -Weffc++
DEFAULT_INCLUDES =
INCLUDES = -I $(top_srcdir) -I $(top_srcdir)/intl @GTK_CFLAGS@ -Wall -Wextra
LIBS = @LTLIBINTL@ @LIBS@

includedir = ${prefix}/include/YGP

libYGP_la_LDFLAGS=${LIBS} -release $(LIBRELEASE) -no-undefined

libYGP_la_SOURCES=Path.cpp IVIOAppl.cpp DirSrch.cpp PathSrch.cpp	\
ANumeric.cpp Parse.cpp FileRExp.cpp Version.cpp ATime.cpp XStrBuf.cpp	\
AYear.cpp ADate.cpp PathDirSrch.cpp ATStamp.cpp INIFile.cpp		\
CRegExp.cpp StackTrc.cpp Socket.cpp RDirSrch.cpp File.cpp		\
RDirSrchSrv.cpp AssParse.cpp AttrParse.cpp IDirSrch.cpp RemoteFile.cpp	\
Entity.cpp Thread.cpp XDirSrch.cpp Check.cpp Process.cpp ConnMgr.cpp	\
StatusObj.cpp Internal.h Relation.cpp TableWriter.cpp MetaEnum.cpp	\
DynTrace.cpp Module.cpp Attribute.cpp FileTypeChk.cpp Utility.cpp

include_HEADERS=Path.h ANumeric.h DirSrch.h IVIOAppl.h PathSrch.h	\
AttrVal.h FileRExp.h XStrBuf.h XStream.h Parse.h Trace.h Check.h	\
PathDirSrch.h AYear.h ADate.h ATStamp.h ATime.h CRegExp.h RegExp.h	\
Log.h StackTrc.h Socket.h RDirSrch.h IDirSrch.h File.h RDirSrchSrv.h	\
AssParse.h Attribute.h AttrParse.h RemoteFile.h Entity.h Thread.h	\
Mutex.h INIFile.h XDirSrch.h ConnMgr.h Process.h StatusObj.h		\
Relation.h TableWriter.h MetaEnum.h SortDirSrch.h DynTrace.h Module.h	\
Exception.h FileTypeChk.h Utility.h

pkgconfigdir =$(libdir)/pkgconfig
pkgconfig_DATA=YGP.pc
