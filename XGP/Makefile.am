## Process this file with automake to produce Makefile.in

SUBDIRS=Icons

lib_LTLIBRARIES=libXAppl.la

include_HEADERS=XAbout.h XApplication.h XFileEntry.h XMessageBox.h \
		XFileList.h XDate.h XFileDlg.h XPrintDlg.h XAttrLabel.h \
		XDialog.h

libXAppl_la_LDFLAGS=-lAttr -lsigc `gtkmm-config --libs` \
                    -version-info $(LIBVERSION) -release $(LIBRELEASE) \
                    -no-undefined

libXAppl_la_SOURCES=XAbout.cpp XApplication.cpp XFileEntry.cpp \
		    XMessageBox.cpp Version.cpp XFileList.cpp XDate.cpp \
		    XFileDlg.cpp XPrintDlg.cpp XDialog.cpp $(include_HEADERS)

INCLUDES = -I $(top_srcdir) -I . -I../Common -DPKGDIR=\"$(pkgdatadir)\" \
           `gtkmm-config --cflags` -I$(top_srcdir)/intl

# Settings for gettext
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@
LIBS = @INTLLIBS@ @LIBS@
