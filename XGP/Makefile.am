## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES=libXAppl.la

libXAppl_la_LIBADD=-lAttr -lgtkmm -lgdkmm -lgtk
libXAppl_la_SOURCES=Check.cpp XAbout.cpp XApplication.cpp XFileEntry.cpp \
		    XMessageBox.cpp Version.cpp XFileList.cpp XDate.cpp

pkgdata_DATA=Icon.c Icon.gif Icon.html Icon.jpg Icon.ps Icon.tgz Icon.xpm \
             Icon.cpp Icon.gz Icon.info Icon.man Icon.rpm Icon.tif Icon.z \
             Icon.doc Icon.h  Icon.java Icon.mp3 Icon.tar Icon.txt Icon.dvi \
             Icon.htm Icon.jpeg Icon.pdf Icon.tex Icon.wav Icon.zip Icon.bmp

EXTRA_DIST=$(pkgdata_DATA)

include_HEADERS=Check.h XAbout.h XApplication.h XFileEntry.h XMessageBox.h \
		XFileList.h XDate.h

INCLUDES = -I../Common -DPKGDIR=\"$(pkgdata_dir)\"
