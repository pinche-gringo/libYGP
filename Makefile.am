 ## Process this file with automake to produce Makefile.in

LDADD = libPort.la

include_HEADERS=ANumeric.h DirSrch.h IVIOAppl.h Tokenize.h AttrVal.h \
		FileRExp.h PathSrch.h XStrBuf.h XStream.h Parse.h Trace.h


lib_LTLIBRARIES=libPort.la
noinst_PROGRAMS=Test
Test_SOURCES=XStrBuf.cpp Test.cpp PathSrch.cpp Tokenize.cpp IVIOAppl.cpp \
	     DirSrch.cpp ANumeric.cpp FileRExp.cpp Parse.cpp
libPort_la_SOURCES=PathSrch.cpp Tokenize.cpp IVIOAppl.cpp DirSrch.cpp \
	           ANumeric.cpp FileRExp.cpp Parse.cpp
