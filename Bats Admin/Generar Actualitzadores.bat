@echo off

set Directori=%~dp0
set Directori=%Directori:\Bats Admin\=%

cd..
cd Actualizador
GenerarActualizador_x64.exe
GenerarActualizador_x86.exe


copy "%Directori%\Actualizador\Actualizador_RAVE_x64.exe" "%Directori%\Instaladores\Actualizador_RAVE_x64.actualizador"
copy "%Directori%\Actualizador\Actualizador_RAVE_x64.md5" "%Directori%\Instaladores\Actualizador_RAVE_x64.md5"
copy "%Directori%\Actualizador\Actualizador_RAVE_x86.exe" "%Directori%\Instaladores\Actualizador_RAVE_x86.actualizador"
copy "%Directori%\Actualizador\Actualizador_RAVE_x86.md5" "%Directori%\Instaladores\Actualizador_RAVE_x86.md5"
pause
