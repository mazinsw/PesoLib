@echo OFF 
SET version="1.2.0.0"
SET filename="%~d0%~p0bin\releases\pesolib-%version%.zip"
del %filename%

"%programfiles%\7-zip\7z" -tzip a %filename% *.txt -xr!.svn
"%programfiles%\7-zip\7z" -tzip a %filename% bin -xr!.svn -xr!releases
"%programfiles%\7-zip\7z" -tzip a %filename% build -xr!.svn -xr!*.layout
"%programfiles%\7-zip\7z" -tzip a %filename% src -xr!.svn -xr!*.o
"%programfiles%\7-zip\7z" -tzip a %filename% test -xr!.svn -xr!*.exe -xr!*.layout -xr!*.o
"%programfiles%\7-zip\7z" -tzip a %filename% res -xr!.svn
"%programfiles%\7-zip\7z" -tzip a %filename% docs -xr!.svn
"%programfiles%\7-zip\7z" -tzip a %filename% include -xr!.svn
"%programfiles%\7-zip\7z" -tzip a %filename% lib -xr!.svn
"%programfiles%\7-zip\7z" -tzip a %filename% wrapper -xr!.svn -xr!.metadata -xr!bin -xr!*.exe -xr!*.dcu
