## Process this file with automake to produce Makefile.in

SUBDIRS=. Tests

lib_LTLIBRARIES=libAttr.la

# Settings for gettext
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@
INCLUDES = -I $(top_srcdir) -I . -I $(top_srcdir)/intl
LIBS = @INTLLIBS@ @LIBS@

libAttr_la_LDFLAGS=-version-info $(LIBVERSION) -release $(LIBRELEASE) \
                   -no-undefined

libAttr_la_SOURCES=Tokenize.cpp IVIOAppl.cpp DirSrch.cpp ANumeric.cpp \
                   FileRExp.cpp Parse.cpp Handle.cpp Version.cpp ATime.cpp \
		   XStrBuf.cpp PathDirSrch.cpp ADate.cpp ATStamp.cpp \
		   INIFile.cpp CRegExp.cpp StackTrc.cpp \
		   AByteArray.cpp Socket.cpp RDirSrch.cpp File.cpp \
		   RDirSrchSrv.cpp AssParse.cpp AttrParse.cpp IDirSrch.cpp \
		   RemoteFile.cpp Entity.cpp Thread.cpp PathSrch.cpp\
		   XDirSrch.cpp

include_HEADERS=ANumeric.h DirSrch.h IVIOAppl.h Tokenize.h AttrVal.h \
		FileRExp.h PathSrch.h XStrBuf.h XStream.h Parse.h Trace_.h \
		SmartPtr.h Handle.h PathDirSrch.h ADate.h ATStamp.h ATime.h \
		CRegExp.h RegExp.h Log.h StackTrc.h AByteArray.h Socket.h \
		RDirSrch.h IDirSrch.h File.h RDirSrchSrv.h AssParse.h \
		Attribute.h AttrParse.h RemoteFile.h Entity.h Internal.h \
		Thread.h Mutex.h INIFile.h XDirSrch.h
