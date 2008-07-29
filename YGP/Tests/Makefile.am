SUBDIRS=.

INCLUDES=-I$(top_srcdir)

# Settings for gettext
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@ -DLIBRELEASE=\"$(LIBRELEASE)\"
LIBS = -L.. -lYGP @LTLIBINTL@ @LIBS@

TESTS=IVIOAppl ANumeric AYear ADate ATime ATStamp XStrBuf XStream Parse \
      FileRExp CRegExp Tokenize PathSrch DirSrch PathDirSrch Attribute \
      AssParse AttrParse INIFile Handle Thread Relation Process MetaEnum \
      Entity SortDirSrch Module FileTypeChk

noinst_PROGRAMS=$(TESTS)

IVIOAppl_SOURCES=IVIOAppl.cpp

ANumeric_SOURCES=ANumeric.cpp

ADate_SOURCES=ADate.cpp

ATime_SOURCES=ATime.cpp

AYear_SOURCES=AYear.cpp

ATStamp_SOURCES=ATStamp.cpp

XStrBuf_SOURCES=XStrBuf.cpp

XStream_SOURCES=XStream.cpp

Parse_SOURCES=Parse.cpp

CRegExp_SOURCES=CRegExp.cpp

FileRExp_SOURCES=FileRExp.cpp

Tokenize_SOURCES=Tokenize.cpp

PathSrch_SOURCES=PathSrch.cpp

DirSrch_SOURCES=DirSrch.cpp

PathDirSrch_SOURCES=PathDirSrch.cpp

Attribute_SOURCES=Attribute.cpp

AssParse_SOURCES=AssParse.cpp

AttrParse_SOURCES=AttrParse.cpp

INIFile_SOURCES=INIFile.cpp

Handle_SOURCES=Handle.cpp

Thread_SOURCES=Thread.cpp

Relation_SOURCES=Relation.cpp

Process_SOURCES=Process.cpp

MetaEnum_SOURCES=MetaEnum.cpp

Entity_SOURCES=Entity.cpp

SortDirSrch_SOURCES=SortDirSrch.cpp

Module_SOURCES=Module.cpp

FileTypChk_SOURCES=FileTypChk.cpp

EXTRA_DIST=Parser.test INIFile.test CRegExp.test Test.h XStrBuf.test
