@echo off
set Directori=%~dp0
set Directori=%Directori:\Bats Admin\=%
copy "%Directori%\Build\CrearMD5\x64\Release\CrearMD5x64.exe" "%Directori%\Instaladores\CrearMD5x64.exe"
copy "%Directori%\Build\RAVE\x64\Release\RAVE.exe" "%ProgramW6432%\Rave\RAVE.exe"
copy "%Directori%\Build\RAVE\x64\Release\RAVE.exe" "%Directori%\Release-x64\RAVE.exe"
pause
