SUBDIRS=.

INCLUDES = -I$(top_srcdir) -I.. -I../../Common @GTKMM_CFLAGS@ -I$(top_srcdir)/intl -Wall

# Settings for gettext
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

LIBS = ../libXGP.la ../../Common/libYGP.la @LIBS@ @GTKMM_LIBS@

TESTS=X-Appl

noinst_PROGRAMS=$(TESTS)

X_Appl_SOURCES=X-Appl.cpp X-Appl.h Dialog.cpp Dialog.h
