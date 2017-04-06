@Echo off
make.exe
if errorlevel 1 (
   echo.
   echo Error while executing make. See the error details above.
   echo.
   echo General Hints to setup make on Windows
   echo ======================================
   echo - Download and install https://launchpad.net/gcc-arm-embedded
   echo - Download and install MinGW http://www.mingw.org/
   echo - Check in system advanced properties if the PATH variable 
   echo   includes the MinGW binary path
   echo - Create a new system environment variable and name it ARMGCC_DIR.
   echo   The value of this variable should point to the ARM GCC Embedded
   echo   toolchain installation path
   echo.
   pause
) ELSE (
   echo.
   echo Make successful.
   pause
)