## Process this file with automake to produce Makefile.in

SUBDIRS=Icons
DIST_SUBDIRS=$(SUBDIRS) Samples

lib_LTLIBRARIES=libXAppl.la

include_HEADERS=XAbout.h XApplication.h XFileEntry.h XMessageBox.h \
		XFileList.h XDate.h XFileDlg.h XPrintDlg.h XAttrLabel.h \
		XDialog.h XAttrEntry.h BrowserDlg.h

libXAppl_la_SOURCES=XAbout.cpp XApplication.cpp XFileEntry.cpp \
		    XMessageBox.cpp Version.cpp XFileList.cpp XDate.cpp \
		    XFileDlg.cpp XPrintDlg.cpp XDialog.cpp BrowserDlg.cpp \
		    $(include_HEADERS) 

libXAppl_la_LDFLAGS=-lsigc @GTKMM_LIBS@ -lAttr \
                    -version-info $(LIBVERSION) -release $(LIBRELEASE) \
                    -no-undefined

INCLUDES = -I $(top_srcdir) -I . -I../Common -DPKGDIR=\"$(pkgdatadir)\" \
           @GTKMM_CFLAGS@ -I$(top_srcdir)/intl

# Settings for gettext
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@
LIBS = @INTLLIBS@ @LIBS@
