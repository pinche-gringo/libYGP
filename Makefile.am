## Process this file with automake to produce Makefile.in

SUBDIRS = intl m4 @MYSUBDIRS@ po bin
DIST_SUBDIRS = intl m4 YGP XGP CGP po doc bin

include_HEADERS=ygp-cfg.h config.h

EXTRA_DIST= config.rpath README.windows

# Add files for windows; don't make own Makefile[.am], as a
# Windows-compiler might need its own Makefile
dist-hook:
	mkdir $(distdir)/Windows
	cp -p $(srcdir)/Windows/*.mak $(srcdir)/Windows/*.ds?\
	      $(srcdir)/Windows/*.h $(distdir)/Windows

ACLOCAL_AMFLAGS = -I m4
