echo off
del libvlc.def
dumpbin /exports VLCx86/libvlc.dll > VLCx86/exports.txt
echo EXPORTS >> VLCx86/libvlc.def
for /f "skip=19 tokens=4" %%A in (VLCx86/exports.txt) do echo %%A >> VLCx86/libvlc.def
lib /def:"VLCx86/libvlc.def" /out:"VLCx86/libvlc.lib" /machine:x86