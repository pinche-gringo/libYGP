## Process this file with automake to produce Makefile.in

# SUBDIRS = intl Common X-windows CORBA po
SUBDIRS = intl @SUBDIRS@ po
DIST_SUBDIRS = intl Common CORBA X-windows po

include_HEADERS=gzo-cfg.h

dist-hook:
	# Add files for windows; don't make own Makefile[.am], as one
	# Windows-compiler might need its own Makefile
	mkdir $(distdir)/Windows
	cp -p $(srcdir)/Windows/*.ds? $(srcdir)/Windows/*.mak \
              $(srcdir)/Windows/*.def $(distdir)/Windows
	cp -p  $(srcdir)/Make.bat $(distdir)/
	# Finally add documentation
	mkdir $(distdir)/doc
	cp -p $(srcdir)/doc/*.html.* $(distdir)/doc
