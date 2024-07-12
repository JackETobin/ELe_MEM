REM Build script for Sandbox
@ECHO OFF
SetLocal enabledelayedexpansion

REM Get a list of all .c files
SET cFilenames=
FOR /R %%f in (*.c) do (
    SET cFilenames=!cFilenames! %%f
)

REM echo "Files:" %cFilenames%

SET assembly=ELe_TEST
SET compilerFlags=-g
REM -Wall -Error
SET includeFlags=-Isrc -I../ELe_MEM/Header/
SET linkerFlags=-L../bin/ -lELe_MEM.lib
SET defines=-D_DEBUG -DTEN_EXPORT

ECHO "Building %assembly%%..."
clang %cFilenames% %compilerFlags% -o ../bin/%assembly%.exe %defines% %includeFlags% %linkerFlags%