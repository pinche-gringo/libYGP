# Microsoft Developer Studio Generated NMAKE File, Based on VCTests.dsp
!IF "$(CFG)" == ""
CFG=Debug
!MESSAGE Keine Konfiguration angegeben. Debug wird als Standard verwendet.
!ENDIF

!IF "$(CFG)" != "Release" && "$(CFG)" != "Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE
!MESSAGE NMAKE /f "VCTests.mak" CFG="Debug"
!MESSAGE
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE
!MESSAGE "Release"
!MESSAGE "Debug"
!MESSAGE
!ERROR Eine ungÅltige Konfiguration wurde angegeben.
!ENDIF

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF

CPP=cl.exe
LINK32=link.exe

EXTRAINCLUDES=/I "." /I ".." /I "..\YGP\Tests"
LIBOBJDIR=$(CFG)

!IF  "$(CFG)" == "Release"

OUTDIR=.\Release\Tests
INTDIR=.\Release\Tests
# Begin Custom Macros
# End Custom Macros

CPP_PROJ=/nologo /ML /W3 /GX /O2 $(EXTRAINCLUDES) /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /machine:I386 /NODEFAULTLIB:libcmt

!ELSEIF  "$(CFG)" == "Debug"

LIBDIR=.\Debug
OUTDIR=.\Debug\Tests
INTDIR=.\Debug\Tests
# Begin Custom Macros
# End Custom Macros

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od $(EXTRAINCLUDES) /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c

LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /debug /machine:I386 /pdbtype:sept  /NODEFAULTLIB:LIBCMTD

!ENDIF

TESTS = "$(OUTDIR)\ADate.exe" "$(OUTDIR)\ANumeric.exe"  "$(OUTDIR)\AssParse.exe" \
        "$(OUTDIR)\ATime.exe" "$(OUTDIR)\ATStamp.exe" "$(OUTDIR)\Attribute.exe" \
        "$(OUTDIR)\AttrParse.exe" "$(OUTDIR)\CRegExp.exe" "$(OUTDIR)\DirSrch.exe" \
        "$(OUTDIR)\FileRExp.exe" "$(OUTDIR)\Handle.exe" "$(OUTDIR)\INIFile.exe" \
        "$(OUTDIR)\IVIOAppl.exe" "$(OUTDIR)\Parse.exe" "$(OUTDIR)\XStrBuf.exe" \
        "$(OUTDIR)\PathDirSrch.exe" "$(OUTDIR)\PathSrch.exe" "$(OUTDIR)\Tokenize.exe" \
        "$(OUTDIR)\XStream.exe"

ALL : $(OUTDIR) $(TESTS)

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

!IF  "$(CFG)" == "Release"

CLEAN : cleancommon

!ELSEIF  "$(CFG)" == "Debug"

CLEAN : cleancommon
	-@erase "$(INTDIR)\*.pdb"
	-@erase "$(OUTDIR)\*.ilk"

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
!IF EXISTS("VCTests.dep")
!INCLUDE "VCTests.dep"
!ELSE
!MESSAGE Warning: cannot find "VCTests.dep"
!ENDIF
!ENDIF


!IF "$(CFG)" == "Release" || "$(CFG)" == "Debug"
FILE=ADate

"$(INTDIR)\$(FILE).obj" : ..\YGP\Tests\$(FILE).cpp "$(INTDIR)"
        $(CPP) $(CPP_PROJ) ..\YGP\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(LIBOBJDIR)\VCGenerals.lib
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(LIBOBJDIR)\VCGenerals.lib /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=ANumeric

"$(INTDIR)\$(FILE).obj" : ..\YGP\Tests\$(FILE).cpp "$(INTDIR)"
        $(CPP) $(CPP_PROJ) ..\YGP\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(LIBOBJDIR)\VCGenerals.lib
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(LIBOBJDIR)\VCGenerals.lib /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=AssParse

"$(INTDIR)\$(FILE).obj" : ..\YGP\Tests\$(FILE).cpp "$(INTDIR)"
        $(CPP) $(CPP_PROJ) ..\YGP\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(LIBOBJDIR)\VCGenerals.lib
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(LIBOBJDIR)\VCGenerals.lib /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=ATime

"$(INTDIR)\$(FILE).obj" : ..\YGP\Tests\$(FILE).cpp "$(INTDIR)"
        $(CPP) $(CPP_PROJ) ..\YGP\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(LIBOBJDIR)\VCGenerals.lib
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(LIBOBJDIR)\VCGenerals.lib /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=ATStamp

"$(INTDIR)\$(FILE).obj" : ..\YGP\Tests\$(FILE).cpp "$(INTDIR)"
        $(CPP) $(CPP_PROJ) ..\YGP\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(LIBOBJDIR)\VCGenerals.lib
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(LIBOBJDIR)\VCGenerals.lib /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=Attribute

