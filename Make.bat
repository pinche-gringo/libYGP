@Echo Off
if "%1" == "-h" goto help

cd Windows
Call NMAKE /f VCGeneral.mak %1 %2 %3 %4 %5 %6 %7 %8 %9
cd ..
goto end

:help
echo Batch to compile the General DLL
echo.
echo Usage: %0 [CFG=Release] [install^|check]

:end