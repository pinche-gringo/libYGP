SUBDIRS=.

INCLUDES=-I.. -I$(top_srcdir)
INCLUDES = -I$(top_srcdir) -I. -I.. -I../Common `gtkmm-config --cflags` -I$(top_srcdir)/intl

# Settings for gettext
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

LIBS = @LIBS@ @GTK_LIBS@

TESTS=X-Appl

noinst_PROGRAMS=$(TESTS)

X_Appl_SOURCES=X-Appl.cpp X-Appl.h
X_Appl_LDADD=../XApplication.o -lAttr ../XFileList.o ../XFileDlg.o ../XPrintDlg.o \
	     ../XAbout.o ../XMessageBox.o ../XDialog.o ../BrowserDlg.o
