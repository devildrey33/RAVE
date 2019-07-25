@echo off
set Directori=%~dp0
set Directori=%Directori:\Bats Admin\=%

copy "%Directori%\Build\CrearMD5\x86\Release\CrearMD5x86.exe" "%Directori%\Instaladores\CrearMD5x86.exe"
copy "%Directori%\Build\RAVE\x86\Release\RAVE.exe" "%PROGRAMFILES(X86)%\Rave\Rave.exe"
copy "%Directori%\Build\RAVE\x86\Release\RAVE.exe" "%Directori%\Release-x86\RAVE.exe"
pause
