## Process this file with automake to produce Makefile.in

SUBDIRS=Icons

lib_LTLIBRARIES=libXAppl.la

include_HEADERS=Check.h XAbout.h XApplication.h XFileEntry.h XMessageBox.h \
		XFileList.h XDate.h XFileDlg.h XPrintDlg.h

libXAppl_la_LDFLAGS=-lAttr -lsigc `gtkmm-config --libs` \
                    -version-info $(LIBVERSION) -release $(LIBRELEASE) \
                    -no-undefined

libXAppl_la_SOURCES=Check.cpp XAbout.cpp XApplication.cpp XFileEntry.cpp \
		    XMessageBox.cpp Version.cpp XFileList.cpp XDate.cpp \
		    XFileDlg.cpp XPrintDlg.cpp $(include_HEADERS)

INCLUDES = -I../Common -DPKGDIR=\"$(pkgdatadir)\" `gtkmm-config --cflags`
