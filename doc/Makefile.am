## Process this file with automake to produce Makefile.in

SUBDIRS=.

EXTRA_DIST=index.html YGP.doxy.in XGP.doxy.in footer.txt

doc: YGP.doxy XGP.doxy footer.txt
	doxygen YGP.doxy
	doxygen XGP.doxy

sf-install:
	 scp -r index.html XGP YGP g17m0,libymp@web.sourceforge.net:htdocs/doc
