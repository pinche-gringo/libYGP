SUBDIRS=.

INCLUDES=-I.. -I$(top_srcdir)
INCLUDES = -I$(top_srcdir) -I.. -I../../Common @GTKMM_CFLAGS@ -I$(top_srcdir)/intl

# Settings for gettext
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@
CXXFLAGS = @CXXFLAGS@ -Wall

LIBS = ../libXAppl.la ../../Common/libAttr.la @LIBS@ @GTKMM_LIBS@

TESTS=X-Appl

noinst_PROGRAMS=$(TESTS)

X_Appl_SOURCES=X-Appl.cpp X-Appl.h Dialog.cpp Dialog.h
