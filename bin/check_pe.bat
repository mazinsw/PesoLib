@echo off

ECHO Verificando arquivos de 64 bits
petype.pl x64\PesoLib.exe
petype.pl x64\PesoLib.dll
petype.pl x64\TesteBalanca.exe
ECHO.
ECHO Verificando arquivos de 32 bits
petype.pl x86\PesoLib.exe
petype.pl x86\PesoLib.dll
petype.pl x86\TesteBalanca.exe
pause > NUL