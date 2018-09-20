@echo off
del ".\VLCx64-Debug\libvlc.def"
dumpbin /exports VLCx64-Debug/libvlc.dll > VLCx64-Debug/exports.txt
echo EXPORTS >> VLCx64-Debug/libvlc.def
for /f "skip=19 tokens=4" %%A in (VLCx64-Debug/exports.txt) do echo %%A >> VLCx64-Debug/libvlc.def
lib /def:"VLCx64-Debug/libvlc.def" /out:"VLCx64-Debug/libvlc.lib" /machine:x64