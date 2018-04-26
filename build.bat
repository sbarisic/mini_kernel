@echo off

rmdir bin\boot /S /Q
mkdir bin\boot
xcopy boot bin\boot /E
bin\fileproc.exe

rem rmdir bin\data /S /Q 2>nul

del bin\*.pdb 2>nul
del bin\*.exp 2>nul
del bin\*.lib 2>nul
del bin\*.exe.config 2>nul
del bin\*.map 2>nul