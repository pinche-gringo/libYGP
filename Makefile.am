## Process this file with automake to produce Makefile.in

SUBDIRS = intl @MYSUBDIRS@ po bin
DIST_SUBDIRS = intl YGP XGP po doc bin

include_HEADERS=ygp-cfg.h config.h

EXTRA_DIST= config.rpath

ACLOCAL_AMFLAGS = -I m4
LIBS = @LTLIBINTL@ @LIBS@
