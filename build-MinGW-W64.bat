@echo off
setlocal
cd /d "%~dp0"
set "CONFIG=%~1"
if "%CONFIG%"=="" set "CONFIG=debug_x64"
pushd build
premake5.exe gmake
if errorlevel 1 (popd & exit /b 1)
popd
mingw32-make config=%CONFIG%
if errorlevel 1 exit /b 1
exit /b 0
