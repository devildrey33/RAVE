@echo off
set Directori=%~dp0
set Directori=%Directori:\Bats Admin\=%

copy "%Directori%\Build\LoaderActualizador\x86\Release\Actualizador.exe" "%PROGRAMFILES(X86)%\Rave\Actualizador.exe"
copy "%Directori%\Build\RAVE\x86\Release\RAVE.exe" "%PROGRAMFILES(X86)%\Rave\Rave.exe"
copy "%Directori%\Build\RAVE\x86\Release\RAVE.exe" "%Directori%\Release-x86\RAVE.exe"
pause
