@echo off
rem This batch file is used privately by the original author John W. Ratcliff to copy source between
rem a Perforce depot to the SourceForge SVN depot.

md bin
cd bin
md win32
cd win32

copy \GoogleCode\MeshImport\bin\win32\MeshConvert.exe
copy \GoogleCode\MeshImport\bin\win32\MeshImport.dll
copy \GoogleCode\MeshImport\bin\win32\MeshImportEzm.dll
copy \GoogleCode\MeshImport\bin\win32\MeshImportObj.dll
copy \GoogleCode\MeshImport\bin\win32\MeshImportOgre.dll
copy \GoogleCode\MeshImport\bin\win32\MeshImportPSK.dll
copy \GoogleCode\MeshImport\bin\win32\MeshImportFBX.dll
copy \GoogleCode\MeshImport\bin\win32\MeshImportARM.dll


cd ..
cd ..

