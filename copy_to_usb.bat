@echo off

call build.bat

xcopy bin D:\ /E /Y
