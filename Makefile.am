## Process this file with automake to produce Makefile.in

include_HEADERS=ANumeric.h DirSrch.h IVIOAppl.h Tokenize.h AttrVal.h \
		FileRExp.h PathSrch.h XStrBuf.h XStream.h Parse.h Trace.h \
		SmartPtr.h Handle.h PathDirSrch.h ADate.h ATStamp.h

lib_LTLIBRARIES=libPort.la
noinst_PROGRAMS=Test
EXTRA_DIST=Test.Dat
libPort_la_SOURCES=PathSrch.cpp Tokenize.cpp IVIOAppl.cpp DirSrch.cpp \
	           ANumeric.cpp FileRExp.cpp Parse.cpp Handle.cpp Version.cpp \
		   XStrBuf.cpp PathDirSrch.cpp ADate.cpp ATStamp.cpp
Test_SOURCES=$(libPort_la_SOURCES) Test.cpp
