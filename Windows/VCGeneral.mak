# Microsoft Developer Studio Generated NMAKE File, Based on VCGeneral.dsp
!IF "$(CFG)" == ""
CFG=Debug
!MESSAGE Keine Konfiguration angegeben. VCGeneral - Win32 Debug wird als Standard verwendet.
!ENDIF

DEST_INCLUDE=..\..\Include
DEST_LIB=..\..\Lib
DEST_DLL=..\..\..\Programme\Utilities

!IF "$(CFG)" != "Release" && "$(CFG)" != "Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE
!MESSAGE NMAKE /f "VCGeneral.mak" CFG="Debug"
!MESSAGE
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE
!MESSAGE "Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE
!ERROR Eine ungÅltige Konfiguration wurde angegeben.
!ENDIF

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF

!IF  "$(CFG)" == "Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\VCGeneral.dll"


CLEAN :
	-@erase "$(INTDIR)\ADate.obj"
	-@erase "$(INTDIR)\ANumeric.obj"
	-@erase "$(INTDIR)\AssParse.obj"
	-@erase "$(INTDIR)\ATime.obj"
	-@erase "$(INTDIR)\ATStamp.obj"
	-@erase "$(INTDIR)\AttrParse.obj"
	-@erase "$(INTDIR)\Check.obj"
	-@erase "$(INTDIR)\CRegExp.obj"
	-@erase "$(INTDIR)\DirSrch.obj"
	-@erase "$(INTDIR)\Entity.obj"
	-@erase "$(INTDIR)\File.obj"
	-@erase "$(INTDIR)\FileRExp.obj"
	-@erase "$(INTDIR)\Handle.obj"
	-@erase "$(INTDIR)\IDirSrch.obj"
	-@erase "$(INTDIR)\INIFile.obj"
	-@erase "$(INTDIR)\IVIOAppl.obj"
	-@erase "$(INTDIR)\Parse.obj"
	-@erase "$(INTDIR)\PathDirSrch.obj"
	-@erase "$(INTDIR)\PathSrch.obj"
	-@erase "$(INTDIR)\RDirSrch.obj"
	-@erase "$(INTDIR)\RDirSrchSrv.obj"
	-@erase "$(INTDIR)\RemoteFile.obj"
	-@erase "$(INTDIR)\Socket.obj"
	-@erase "$(INTDIR)\StackTrc.obj"
	-@erase "$(INTDIR)\Thread.obj"
	-@erase "$(INTDIR)\Tokenize.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\Version.obj"
	-@erase "$(INTDIR)\XDirSrch.obj"
	-@erase "$(INTDIR)\XStrBuf.obj"
	-@erase "$(INTDIR)\Process.obj"
	-@erase "$(OUTDIR)\VCGeneral.dll"
	-@erase "$(OUTDIR)\VCGeneral.exp"
	-@erase "$(OUTDIR)\VCGeneral.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I ".." /I "../Windows" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "VCGENERAL_EXPORTS" /Fp"$(INTDIR)\VCGeneral.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\VCGeneral.bsc"
BSC32_SBRS= \

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\VCGeneral.pdb" /machine:I386 /def:"$(OUTDIR)\VCGeneral.def" /out:"$(OUTDIR)\VCGeneral.dll" /implib:"$(OUTDIR)\VCGeneral.lib"
DEF_FILE= \
        "$(OUTDIR)\VCGeneral.def"
LINK32_OBJS= \
	"$(INTDIR)\ADate.obj" \
	"$(INTDIR)\ANumeric.obj" \
	"$(INTDIR)\AssParse.obj" \
	"$(INTDIR)\ATime.obj" \
	"$(INTDIR)\ATStamp.obj" \
	"$(INTDIR)\AttrParse.obj" \
	"$(INTDIR)\Check.obj" \
	"$(INTDIR)\CRegExp.obj" \
	"$(INTDIR)\DirSrch.obj" \
	"$(INTDIR)\Entity.obj" \
	"$(INTDIR)\File.obj" \
	"$(INTDIR)\FileRExp.obj" \
	"$(INTDIR)\Handle.obj" \
	"$(INTDIR)\IDirSrch.obj" \
	"$(INTDIR)\INIFile.obj" \
	"$(INTDIR)\IVIOAppl.obj" \
	"$(INTDIR)\Parse.obj" \
	"$(INTDIR)\PathDirSrch.obj" \
	"$(INTDIR)\PathSrch.obj" \
	"$(INTDIR)\RDirSrch.obj" \
	"$(INTDIR)\RDirSrchSrv.obj" \
	"$(INTDIR)\RemoteFile.obj" \
	"$(INTDIR)\Socket.obj" \
	"$(INTDIR)\StackTrc.obj" \
	"$(INTDIR)\Thread.obj" \
	"$(INTDIR)\Tokenize.obj" \
	"$(INTDIR)\Version.obj" \
	"$(INTDIR)\XStrBuf.obj" \
        "$(INTDIR)\XDirSrch.obj" \
	"$(INTDIR)\Process.obj"

