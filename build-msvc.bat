@echo OFF
set SUFFIX="-msvc"

rem Debug / Release
ECHO ### Create MSVC Project ###
cmake -S . -B build%SUFFIX% -G "Visual Studio 17 2022" -A x64 || GOTO :PRINT_ERROR

ECHO ### Build ###
cmake --build build%SUFFIX% --config Release || GOTO :PRINT_ERROR

ECHO ### Install ###
cmake --install build%SUFFIX% --config Release --prefix install%SUFFIX% || GOTO :PRINT_ERROR

ECHO ### Success ###
GOTO :EOF

:PRINT_ERROR
ECHO Failed with error #%errorlevel%
EXIT /b %errorlevel%