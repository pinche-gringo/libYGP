# Microsoft Developer Studio Generated NMAKE File, Based on VCGeneral.dsp
!IF "$(CFG)" == ""
CFG=General - Win32 Debug
!MESSAGE Keine Konfiguration angegeben. General - Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "General - Win32 Release" && "$(CFG)" != "General - Win32 Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "VCGeneral.mak" CFG="General - Win32 Debug"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "General - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "General - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR Eine ungÅltige Konfiguration wurde angegeben.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "General - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\VCGeneral.dll"


CLEAN :
	-@erase "$(INTDIR)\ADate.obj"
	-@erase "$(INTDIR)\ANumeric.obj"
	-@erase "$(INTDIR)\ATime.obj"
	-@erase "$(INTDIR)\ATStamp.obj"
	-@erase "$(INTDIR)\DirSrch.obj"
	-@erase "$(INTDIR)\FileRExp.obj"
	-@erase "$(INTDIR)\Handle.obj"
	-@erase "$(INTDIR)\IVIOAppl.obj"
	-@erase "$(INTDIR)\Parse.obj"
	-@erase "$(INTDIR)\PathDirSrch.obj"
	-@erase "$(INTDIR)\StackTrc.obj"
	-@erase "$(INTDIR)\Tokenize.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\Version.obj"
	-@erase "$(INTDIR)\XStrBuf.obj"
	-@erase "$(OUTDIR)\VCGeneral.dll"
	-@erase "$(OUTDIR)\VCGeneral.exp"
	-@erase "$(OUTDIR)\VCGeneral.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /Za /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GENERAL_EXPORTS" /D "WINDOWS" /D PACKAGE=\"General\" /D VERSION=\"0.6\" /D MICRO_VERSION=\"00\" /Fp"$(INTDIR)\VCGeneral.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\VCGeneral.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\VCGeneral.pdb" /machine:I386 /out:"$(OUTDIR)\VCGeneral.dll" /implib:"$(OUTDIR)\VCGeneral.lib" 
LINK32_OBJS= \
	"$(INTDIR)\ADate.obj" \
	"$(INTDIR)\ANumeric.obj" \
	"$(INTDIR)\ATime.obj" \
	"$(INTDIR)\ATStamp.obj" \
	"$(INTDIR)\DirSrch.obj" \
	"$(INTDIR)\FileRExp.obj" \
	"$(INTDIR)\Handle.obj" \
	"$(INTDIR)\IVIOAppl.obj" \
	"$(INTDIR)\Parse.obj" \
	"$(INTDIR)\PathDirSrch.obj" \
	"$(INTDIR)\Tokenize.obj" \
	"$(INTDIR)\Version.obj" \
	"$(INTDIR)\XStrBuf.obj" \
	"$(INTDIR)\StackTrc.obj"

"$(OUTDIR)\VCGeneral.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "General - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\VCGeneral.dll"


CLEAN :
	-@erase "$(INTDIR)\ADate.obj"
	-@erase "$(INTDIR)\ANumeric.obj"
	-@erase "$(INTDIR)\ATime.obj"
	-@erase "$(INTDIR)\ATStamp.obj"
	-@erase "$(INTDIR)\DirSrch.obj"
	-@erase "$(INTDIR)\FileRExp.obj"
	-@erase "$(INTDIR)\Handle.obj"
	-@erase "$(INTDIR)\IVIOAppl.obj"
	-@erase "$(INTDIR)\Parse.obj"
	-@erase "$(INTDIR)\PathDirSrch.obj"
	-@erase "$(INTDIR)\StackTrc.obj"
	-@erase "$(INTDIR)\Tokenize.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\Version.obj"
	-@erase "$(INTDIR)\XStrBuf.obj"
	-@erase "$(OUTDIR)\VCGeneral.dll"
	-@erase "$(OUTDIR)\VCGeneral.exp"
	-@erase "$(OUTDIR)\VCGeneral.ilk"
	-@erase "$(OUTDIR)\VCGeneral.lib"
	-@erase "$(OUTDIR)\VCGeneral.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /Za /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GENERAL_EXPORTS" /D "WINDOWS" /D PACKAGE=\"General\" /D VERSION=\"0.6\" /D MICRO_VERSION=\"00\" /Fp"$(INTDIR)\VCGeneral.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\VCGeneral.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\VCGeneral.pdb" /debug /machine:I386 /out:"$(OUTDIR)\VCGeneral.dll" /implib:"$(OUTDIR)\VCGeneral.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\ADate.obj" \
	"$(INTDIR)\ANumeric.obj" \
	"$(INTDIR)\ATime.obj" \
	"$(INTDIR)\ATStamp.obj" \
	"$(INTDIR)\DirSrch.obj" \
	"$(INTDIR)\FileRExp.obj" \
	"$(INTDIR)\Handle.obj" \
	"$(INTDIR)\IVIOAppl.obj" \
	"$(INTDIR)\Parse.obj" \
	"$(INTDIR)\PathDirSrch.obj" \
	"$(INTDIR)\Tokenize.obj" \
	"$(INTDIR)\Version.obj" \
	"$(INTDIR)\XStrBuf.obj" \
	"$(INTDIR)\StackTrc.obj"

"$(OUTDIR)\VCGeneral.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

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


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("VCGeneral.dep")
!INCLUDE "VCGeneral.dep"
!ELSE 
!MESSAGE Warning: cannot find "VCGeneral.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "General - Win32 Release" || "$(CFG)" == "General - Win32 Debug"
SOURCE=..\Common\ADate.cpp

"$(INTDIR)\ADate.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Common\ANumeric.cpp

"$(INTDIR)\ANumeric.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Common\ATime.cpp

"$(INTDIR)\ATime.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Common\ATStamp.cpp

"$(INTDIR)\ATStamp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Common\DirSrch.cpp

"$(INTDIR)\DirSrch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Common\FileRExp.cpp

"$(INTDIR)\FileRExp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Common\Handle.cpp

"$(INTDIR)\Handle.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Common\IVIOAppl.cpp

"$(INTDIR)\IVIOAppl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Common\Parse.cpp

"$(INTDIR)\Parse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Common\PathDirSrch.cpp

"$(INTDIR)\PathDirSrch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Common\StackTrc.cpp

"$(INTDIR)\StackTrc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Common\Tokenize.cpp

"$(INTDIR)\Tokenize.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Common\Version.cpp

"$(INTDIR)\Version.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Common\XStrBuf.cpp

"$(INTDIR)\XStrBuf.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

