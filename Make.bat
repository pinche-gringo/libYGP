Rem @Echo Off
cd Windows
If "%1" == "MakeDEF" Goto MakeDEF
Call NMake /f VCGeneral.mak %1 %2 %3 %4 %5 %6 %7 %8 %9
cd ..
Goto End

:MakeDEF
cd Debug
Del ..\VCGeneral.def
for %%i in (*.obj) do (dumpbin /SYMBOLS %%i | grep -E " External.*\(public:.+::[^ ]+\(.+\)\)")>>Temp.def
Echo LIBRARY VCGeneral > ..\VCGeneral.def
Echo EXPORTS >> ..\VCGeneral.def
awk -f ..\..\bin\mdef.awk Temp.def>>..\VCGeneral.def
Del Temp.def
cd ..\..

:End
