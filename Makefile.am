## Process this file with automake to produce Makefile.in

include_HEADERS=ANumeric.h DirSrch.h IVIOAppl.h Tokenize.h AttrVal.h \
		FileRExp.h PathSrch.h XStrBuf.h XStream.h Parse.h Trace.h \
		SmartPtr.h Handle.h PathDirSrch.h gmp.h

lib_LTLIBRARIES=libPort.la
noinst_PROGRAMS=Test
EXTRA_DIST=Test.Dat
Test_SOURCES=XStrBuf.cpp Test.cpp PathSrch.cpp Tokenize.cpp IVIOAppl.cpp \
	     DirSrch.cpp ANumeric.cpp FileRExp.cpp Parse.cpp Handle.cpp \
	     gmp_mpz.c gmp.h gmp-impl.h gmp-mparam.h longlong.h PathDirSrch.h \
	     PathDirSrch.cpp
libPort_la_SOURCES=PathSrch.cpp Tokenize.cpp IVIOAppl.cpp DirSrch.cpp \
	           ANumeric.cpp FileRExp.cpp Parse.cpp Handle.cpp Version.cpp \
		   XStrBuf.cpp gmp_mpz.c gmp.h gmp-impl.h gmp-mparam.h \
		   longlong.h PathDirSrch.cpp PathDirSrch.h
