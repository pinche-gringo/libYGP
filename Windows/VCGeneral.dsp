# Microsoft Developer Studio Project File - Name="VCGeneral" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=VCGeneral - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE
!MESSAGE NMAKE /f "VCGeneral.mak".
!MESSAGE
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE
!MESSAGE NMAKE /f "VCGeneral.mak" CFG="VCGeneral - Win32 Debug"
!MESSAGE
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE
!MESSAGE "VCGeneral - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "VCGeneral - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "VCGeneral - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "VCGENERAL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../Common" /I "../Windows" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "VCGENERAL_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "VCGeneral - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "VCGeneral___Win32_Debug"
# PROP BASE Intermediate_Dir "VCGeneral___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "VCGENERAL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "VCGENERAL_EXPORTS" /YX /FD /GZ /I../Common /I../Windows /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept

!ENDIF

# Begin Target

# Name "VCGeneral - Win32 Release"
# Name "VCGeneral - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Common\AByteArray.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\ADate.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\ANumeric.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\Process.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\AssParse.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\ATime.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\ATStamp.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\AttrParse.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\Check.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\CRegExp.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\DirSrch.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\XDirSrch.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\Entity.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\File.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\FileRExp.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\Handle.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\IDirSrch.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\INIFile.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\IVIOAppl.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\Parse.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\PathDirSrch.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\PathSrch.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\RDirSrch.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\RDirSrchSrv.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\RemoteFile.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\Socket.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\StackTrc.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\Thread.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\Tokenize.cpp
# End Source File
# Begin Source File

SOURCE=.\VCGeneral.def
# End Source File
# Begin Source File

SOURCE=..\Common\Version.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\XStrBuf.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Common\AByteArray.h
# End Source File
# Begin Source File

SOURCE=..\Common\ADate.h
# End Source File
# Begin Source File

SOURCE=..\Common\ANumeric.h
# End Source File
# Begin Source File

SOURCE=..\Common\Process_.h
# End Source File
# Begin Source File

SOURCE=..\Common\AssParse.h
# End Source File
# Begin Source File

SOURCE=..\Common\ATime.h
# End Source File
# Begin Source File

SOURCE=..\Common\ATStamp.h
# End Source File
# Begin Source File

SOURCE=..\Common\Attribute.h
# End Source File
# Begin Source File

SOURCE=..\Common\AttrParse.h
# End Source File
# Begin Source File

SOURCE=..\Common\AttrVal.h
# End Source File
# Begin Source File

SOURCE=..\Common\Check.h
# End Source File
# Begin Source File

SOURCE=..\Common\CRegExp.h
# End Source File
# Begin Source File

SOURCE=..\Common\DirSrch.h
# End Source File
# Begin Source File

SOURCE=..\Common\XDirSrch.h
# End Source File
# Begin Source File

SOURCE=..\Common\Entity.h
# End Source File
# Begin Source File

SOURCE=..\Common\File.h
# End Source File
# Begin Source File

SOURCE=..\Common\FileRExp.h
# End Source File
# Begin Source File

SOURCE=".\gzo-cfg.h"
# End Source File
# Begin Source File

SOURCE=..\Common\Handle.h
# End Source File
# Begin Source File

SOURCE=..\Common\IDirSrch.h
# End Source File
# Begin Source File

SOURCE=..\Common\INIFile.h
# End Source File
# Begin Source File

SOURCE=..\Common\Internal.h
# End Source File
# Begin Source File

SOURCE=..\Common\IVIOAppl.h
# End Source File
# Begin Source File

SOURCE=..\Common\Log.h
# End Source File
# Begin Source File

SOURCE=..\Common\Mutex.h
# End Source File
# Begin Source File

SOURCE=..\Common\Parse.h
# End Source File
# Begin Source File

SOURCE=..\Common\PathDirSrch.h
# End Source File
# Begin Source File

SOURCE=..\Common\PathSrch.h
# End Source File
# Begin Source File

SOURCE=..\Common\RDirSrch.h
# End Source File
# Begin Source File

SOURCE=..\Common\RDirSrchSrv.h
# End Source File
# Begin Source File

SOURCE=..\Common\RegExp.h
# End Source File
# Begin Source File

SOURCE=..\Common\RemoteFile.h
# End Source File
# Begin Source File

SOURCE=..\Common\SmartPtr.h
# End Source File
# Begin Source File

SOURCE=..\Common\Socket.h
# End Source File
# Begin Source File

SOURCE=..\Common\StackTrc.h
# End Source File
# Begin Source File

SOURCE=..\Common\Thread.h
# End Source File
# Begin Source File

SOURCE=..\Common\Tokenize.h
# End Source File
# Begin Source File

SOURCE=..\Common\Trace_.h
# End Source File
# Begin Source File

SOURCE=..\Common\XStrBuf.h
# End Source File
# Begin Source File

SOURCE=..\Common\XStream.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
