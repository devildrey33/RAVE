echo off
del libvlc.def
dumpbin /exports VLCx64/libvlc.dll > VLCx64/exports.txt
echo EXPORTS >> VLCx64/libvlc.def
for /f "skip=19 tokens=4" %%A in (VLCx64/exports.txt) do echo %%A >> VLCx64/libvlc.def
lib /def:"VLCx64/libvlc.def" /out:"VLCx64/libvlc.lib" /machine:x64