# Microsoft Developer Studio Generated NMAKE File, Based on Test.dsp
!IF "$(CFG)" == ""
CFG=Test - Win32 Debug
!MESSAGE Keine Konfiguration angegeben. Test - Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "Test - Win32 Release" && "$(CFG)" != "Test - Win32 Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "Test.mak" CFG="Test - Win32 Debug"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "Test - Win32 Release" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE "Test - Win32 Debug" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE 
!ERROR Eine ungÅltige Konfiguration wurde angegeben.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Test - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Test.exe"

!ELSE 

ALL : "General - Win32 Release" "$(OUTDIR)\Test.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"General - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\Test.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\Test.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "WINDOWS" /D PACKAGE=\"General\" /D VERSION=\"0.3\" /D MICRO_VERSION=\"00\" /Fp"$(INTDIR)\Test.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Test.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=Debug\General.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\Test.pdb" /machine:I386 /out:"$(OUTDIR)\Test.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Test.obj" \
	"$(OUTDIR)\General.lib"

"$(OUTDIR)\Test.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Test - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Test.exe"

!ELSE 

ALL : "General - Win32 Debug" "$(OUTDIR)\Test.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"General - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\Test.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Test.exe"
	-@erase "$(OUTDIR)\Test.ilk"
	-@erase "$(OUTDIR)\Test.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "WINDOWS" /D PACKAGE=\"General\" /D VERSION=\"0.3\" /D MICRO_VERSION=\"00\" /Fp"$(INTDIR)\Test.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Test.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=Debug\General.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\Test.pdb" /debug /machine:I386 /out:"$(OUTDIR)\Test.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\Test.obj" \
	"$(OUTDIR)\General.lib"

"$(OUTDIR)\Test.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("Test.dep")
!INCLUDE "Test.dep"
!ELSE 
!MESSAGE Warning: cannot find "Test.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Test - Win32 Release" || "$(CFG)" == "Test - Win32 Debug"
SOURCE=..\Common\Test.cpp

"$(INTDIR)\Test.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!IF  "$(CFG)" == "Test - Win32 Release"

"General - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\General.mak CFG="General - Win32 Release" 
   cd "."

"General - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\General.mak CFG="General - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "Test - Win32 Debug"

"General - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\General.mak CFG="General - Win32 Debug" 
   cd "."

"General - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\General.mak CFG="General - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 


!ENDIF 

