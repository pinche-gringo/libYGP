SUBDIRS=.

INCLUDES = -I$(top_srcdir) @GTKMM_CFLAGS@ -I$(top_srcdir)/intl  -I$(top_srcdir)/YGP -Wall -Wextra -Woverloaded-virtual -Weffc++

# Settings for gettext
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

LIBS = ../libXGP.la ../../YGP/libYGP.la @LTLIBINTL@ @LIBS@ @GTKMM_LIBS@

TESTS=X-Appl

noinst_PROGRAMS=$(TESTS)

X_Appl_SOURCES=X-Appl.cpp X-Appl.h Dialog.cpp Dialog.h AnimWindow.cpp \
	AnimWindow.h images.h
