@echo off

rmdir bin\boot /S /Q
mkdir bin\boot
xcopy boot bin\boot /E
bin\fileproc.exe