## Process this file with automake to produce Makefile.in

# SUBDIRS = Common X-windows CORBA
SUBDIRS = intl @SUBDIRS@ po

include_HEADERS=gzo-cfg.h

# Settings for gettext
localedir = $(datadir)/locale
DEFS=-DLOCALEDIR=\"$(localedir)\" @DEFS@

dist-hook:
	mkdir $(distdir)/Windows
	cp -p $(srcdir)/Windows/*.ds? $(srcdir)/Windows/*.mak \
              $(srcdir)/Windows/*.def $(distdir)/Windows
	mkdir $(distdir)/doc
	cp -p $(srcdir)/doc/*.html $(distdir)/doc
