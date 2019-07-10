@echo off
set Directori=%~dp0
set Directori=%Directori:\Bats Admin\=%

copy "%Directori%\Build\LoaderActualizador\x64\Debug\Actualizador.exe" "%ProgramW6432%\Rave\Actualizador.exe"
copy "%Directori%\Build\RAVE\x64\Debug\RAVE.exe" "%ProgramW6432%\Rave\RAVE.exe"
copy "%Directori%\Build\RAVE\x64\Debug\RAVE.exe" "%Directori%\Release-x64\RAVE.exe"
pause
