Rem @Echo Off
cd Windows
If "%1" == "MakeDEF" Goto MakeDEF
Call NMake /f VCGeneral.mak %1 %2 %3 %4 %5 %6 %7 %8 %9
cd ..
Goto End

:MakeDEF
cd Debug
Echo Library General > ..\VCGeneral.def
Echo Exports >> ..\VCGeneral.def
if "%@eval[2+2]%" == "4" goto 4NT
for %%i in (*.obj) do dumpbin /SYMBOLS %%i | grep -E " External.*(.*%%~ni.+)" >> ..\VCGeneral.def
goto EndDEF

:4NT
for %%i in (*.obj) do (dumpbin /SYMBOLS %i | grep -E " External.*(.*%@NAME[%i].+)") >> ..\VCGeneral.def

:EndDEF
cd ..\..

:End
