@Echo Off
if "%1" == "MSC" (Set MAKE=NMake.exe&set MAKEFILE=/f VCGeneral.mak&shift&goto go)
if "%1" == "BCC" (Set MAKE=Make.exe&set MAKEFILE=-f BCGeneral.mak&shift&goto go)

echo Batch to compile the General DLL
echo.
echo Usage: %0 MSC^|BCC
goto end

:go
cd Windows
Call %MAKE% %MAKEFILE% %1 %2 %3 %4 %5 %6 %7 %8 %9
cd ..

:end
