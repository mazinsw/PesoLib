@echo off
cd PesoLib
javah -jni -d ..\..\..\src\java -classpath bin br.com.mzsw.PesoLibWrapper
if not %ERRORLEVEL% == 0 pause > NUL
echo Ok