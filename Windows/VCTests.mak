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
RSC=rc.exe

EXTRAINCLUDES=/I "." /I "..\Common" /I "..\Common\Tests"
LIBOBJDIR=$(CFG)

!IF  "$(CFG)" == "Release"

OUTDIR=.\Release\Tests
INTDIR=.\Release\Tests
# Begin Custom Macros
# End Custom Macros

CPP_PROJ=/nologo /ML /W3 /GX /O2 $(EXTRAINCLUDES) /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /machine:I386 /NODEFAULTLIB:LIBCMTD

!ELSEIF  "$(CFG)" == "Debug"

OUTDIR=.\Debug\Tests
INTDIR=.\Debug\Tests
# Begin Custom Macros
# End Custom Macros

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od $(EXTRAINCLUDES) /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\VCTests.bsc"
BSC32_SBRS= \

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /debug /machine:I386 /pdbtype:sept  /NODEFAULTLIB:LIBCMTD

!ENDIF

TESTS = "$(OUTDIR)\AByteArray.exe" "$(OUTDIR)\ADate.exe" "$(OUTDIR)\ANumeric.exe"  \
	"$(OUTDIR)\AssParse.exe" "$(OUTDIR)\ATime.exe" "$(OUTDIR)\ATStamp.exe" \
	"$(OUTDIR)\Attribute.exe" "$(OUTDIR)\AttrParse.exe" "$(OUTDIR)\CRegExp.exe" \
	"$(OUTDIR)\DirSrch.exe" "$(OUTDIR)\FileRExp.exe" "$(OUTDIR)\Handle.exe" \
	"$(OUTDIR)\INIFile.exe" "$(OUTDIR)\IVIOAppl.exe" "$(OUTDIR)\Parse.exe" \
	"$(OUTDIR)\XStrBuf.exe" "$(OUTDIR)\PathDirSrch.exe" "$(OUTDIR)\PathSrch.exe" \
	"$(OUTDIR)\Tokenize.exe" "$(OUTDIR)\XStream.exe"

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
FILE=AByteArray
DEP_OBJS="$(LIBOBJDIR)\$(FILE).obj"

