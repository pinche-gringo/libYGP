# Set common flags
CPP=BCC32.exe
LINK=ILink32.exe

CPPFLAGS=-c -I..\Common -I. -g100 -j50 -DWin32 -I$(INCLUDE) -D__MT__
LINKFLAGS=-E50 -q -Gi -L$LIB) -Gpr

INDIR=..\Common

!if "$(CFG)" != "Release" && "$(CFG)" != "Debug"
!if "$(CFG)" == ""
CFG=Debug
!MESSAGE Keine Konfiguration angegeben. Debug wird als Standard verwendet.
!else
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von MAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE
!MESSAGE MAKE -DCFG="Debug"
!MESSAGE
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE
!MESSAGE "Release"
!MESSAGE "Debug"
!MESSAGE
!ERROR Eine ungÅltige Konfiguration wurde angegeben.
!endif
!endif

!if $(CFG) == Debug
CPPFLAGS=$(CPPFLAGS) -v
LINKFLAGS=$(LINKFLAGS) -v
OUTDIR=Debug

!else
CPPFLAGS=$(CPPFLAGS) -Os

OUTDIR=Release
!endif

ALL: $(OUTDIR) $(OUTDIR)\BCGeneral.dll

$(OUTDIR) :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

.cpp.obj:
	$(CPP) $(CPPFLAGS) -o$@ $<

.SUFFIXES: .exe .obj .cpp

OBJECTS = $(OUTDIR)\AByteArray.obj $(OUTDIR)\ADate.obj \
     $(OUTDIR)\AssParse.obj $(OUTDIR)\ATime.obj $(OUTDIR)\ATStamp.obj \
     $(OUTDIR)\AttrParse.obj $(OUTDIR)\Check.obj $(OUTDIR)\CRegExp.obj \
     $(OUTDIR)\DirSrch.obj $(OUTDIR)\Entity.obj $(OUTDIR)\File.obj \
     $(OUTDIR)\FileRExp.obj $(OUTDIR)\Handle.obj $(OUTDIR)\IDirSrch.obj \
     $(OUTDIR)\INIFile.obj $(OUTDIR)\IVIOAppl.obj $(OUTDIR)\Parse.obj \
     $(OUTDIR)\PathDirSrch.obj $(OUTDIR)\PathSrch.obj $(OUTDIR)\RDirSrch.obj \
     $(OUTDIR)\RDirSrchSrv.obj $(OUTDIR)\RemoteFile.obj $(OUTDIR)\Socket.obj \
     $(OUTDIR)\StackTrc.obj $(OUTDIR)\Thread.obj $(OUTDIR)\Tokenize.obj \
     $(OUTDIR)\Version.obj $(OUTDIR)\XStrBuf.obj $(OUTDIR)\ANumeric.obj \
     $(OUTDIR)\Process.obj $(OUTDIR)\XDirSrch.obj


$(OUTDIR)\BCGeneral.dll: $(OBJECTS)
	$(LINK) $(LINKFLAGS) -Tpd $(OBJECTS), $@

$(OUTDIR)\AByteArray.obj: $(INDIR)\AByteArray.cpp

$(OUTDIR)\ADate.obj: $(INDIR)\ADate.cpp

$(OUTDIR)\AssParse.obj: $(INDIR)\AssParse.cpp

$(OUTDIR)\ATime.obj: $(INDIR)\ATime.cpp

$(OUTDIR)\ATStamp.obj: $(INDIR)\ATStamp.cpp

$(OUTDIR)\AttrParse.obj: $(INDIR)\AttrParse.cpp

$(OUTDIR)\Check.obj: $(INDIR)\Check.cpp

$(OUTDIR)\CRegExp.obj: $(INDIR)\CRegExp.cpp

$(OUTDIR)\IDirSrch.obj: $(INDIR)\IDirSrch.cpp

$(OUTDIR)\DirSrch.obj: $(INDIR)\DirSrch.cpp

$(OUTDIR)\File.obj: $(INDIR)\File.cpp

$(OUTDIR)\FileRExp.obj: $(INDIR)\FileRExp.cpp

$(OUTDIR)\Handle.obj: $(INDIR)\Handle.cpp

$(OUTDIR)\INIFile.obj: $(INDIR)\INIFile.cpp

$(OUTDIR)\Parse.obj: $(INDIR)\Parse.cpp

$(OUTDIR)\Entity.obj: $(INDIR)\Entity.cpp

$(OUTDIR)\IVIOAppl.obj: $(INDIR)\IVIOAppl.cpp

$(OUTDIR)\PathDirSrch.obj: $(INDIR)\PathDirSrch.cpp

$(OUTDIR)\PathSrch.obj: $(INDIR)\PathSrch.cpp

$(OUTDIR)\RDirSrch.obj: $(INDIR)\RDirSrch.cpp

$(OUTDIR)\RDirSrchSrv.obj: $(INDIR)\RDirSrchSrv.cpp

$(OUTDIR)\RemoteFile.obj: $(INDIR)\RemoteFile.cpp

$(OUTDIR)\Socket.obj: $(INDIR)\Socket.cpp

$(OUTDIR)\StackTrc.obj: $(INDIR)\StackTrc.cpp

$(OUTDIR)\Thread.obj: $(INDIR)\Thread.cpp

$(OUTDIR)\Tokenize.obj: $(INDIR)\Tokenize.cpp

$(OUTDIR)\Version.obj: $(INDIR)\Version.cpp

$(OUTDIR)\XStrBuf.obj: $(INDIR)\XStrBuf.cpp

$(OUTDIR)\Process.obj: $(INDIR)\Process.cpp

$(OUTDIR)\ANumeric.obj: $(INDIR)\ANumeric.cpp

$(OUTDIR)\XDirSrch.obj: $(INDIR)\XDirSrch.cpp

check:
