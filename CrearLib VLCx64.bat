@echo off
del ".\Release-x64\libvlc.def"
del ".\Release-x64\exports.txt"
dumpbin /exports Release-x64/libvlc.dll > VLCx64/exports.txt
echo EXPORTS >> Release-x64/libvlc.def
for /f "skip=19 tokens=4" %%A in (Release-x64/exports.txt) do echo %%A >> Release-x64/libvlc.def
lib /def:"Release-x64/libvlc.def" /out:"Libs/libvlc64.lib" /machine:x64
del ".\Release-x64\libvlc.def"
del ".\Release-x64\exports.txt"
 