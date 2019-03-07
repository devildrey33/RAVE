@echo off
del ".\Release-x86\libvlc.def"
del ".\Release-x86\exports.txt"
dumpbin /exports Release-x86/libvlc.dll > Release-x86/exports.txt
echo EXPORTS >> Release-x86/libvlc.def
for /f "skip=19 tokens=4" %%A in (Release-x86/exports.txt) do echo %%A >> Release-x86/libvlc.def
lib /def:"Release-x86/libvlc.def" /out:"Libs/libvlc.lib" /machine:x86
del ".\Release-x86\libvlc.def"
del ".\Release-x86\exports.txt"
