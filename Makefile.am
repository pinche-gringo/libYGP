## Process this file with automake to produce Makefile.in

SUBDIRS = intl @SUBDIRS@ po
DIST_SUBDIRS = intl Common CORBA X-windows po doc

include_HEADERS=gzo-cfg.h

dist-hook:
	# Add files for windows; don't make own Makefile[.am], as one
	# Windows-compiler might need its own Makefile
	mkdir $(distdir)/Windows
	cp -p $(srcdir)/Windows/*.ds? $(srcdir)/Windows/*.mak \
              $(srcdir)/Windows/*.def $(distdir)/Windows
	cp -p  $(srcdir)/Make.bat $(distdir)/