"$(INTDIR)\$(FILE).obj" : ..\Common\Tests\$(FILE).cpp
	$(CPP) $(CPP_PROJ) ..\Common\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(DEP_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(DEP_OBJS) /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=ADate
DEP_OBJS="$(LIBOBJDIR)\$(FILE).obj"

"$(INTDIR)\$(FILE).obj" : ..\Common\Tests\$(FILE).cpp "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ..\Common\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(DEP_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(DEP_OBJS) /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=ANumeric
DEP_OBJS="$(LIBOBJDIR)\$(FILE).obj"

"$(INTDIR)\$(FILE).obj" : ..\Common\Tests\$(FILE).cpp "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ..\Common\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(DEP_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(DEP_OBJS) /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=AssParse
DEP_OBJS="$(LIBOBJDIR)\$(FILE).obj" "$(LIBOBJDIR)\Tokenize.obj"

"$(INTDIR)\$(FILE).obj" : ..\Common\Tests\$(FILE).cpp "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ..\Common\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(DEP_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(DEP_OBJS) /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=ATime
DEP_OBJS="$(LIBOBJDIR)\$(FILE).obj"

"$(INTDIR)\$(FILE).obj" : ..\Common\Tests\$(FILE).cpp "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ..\Common\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(DEP_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(DEP_OBJS) /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=ATStamp
DEP_OBJS="$(LIBOBJDIR)\$(FILE).obj" "$(LIBOBJDIR)\ATime.obj" "$(LIBOBJDIR)\ADate.obj"

"$(INTDIR)\$(FILE).obj" : ..\Common\Tests\$(FILE).cpp "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ..\Common\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(DEP_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(DEP_OBJS) /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=Attribute
DEP_OBJS="$(LIBOBJDIR)\ATime.obj"

"$(INTDIR)\$(FILE).obj" : ..\Common\Tests\$(FILE).cpp "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ..\Common\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(DEP_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(DEP_OBJS) /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=AttrParse
DEP_OBJS="$(LIBOBJDIR)\$(FILE).obj" $(LIBOBJDIR)\ATime.obj $(LIBOBJDIR)\ATStamp.obj \
	 $(LIBOBJDIR)\ANumeric.obj $(LIBOBJDIR)\ADate.obj $(LIBOBJDIR)\AssParse.obj \
	 $(LIBOBJDIR)\Tokenize.obj

"$(INTDIR)\$(FILE).obj" : ..\Common\Tests\$(FILE).cpp "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ..\Common\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(DEP_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(DEP_OBJS) /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=CRegExp
DEP_OBJS="$(LIBOBJDIR)\$(FILE).obj" $(LIBOBJDIR)\ANumeric.obj $(LIBOBJDIR)\Parse.obj \
	 $(LIBOBJDIR)\XStrBuf.obj

"$(INTDIR)\$(FILE).obj" : ..\Common\Tests\$(FILE).cpp "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ..\Common\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(DEP_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(DEP_OBJS) /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=DirSrch
DEP_OBJS="$(LIBOBJDIR)\$(FILE).obj" $(LIBOBJDIR)\File.obj $(LIBOBJDIR)\IDirSrch.obj \
	 $(LIBOBJDIR)\FileRExp.obj $(LIBOBJDIR)\ANumeric.obj

"$(INTDIR)\$(FILE).obj" : ..\Common\Tests\$(FILE).cpp "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ..\Common\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(DEP_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(DEP_OBJS) /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=FileRExp
DEP_OBJS="$(LIBOBJDIR)\$(FILE).obj" $(LIBOBJDIR)\ANumeric.obj

"$(INTDIR)\$(FILE).obj" : ..\Common\Tests\$(FILE).cpp "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ..\Common\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(DEP_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(DEP_OBJS) /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=Handle
DEP_OBJS="$(LIBOBJDIR)\$(FILE).obj"

"$(INTDIR)\$(FILE).obj" : ..\Common\Tests\$(FILE).cpp "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ..\Common\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(DEP_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(DEP_OBJS) /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=INIFile
DEP_OBJS="$(LIBOBJDIR)\$(FILE).obj" "$(LIBOBJDIR)\Parse.obj" "$(LIBOBJDIR)\XStrBuf.obj" \
	 "$(LIBOBJDIR)\ANumeric.obj" "$(LIBOBJDIR)\ATStamp.obj" "$(LIBOBJDIR)\ADate.obj" \
	 "$(LIBOBJDIR)\ATime.obj"

"$(INTDIR)\$(FILE).obj" : ..\Common\Tests\$(FILE).cpp "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ..\Common\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(DEP_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(DEP_OBJS) /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=IVIOAppl
DEP_OBJS="$(LIBOBJDIR)\$(FILE).obj" "$(LIBOBJDIR)\StackTrc.obj" "$(LIBOBJDIR)\File.obj" \
	  "$(LIBOBJDIR)\PathSrch.obj"

"$(INTDIR)\$(FILE).obj" : ..\Common\Tests\$(FILE).cpp "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ..\Common\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(DEP_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(DEP_OBJS) /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=Parse
DEP_OBJS="$(LIBOBJDIR)\$(FILE).obj" "$(LIBOBJDIR)\IVIOAppl.obj" "$(LIBOBJDIR)\XStrBuf.obj" \
	 "$(LIBOBJDIR)\StackTrc.obj" "$(LIBOBJDIR)\File.obj" "$(LIBOBJDIR)\PathSrch.obj"

"$(INTDIR)\$(FILE).obj" : ..\Common\Tests\$(FILE).cpp "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ..\Common\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(DEP_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(DEP_OBJS) /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=XStrBuf
DEP_OBJS="$(LIBOBJDIR)\$(FILE).obj"

"$(INTDIR)\$(FILE).obj" : ..\Common\Tests\$(FILE).cpp "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ..\Common\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(DEP_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(DEP_OBJS) /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=PathDirSrch
DEP_OBJS="$(LIBOBJDIR)\$(FILE).obj" $(LIBOBJDIR)\File.obj $(LIBOBJDIR)\IDirSrch.obj \
	 $(LIBOBJDIR)\FileRExp.obj $(LIBOBJDIR)\ANumeric.obj $(LIBOBJDIR)\DirSrch.obj \
	 $(LIBOBJDIR)\PathSrch.obj $(LIBOBJDIR)\Tokenize.obj

"$(INTDIR)\$(FILE).obj" : ..\Common\Tests\$(FILE).cpp "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ..\Common\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(DEP_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(DEP_OBJS) /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=PathSrch
DEP_OBJS="$(LIBOBJDIR)\$(FILE).obj" $(LIBOBJDIR)\Tokenize.obj $(LIBOBJDIR)\File.obj

"$(INTDIR)\$(FILE).obj" : ..\Common\Tests\$(FILE).cpp "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ..\Common\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(DEP_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(DEP_OBJS) /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=Tokenize
DEP_OBJS="$(LIBOBJDIR)\$(FILE).obj"

"$(INTDIR)\$(FILE).obj" : ..\Common\Tests\$(FILE).cpp "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ..\Common\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(DEP_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(DEP_OBJS) /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<

FILE=XStream
DEP_OBJS=$(LIBOBJDIR)\XStrBuf.obj

"$(INTDIR)\$(FILE).obj" : ..\Common\Tests\$(FILE).cpp "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ..\Common\Tests\$(FILE).cpp

"$(OUTDIR)\$(FILE).exe" : "$(OUTDIR)\$(FILE).obj" $(DEF_FILE) $(DEP_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(INTDIR)\$(FILE).obj $(DEP_OBJS) /pdb:"$(OUTDIR)\$(FILE).pdb" /out:"$(OUTDIR)\$(FILE).exe"
<<


!ENDIF

cleancommon:
	-@erase "$(INTDIR)\*.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\*.exe"

check: ALL
	for %i in ( $(TESTS) ) do %%i
