## Process this file with automake to produce Makefile.in

SUBDIRS = intl @SUBDIRS@ po
DIST_SUBDIRS = intl Common CORBA X-windows po doc

include_HEADERS=gzo-cfg.h config.h

EXTRA_DIST=README.windows

dist-hook:
	# Add files for windows; don't make own Makefile[.am], as one
	# Windows-compiler might need its own Makefile
	mkdir $(distdir)/Windows
	cp -p $(srcdir)/Windows/*.mak $(srcdir)/Windows/*.def \
	      $(srcdir)/Windows/*.ds? $(srcdir)/Windows/*.h $(distdir)/Windows
	cp -p  $(srcdir)/Make.bat $(distdir)/
