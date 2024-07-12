REM Build script for TEN
@ECHO OFF
SetLocal enabledelayedexpansion

REM Get a list of all .c files
SET cFilenames=
FOR /R %%f in (*.c) do (
    SET cFilenames=!cFilenames! %%f
)

REM echo "Files:" %cFilenames%

SET assembly=ELe_MEM
SET compilerFlags=-g -shared -Wvarargs -Wall -Werror
REM -Wall -Error
SET includeFlags=-IHeader
SET linkerFlags=-luser32
SET defines=-D_DEBUG -DELe_EXPORT -D_CRT_SECURE_NO_WARNINGS

ECHO "Building %assembly%%..."
clang %cFilenames% %compilerFlags% -o ../bin/%assembly%.dll %defines% %includeFlags% %linkerFlags%