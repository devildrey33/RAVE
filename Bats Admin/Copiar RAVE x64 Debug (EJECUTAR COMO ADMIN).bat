@echo off
set Directori=%~dp0
set Directori=%Directori:\Bats Admin\=%

copy "%Directori%\Build\CrearMD5\x64\Debug\CrearMD5x64.exe" "%Directori%\Instaladores\CrearMD5x64.exe"
copy "%Directori%\Build\RAVE\x64\Debug\RAVE.exe" "%ProgramW6432%\Rave\RAVE.exe"
copy "%Directori%\Build\RAVE\x64\Debug\RAVE.exe" "%Directori%\Release-x64\RAVE.exe"
copy "%Directori%\Build\RAVE\x64\Debug\RAVE.exe" "\\ST0RM-NAS\Apps\RAVE\Ultima Build\RAVEx64_Debug.exe"
pause
