SUBDIRS=.

INCLUDES=-I.. -I$(top_srcdir)
INCLUDES = -I$(top_srcdir) -I.. -I../../Common @GTKMM_CFLAGS@ -I$(top_srcdir)/intl

# Settings for gettext
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

LIBS = @LIBS@ @GTKMM_LIBS@

TESTS=X-Appl

noinst_PROGRAMS=$(TESTS)

X_Appl_SOURCES=X-Appl.cpp X-Appl.h Dialog.cpp Dialog.h
X_Appl_LDADD=../XApplication.o -lAttr ../XFileList.o ../XFileDlg.o ../XDate.o \
	     ../XPrintDlg.o ../XAbout.o ../XDialog.o ../BrowserDlg.o \
	     ../XFileEntry.o