"$(INTDIR)\$(FILE).obj" : ..\YGP\Tests\$(FILE).cpp "$(INTDIR)"
        $(CPP) $(CPP_PROJ) ..\YGP\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(LIBOBJDIR)\VCGenerals.lib
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(LIBOBJDIR)\VCGenerals.lib /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=AttrParse

"$(INTDIR)\$(FILE).obj" : ..\YGP\Tests\$(FILE).cpp "$(INTDIR)"
        $(CPP) $(CPP_PROJ) ..\YGP\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(LIBOBJDIR)\VCGenerals.lib
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(LIBOBJDIR)\VCGenerals.lib /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=CRegExp

"$(INTDIR)\$(FILE).obj" : ..\YGP\Tests\$(FILE).cpp "$(INTDIR)"
        $(CPP) $(CPP_PROJ) ..\YGP\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(LIBOBJDIR)\VCGenerals.lib
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(LIBOBJDIR)\VCGenerals.lib /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=DirSrch

"$(INTDIR)\$(FILE).obj" : ..\YGP\Tests\$(FILE).cpp "$(INTDIR)"
        $(CPP) $(CPP_PROJ) ..\YGP\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(LIBOBJDIR)\VCGenerals.lib
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(LIBOBJDIR)\VCGenerals.lib /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=FileRExp

"$(INTDIR)\$(FILE).obj" : ..\YGP\Tests\$(FILE).cpp "$(INTDIR)"
        $(CPP) $(CPP_PROJ) ..\YGP\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(LIBOBJDIR)\VCGenerals.lib
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(LIBOBJDIR)\VCGenerals.lib /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=Handle

"$(INTDIR)\$(FILE).obj" : ..\YGP\Tests\$(FILE).cpp "$(INTDIR)"
        $(CPP) $(CPP_PROJ) ..\YGP\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(LIBOBJDIR)\VCGenerals.lib
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(LIBOBJDIR)\VCGenerals.lib /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=INIFile

"$(INTDIR)\$(FILE).obj" : ..\YGP\Tests\$(FILE).cpp "$(INTDIR)"
        $(CPP) $(CPP_PROJ) ..\YGP\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(LIBOBJDIR)\VCGenerals.lib
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(LIBOBJDIR)\VCGenerals.lib /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=IVIOAppl

"$(INTDIR)\$(FILE).obj" : ..\YGP\Tests\$(FILE).cpp "$(INTDIR)"
        $(CPP) $(CPP_PROJ) ..\YGP\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(LIBOBJDIR)\VCGenerals.lib
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(LIBOBJDIR)\VCGenerals.lib /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=Parse

"$(INTDIR)\$(FILE).obj" : ..\YGP\Tests\$(FILE).cpp "$(INTDIR)"
        $(CPP) $(CPP_PROJ) ..\YGP\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(LIBOBJDIR)\VCGenerals.lib
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(LIBOBJDIR)\VCGenerals.lib /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=XStrBuf

"$(INTDIR)\$(FILE).obj" : ..\YGP\Tests\$(FILE).cpp "$(INTDIR)"
        $(CPP) $(CPP_PROJ) ..\YGP\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(LIBOBJDIR)\VCGenerals.lib
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(LIBOBJDIR)\VCGenerals.lib /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=PathDirSrch

"$(INTDIR)\$(FILE).obj" : ..\YGP\Tests\$(FILE).cpp "$(INTDIR)"
        $(CPP) $(CPP_PROJ) ..\YGP\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(LIBOBJDIR)\VCGenerals.lib
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(LIBOBJDIR)\VCGenerals.lib /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=PathSrch

"$(INTDIR)\$(FILE).obj" : ..\YGP\Tests\$(FILE).cpp "$(INTDIR)"
        $(CPP) $(CPP_PROJ) ..\YGP\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(LIBOBJDIR)\VCGenerals.lib
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(LIBOBJDIR)\VCGenerals.lib /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=Tokenize

"$(INTDIR)\$(FILE).obj" : ..\YGP\Tests\$(FILE).cpp "$(INTDIR)"
        $(CPP) $(CPP_PROJ) ..\YGP\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(LIBOBJDIR)\VCGenerals.lib
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(LIBOBJDIR)\VCGenerals.lib /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=XStream

"$(INTDIR)\$(FILE).obj" : ..\YGP\Tests\$(FILE).cpp "$(INTDIR)"
        $(CPP) $(CPP_PROJ) ..\YGP\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(LIBOBJDIR)\VCGenerals.lib
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(LIBOBJDIR)\VCGenerals.lib /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<


!ENDIF

cleanYGP:
	-@erase "$(INTDIR)\*.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\*.exe"

check: ALL
	for %i in ( $(TESTS) ) do %%i