"$(OUTDIR)\VCGeneral.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\VCGeneral.dll"


CLEAN :
	-@erase "$(INTDIR)\ADate.obj"
	-@erase "$(INTDIR)\ANumeric.obj"
	-@erase "$(INTDIR)\AssParse.obj"
	-@erase "$(INTDIR)\ATime.obj"
	-@erase "$(INTDIR)\ATStamp.obj"
	-@erase "$(INTDIR)\AttrParse.obj"
	-@erase "$(INTDIR)\Check.obj"
	-@erase "$(INTDIR)\CRegExp.obj"
	-@erase "$(INTDIR)\DirSrch.obj"
	-@erase "$(INTDIR)\Entity.obj"
	-@erase "$(INTDIR)\File.obj"
	-@erase "$(INTDIR)\FileRExp.obj"
	-@erase "$(INTDIR)\Handle.obj"
	-@erase "$(INTDIR)\IDirSrch.obj"
	-@erase "$(INTDIR)\INIFile.obj"
	-@erase "$(INTDIR)\IVIOAppl.obj"
	-@erase "$(INTDIR)\Parse.obj"
	-@erase "$(INTDIR)\PathDirSrch.obj"
	-@erase "$(INTDIR)\PathSrch.obj"
	-@erase "$(INTDIR)\RDirSrch.obj"
	-@erase "$(INTDIR)\RDirSrchSrv.obj"
	-@erase "$(INTDIR)\RemoteFile.obj"
	-@erase "$(INTDIR)\Socket.obj"
	-@erase "$(INTDIR)\StackTrc.obj"
	-@erase "$(INTDIR)\Thread.obj"
	-@erase "$(INTDIR)\Tokenize.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\Version.obj"
	-@erase "$(INTDIR)\XDirSrch.obj"
	-@erase "$(INTDIR)\XStrBuf.obj"
	-@erase "$(INTDIR)\Process.obj"
	-@erase "$(OUTDIR)\VCGeneral.dll"
	-@erase "$(OUTDIR)\VCGeneral.exp"
	-@erase "$(OUTDIR)\VCGeneral.ilk"
	-@erase "$(OUTDIR)\VCGeneral.lib"
	-@erase "$(OUTDIR)\VCGeneral.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "VCGENERAL_EXPORTS" /Fp"$(INTDIR)\VCGeneral.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /I.. /I../Windows /c

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\VCGeneral.bsc"
BSC32_SBRS= \

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\VCGeneral.pdb" /debug /machine:I386 /def:"$(OUTDIR)\VCGeneral.def" /out:"$(OUTDIR)\VCGeneral.dll" /implib:"$(OUTDIR)\VCGeneral.lib" /pdbtype:sept
DEF_FILE= \
        "$(OUTDIR)\VCGeneral.def"
LINK32_OBJS= \
	"$(INTDIR)\ADate.obj" \
	"$(INTDIR)\ANumeric.obj" \
	"$(INTDIR)\AssParse.obj" \
	"$(INTDIR)\ATime.obj" \
	"$(INTDIR)\ATStamp.obj" \
	"$(INTDIR)\AttrParse.obj" \
	"$(INTDIR)\Check.obj" \
	"$(INTDIR)\CRegExp.obj" \
	"$(INTDIR)\DirSrch.obj" \
	"$(INTDIR)\Entity.obj" \
	"$(INTDIR)\File.obj" \
	"$(INTDIR)\FileRExp.obj" \
	"$(INTDIR)\Handle.obj" \
	"$(INTDIR)\IDirSrch.obj" \
	"$(INTDIR)\INIFile.obj" \
	"$(INTDIR)\IVIOAppl.obj" \
	"$(INTDIR)\Parse.obj" \
	"$(INTDIR)\PathDirSrch.obj" \
	"$(INTDIR)\PathSrch.obj" \
	"$(INTDIR)\RDirSrch.obj" \
	"$(INTDIR)\RDirSrchSrv.obj" \
	"$(INTDIR)\RemoteFile.obj" \
	"$(INTDIR)\Socket.obj" \
	"$(INTDIR)\StackTrc.obj" \
	"$(INTDIR)\Thread.obj" \
	"$(INTDIR)\Tokenize.obj" \
	"$(INTDIR)\Version.obj" \
	"$(INTDIR)\XStrBuf.obj" \
        "$(INTDIR)\XDirSrch.obj" \
	"$(INTDIR)\Process.obj"

