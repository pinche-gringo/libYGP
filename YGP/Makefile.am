## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES=libAttr.la
noinst_PROGRAMS=Test

libAttr_la_SOURCES=Tokenize.cpp IVIOAppl.cpp DirSrch.cpp ANumeric.cpp \
                   FileRExp.cpp Parse.cpp Handle.cpp Version.cpp ATime.cpp \
		   XStrBuf.cpp PathDirSrch.cpp ADate.cpp ATStamp.cpp \
		   INIFile.h INIFile.cpp CRegExp.cpp
Test_SOURCES=$(libAttr_la_SOURCES) Test.cpp

include_HEADERS=ANumeric.h DirSrch.h IVIOAppl.h Tokenize.h AttrVal.h \
		FileRExp.h PathSrch.h XStrBuf.h XStream.h Parse.h Trace_.h \
		SmartPtr.h Handle.h PathDirSrch.h ADate.h ATStamp.h ATime.h \
		CRegExp.h RegExp.h

EXTRA_DIST=Test.Dat Test.ini Test.rex
