SUBDIRS=.

INCLUDES=-I..

TESTS=IVIOAppl ANumeric ADate ATime ATStamp AByteArray XStrBuf XStream Parse \
      FileRExp CRegExp Tokenize PathSrch DirSrch PathDirSrch Attribute \
      AssParse AttrParse INIFile Handle

noinst_PROGRAMS=$(TESTS)

IVIOAppl_SOURCES=IVIOAppl.cpp
IVIOAppl_LDADD=../IVIOAppl.o ../StackTrc.o

ANumeric_SOURCES=ANumeric.cpp
ANumeric_LDADD=../ANumeric.o

ADate_SOURCES=ADate.cpp
ADate_LDADD=../ADate.o

ATime_SOURCES=ATime.cpp
ATime_LDADD=../ATime.o

ATStamp_SOURCES=ATStamp.cpp
ATStamp_LDADD=../ATStamp.o ../ATime.o ../ADate.o

AByteArray_SOURCES=AByteArray.cpp
AByteArray_LDADD=../AByteArray.o

XStrBuf_SOURCES=XStrBuf.cpp
XStrBuf_LDADD=../XStrBuf.o

XStream_SOURCES=XStream.cpp
XStream_LDADD=../XStrBuf.o

Parse_SOURCES=Parse.cpp
Parse_LDADD=../Parse.o ../IVIOAppl.o ../XStrBuf.o ../StackTrc.o

CRegExp_SOURCES=CRegExp.cpp
CRegExp_LDADD=../CRegExp.o ../Parse.o ../ANumeric.o ../XStrBuf.o ../StackTrc.o

FileRExp_SOURCES=FileRExp.cpp
FileRExp_LDADD=../FileRExp.o ../ANumeric.o

Tokenize_SOURCES=Tokenize.cpp
Tokenize_LDADD=../Tokenize.o

PathSrch_SOURCES=PathSrch.cpp
PathSrch_LDADD=../Tokenize.o

DirSrch_SOURCES=DirSrch.cpp
DirSrch_LDADD=../DirSrch.o ../IDirSrch.o ../File.o ../FileRExp.o \
              ../ANumeric.o

PathDirSrch_SOURCES=PathDirSrch.cpp
PathDirSrch_LDADD=../PathDirSrch.o ../DirSrch.o ../IDirSrch.o ../File.o \
                  ../FileRExp.o ../ANumeric.o ../Tokenize.o

Attribute_SOURCES=Attribute.cpp
Attribute_LDADD=../ATime.o

AssParse_SOURCES=AssParse.cpp
AssParse_LDADD=../AssParse.o ../Tokenize.o

AttrParse_SOURCES=AttrParse.cpp
AttrParse_LDADD=../AttrParse.o ../Tokenize.o ../AssParse.o ../ANumeric.o \
                ../ATime.o ../ADate.o ../ATStamp.o

INIFile_SOURCES=INIFile.cpp
INIFile_LDADD=../INIFile.o ../XStrBuf.o ../Parse.o ../ANumeric.o ../ADate.o \
               ../ATime.o ../ATStamp.o

Handle_SOURCES=Handle.cpp
Handle_LDADD=../Handle.o

EXTRA_DIST=Parsertest.dat INIFile.test CRegExp.test Test.h
