## Process this file with automake to produce Makefile.in

include_HEADERS=ANumeric.h DirSrch.h IVIOAppl.h Tokenize.h AttrVal.h \
		FileRExp.h PathSrch.h XStrBuf.h XStream.h Parse.h Trace.h \
		SmartPtr.h Handle.h PathDirSrch.h ADate.h ATStamp.h ATime.h \
		Check.h XAbout.h XApplication.h XFileEntry.h XMessageBox.h \
		XFileList.h XDate.h

lib_LTLIBRARIES=libAttr.la libXAppl.la

noinst_PROGRAMS=Test
EXTRA_DIST=Test.Dat

libAttr_la_SOURCES=PathSrch.cpp Tokenize.cpp IVIOAppl.cpp DirSrch.cpp \
	           ANumeric.cpp FileRExp.cpp Parse.cpp Handle.cpp Version.cpp \
		   XStrBuf.cpp PathDirSrch.cpp ADate.cpp ATStamp.cpp ATime.cpp

libXAppl_la_LIBADD=-lAttr -lgtkmm -lgdkmm -lgtk
libXAppl_la_SOURCES=Check.cpp XAbout.cpp XApplication.cpp XFileEntry.cpp \
		    XMessageBox.cpp Version.cpp XFileList.cpp XDate.cpp

Test_SOURCES=$(libAttr_la_SOURCES) Test.cpp

pkgdata_DATA=Icon.c Icon.gif Icon.html Icon.jpg Icon.ps Icon.tgz Icon.xpm \
             Icon.cpp Icon.gz Icon.info Icon.man Icon.rpm Icon.tif Icon.z \
             Icon.doc Icon.h  Icon.java Icon.mp3 Icon.tar Icon.txt Icon.dvi \
             Icon.htm Icon.jpeg Icon.pdf Icon.tex Icon.wav Icon.zip Icon.bmp

EXTRA_DIST=$(pkgdata_DATA)
