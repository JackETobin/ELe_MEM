@ECHO OFF
REM Build Everything

ECHO "Building all..."

PUSHD ELe_MEM
CALL build.bat
POPD
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

PUSHD ELe_TEST
CALL build.bat
POPD
IF %ERRORLEVEL% NEQ 0 (echo %ERRORLEVEL% && exit)

ECHO "All assemblies built successfully."