"$(OUTDIR)\VCGeneral.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF

!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("VCGeneral.dep")
!INCLUDE "VCGeneral.dep"
!ELSE
!MESSAGE Warning: cannot find "VCGeneral.dep"
!ENDIF
!ENDIF


!IF "$(CFG)" == "Release" || "$(CFG)" == "Debug"
SOURCE=..\YGP\ADate.cpp

"$(INTDIR)\ADate.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\YGP\ANumeric.cpp

"$(INTDIR)\ANumeric.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)

SOURCE=..\YGP\Process.cpp

"$(INTDIR)\Process.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\YGP\AssParse.cpp

"$(INTDIR)\AssParse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\YGP\ATime.cpp

"$(INTDIR)\ATime.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\YGP\ATStamp.cpp

"$(INTDIR)\ATStamp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\YGP\AttrParse.cpp

"$(INTDIR)\AttrParse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\YGP\Check.cpp

"$(INTDIR)\Check.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\YGP\CRegExp.cpp

"$(INTDIR)\CRegExp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\YGP\DirSrch.cpp

"$(INTDIR)\DirSrch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\YGP\XDirSrch.cpp

"$(INTDIR)\XDirSrch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\YGP\Entity.cpp

"$(INTDIR)\Entity.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\YGP\File.cpp

"$(INTDIR)\File.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\YGP\FileRExp.cpp

"$(INTDIR)\FileRExp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\YGP\Handle.cpp

"$(INTDIR)\Handle.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\YGP\IDirSrch.cpp

"$(INTDIR)\IDirSrch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\YGP\INIFile.cpp

"$(INTDIR)\INIFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\YGP\IVIOAppl.cpp

"$(INTDIR)\IVIOAppl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\YGP\Parse.cpp

"$(INTDIR)\Parse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\YGP\PathDirSrch.cpp

"$(INTDIR)\PathDirSrch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\YGP\PathSrch.cpp

"$(INTDIR)\PathSrch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\YGP\RDirSrch.cpp

"$(INTDIR)\RDirSrch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\YGP\RDirSrchSrv.cpp

"$(INTDIR)\RDirSrchSrv.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\YGP\RemoteFile.cpp

"$(INTDIR)\RemoteFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\YGP\Socket.cpp

"$(INTDIR)\Socket.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\YGP\StackTrc.cpp

"$(INTDIR)\StackTrc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\YGP\Thread.cpp

"$(INTDIR)\Thread.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\YGP\Tokenize.cpp

"$(INTDIR)\Tokenize.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\YGP\Version.cpp

"$(INTDIR)\Version.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\YGP\XStrBuf.cpp

"$(INTDIR)\XStrBuf.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF

VCGeneral.def: $(OUTDIR)\VCGeneral.def

$(OUTDIR)\VCGeneral.def: $(LINK32_OBJS)
	Echo LIBRARY VCGeneral > $(OUTDIR)\VCGeneral.def
	Echo EXPORTS >> $(OUTDIR)\VCGeneral.def
        if exist Dumpbin.out Del Dumpbin.out
	for %%i in ($(OUTDIR)\*.obj) do dumpbin /SYMBOLS %%i >> Dumpbin.out
        gawk -f ..\bin\mdef.awk Dumpbin.out >> $(OUTDIR)\VCGeneral.def
	Del Dumpbin.out

check: ALL
	nmake /f VCTests.mak check

install: ALL
        copy ..\YGP\*.h $(DEST_INCLUDE)
        copy *.h $(DEST_INCLUDE)
        copy $(OUTDIR)\*.lib $(DEST_LIB)
        copy $(OUTDIR)\*.dll $(DEST_DLL)
