## Process this file with automake to produce Makefile.in

include_HEADERS=ANumeric.h DirSrch.h IVIOAppl.h Tokenize.h AttrVal.h \
		FileRExp.h PathSrch.h XStrBuf.h XStream.h Parse.h Trace.h \
		SmartPtr.h Handle.h


lib_LTLIBRARIES=libPort.la
noinst_PROGRAMS=Test
noinst_DATA=Test.Dat
Test_SOURCES=XStrBuf.cpp Test.cpp PathSrch.cpp Tokenize.cpp IVIOAppl.cpp \
	     DirSrch.cpp ANumeric.cpp FileRExp.cpp Parse.cpp Handle.cpp \
	     gmp_mpz.c
libPort_la_SOURCES=PathSrch.cpp Tokenize.cpp IVIOAppl.cpp DirSrch.cpp \
	           ANumeric.cpp FileRExp.cpp Parse.cpp Handle.cpp Version.cpp \
		   XStrBuf.cpp gmp_mpz.c