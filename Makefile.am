## Process this file with automake to produce Makefile.in

SUBDIRS = Common X-windows CORBA

dist-hook:
	mkdir $(distdir)/Windows
	cp -p $(srcdir)/Windows/*.ds? $(srcdir)/Windows/*.mak \
              $(srcdir)/Windows/*.def $(distdir)/Windows
	mkdir $(distdir)/doc
	cp -p $(srcdir)/doc/*.html $(distdir)/doc
