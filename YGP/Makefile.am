## Process this file with automake to produce Makefile.in

SUBDIRS=. Tests

lib_LTLIBRARIES=libYGP.la

# Settings for gettext
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@ -Wall
INCLUDES = -I $(top_srcdir) -I $(top_srcdir)/intl @GTK_CFLAGS@
LIBS = @LTLIBINTL@ @LIBS@ @GTK_LIBS@

includedir = ${prefix}/include/YGP

libYGP_la_LDFLAGS=-release $(LIBRELEASE) -no-undefined

libYGP_la_SOURCES=Tokenize.cpp IVIOAppl.cpp DirSrch.cpp ANumeric.cpp Parse.cpp \
	FileRExp.cpp Handle.cpp Version.cpp ATime.cpp XStrBuf.cpp ADate.cpp \
	PathDirSrch.cpp ATStamp.cpp INIFile.cpp CRegExp.cpp StackTrc.cpp \
	Socket.cpp RDirSrch.cpp File.cpp RDirSrchSrv.cpp AssParse.cpp \
	AttrParse.cpp IDirSrch.cpp RemoteFile.cpp Entity.cpp Thread.cpp \
	PathSrch.cpp XDirSrch.cpp Check.cpp Process.cpp ConnMgr.cpp  \
	StatusObj.cpp Internal.h

nobase_include_HEADERS=ANumeric.h DirSrch.h IVIOAppl.h Tokenize.h AttrVal.h \
	FileRExp.h PathSrch.h XStrBuf.h XStream.h Parse.h Trace.h Check.h \
	SmartPtr.h Handle.h PathDirSrch.h ADate.h ATStamp.h ATime.h \
	CRegExp.h RegExp.h Log.h StackTrc.h Socket.h RDirSrch.h IDirSrch.h \
	File.h RDirSrchSrv.h AssParse.h Attribute.h AttrParse.h RemoteFile.h \
	Entity.h Thread.h Mutex.h INIFile.h XDirSrch.h ConnMgr.h \
	Process.h StatusObj.h

pkgconfigdir =$(libdir)/pkgconfig
dist_pkgconfig_DATA=YGP.pc
