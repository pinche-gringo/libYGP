## Process this file with automake to produce Makefile.in

SUBDIRS=Icons
DIST_SUBDIRS=$(SUBDIRS) Samples

LIBS = @LTLIBINTL@ @LIBS@ @GTK_LIBS@ @GIOMM_LIBS@ @GTKMM_LIBS@

lib_LTLIBRARIES=libXGP.la

includedir = ${prefix}/include/XGP

include_HEADERS=XAbout.h XApplication.h XFileEntry.h BrowserDlg.h	\
XFileList.h XDate.h XFileDlg.h XPrintDlg.h XAttrLabel.h XDialog.h	\
XAttrEntry.h XAttribute.h ConnectDlg.h HTMLViewer.h GTKViewer.h		\
AutoContainer.h MessageDlg.h LoginDlg.h XValue.h SearchDlg.cpp		\
SearchDlg.h EnumEntry.h XAttrSpin.h GtkMozViewer.h TraceDlg.h		\
AnimWindow.h WebkitViewer.h

libXGP_la_SOURCES=XDialog.cpp XAbout.cpp XApplication.cpp		\
XFileEntry.cpp Version.cpp XFileList.cpp XDate.cpp XFileDlg.cpp		\
XPrintDlg.cpp BrowserDlg.cpp ConnectDlg.cpp HTMLViewer.cpp GTKViewer.c	\
AutoContainer.cpp MessageDlg.cpp LoginDlg.cpp EnumEntry.cpp		\
GtkMozViewer.c TraceDlg.cpp AnimWindow.cpp WebkitViewer.c		\
$(include_HEADERS)

libXGP_la_LDFLAGS=$(LIBS) -release $(LIBRELEASE) -no-undefined

INCLUDES = -I $(top_srcdir) -I . -DPKGDIR=\"$(datadir)/XGP/\"	\
           @GIOMM_CFLAGS@ @GTKMM_CFLAGS@ @VIEWER_CFLAGS@	\
           -I$(top_srcdir)/intl -Wextra -Wall
AM_CXXFLAGS = -Woverloaded-virtual -Weffc++ -Wextra

# Settings for gettext
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

pkgconfigdir=$(libdir)/pkgconfig
pkgconfig_DATA=XGP.pc
