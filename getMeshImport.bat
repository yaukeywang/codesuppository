@echo off
rem This batch file is used privately by the original author John W. Ratcliff to copy source between
rem a Perforce depot to the SourceForge SVN depot.

md bin
cd bin
md win32
cd win32

copy \GoogleCode\MeshImport\bin\win32\MeshConvert.exe
copy \GoogleCode\MeshImport\bin\win32\MeshConvert.exe.manifest
copy \GoogleCode\MeshImport\bin\win32\MeshImport.dll
copy \GoogleCode\MeshImport\bin\win32\MeshImportEzm.dll
copy \GoogleCode\MeshImport\bin\win32\MeshImportObj.dll
copy \GoogleCode\MeshImport\bin\win32\MeshImportOgre.dll
copy \GoogleCode\MeshImport\bin\win32\MeshImportPSK.dll
copy \GoogleCode\MeshImport\bin\win32\MeshImportFBX.dll
copy \GoogleCode\MeshImport\bin\win32\MSVCP80.dll
copy \GoogleCode\MeshImport\bin\win32\MSVCR80.dll


cd ..
cd ..

md ext
cd ext
md fbx
cd fbx
xcopy c:\p4\experimental\MeshImport\ext\fbx\*.* /s
cd ..
cd ..

md media
cd media
md MeshImport
cd MeshImport
copy \GoogleCode\MeshImport\media\MeshImport\*.*"
cd ..
cd ..

md include
cd include

md MeshImport
cd MeshImport
copy \GoogleCode\MeshImport\include\MeshImport\*.h
copy \GoogleCode\MeshImport\include\MeshImport\*.cpp
cd ..

md MeshImportPSK
cd MeshImportPSK
copy \GoogleCode\MeshImport\include\MeshImportPSK\*.h
cd ..

md MeshImportFBX
cd MeshImportFBX
copy \GoogleCode\MeshImport\include\MeshImportFBX\*.h
cd ..

md MeshImportEzm
cd MeshImportEzm
copy \GoogleCode\MeshImport\include\MeshImportEzm\*.h
cd ..


md MeshImportObj
cd MeshImportObj
copy \GoogleCode\MeshImport\include\MeshImportObj\*.h
cd ..

md MeshImportOgre
cd MeshImportOgre
copy \GoogleCode\MeshImport\include\MeshImportOgre\*.h
cd ..

md common
cd common

md binding
cd binding
copy \GoogleCode\MeshImport\include\common\binding\binding.h
copy \GoogleCode\MeshImport\include\common\binding\binding.cpp
cd ..

md snippets
cd snippets
copy \GoogleCode\MeshImport\include\common\snippets\UserMemAlloc.h
copy \GoogleCode\MeshImport\include\common\snippets\KeyValue.h
copy \GoogleCode\MeshImport\include\common\snippets\KeyValue.cpp
copy \GoogleCode\MeshImport\include\common\snippets\KeyValueIni.h
copy \GoogleCode\MeshImport\include\common\snippets\KeyValueIni.cpp
copy \GoogleCode\MeshImport\include\common\snippets\winmsg.h
copy \GoogleCode\MeshImport\include\common\snippets\winmsg.cpp
copy \GoogleCode\MeshImport\include\common\snippets\telnet.h
copy \GoogleCode\MeshImport\include\common\snippets\telnet.cpp
copy \GoogleCode\MeshImport\include\common\snippets\FileInterface.h
copy \GoogleCode\MeshImport\include\common\snippets\FileInterface.cpp
copy \GoogleCode\MeshImport\include\common\snippets\FileSystem.h
copy \GoogleCode\MeshImport\include\common\snippets\SystemServices.cpp
copy \GoogleCode\MeshImport\include\common\snippets\SystemServices.h
copy \GoogleCode\MeshImport\include\common\snippets\inparser.h
copy \GoogleCode\MeshImport\include\common\snippets\inparser.cpp
copy \GoogleCode\MeshImport\include\common\snippets\asc2bin.cpp
copy \GoogleCode\MeshImport\include\common\snippets\asc2bin.h
copy \GoogleCode\MeshImport\include\common\snippets\fmem.h
copy \GoogleCode\MeshImport\include\common\snippets\stable.h
copy \GoogleCode\MeshImport\include\common\snippets\StringDict.cpp
copy \GoogleCode\MeshImport\include\common\snippets\StringDict.h
copy \GoogleCode\MeshImport\include\common\snippets\SendTextMessage.h
copy \GoogleCode\MeshImport\include\common\snippets\NxSimpleTypes.h
copy \GoogleCode\MeshImport\include\common\snippets\NxAssert.h
copy \GoogleCode\MeshImport\include\common\snippets\Nx.h
copy \GoogleCode\MeshImport\include\common\snippets\FastXml.h
copy \GoogleCode\MeshImport\include\common\snippets\FastXml.cpp
copy \GoogleCode\MeshImport\include\common\snippets\FloatMath.inl
copy \GoogleCode\MeshImport\include\common\snippets\FloatMath.cpp
copy \GoogleCode\MeshImport\include\common\snippets\FloatMath.h
copy \GoogleCode\MeshImport\include\common\snippets\sutil.h
copy \GoogleCode\MeshImport\include\common\snippets\sutil.cpp
cd ..

cd ..
cd ..

md compiler
cd compiler
md vc8
cd vc8
REM copy \GoogleCode\MeshImport\compiler\vc8\MeshConvert.sln
REM copy \GoogleCode\MeshImport\compiler\vc8\MeshConvert.vcproj
REM copy \GoogleCode\MeshImport\compiler\vc8\MeshImport.sln
REM copy \GoogleCode\MeshImport\compiler\vc8\MeshImport.vcproj
REM copy \GoogleCode\MeshImport\compiler\vc8\MeshImportEzm.sln
REM copy \GoogleCode\MeshImport\compiler\vc8\MeshImportEzm.vcproj
REM copy \GoogleCode\MeshImport\compiler\vc8\MeshImportFbx.vcproj
REM copy \GoogleCode\MeshImport\compiler\vc8\MeshImportPSK.sln
REM copy \GoogleCode\MeshImport\compiler\vc8\MeshImportPSK.vcproj
REM copy \GoogleCode\MeshImport\compiler\vc8\MeshImportObj.sln
REM copy \GoogleCode\MeshImport\compiler\vc8\MeshImportObj.vcproj
REM copy \GoogleCode\MeshImport\compiler\vc8\MeshImportOgre.sln
REM copy \GoogleCode\MeshImport\compiler\vc8\MeshImportOgre.vcproj
cd ..
md xpj
cd xpj
copy \GoogleCode\MeshImport\compiler\xpj\MeshConvert.xpj
copy \GoogleCode\MeshImport\compiler\xpj\MeshImport.xpj
copy \GoogleCode\MeshImport\compiler\xpj\MeshImportFBX.xpj
copy \GoogleCode\MeshImport\compiler\xpj\MeshImportEzm.xpj
copy \GoogleCode\MeshImport\compiler\xpj\MeshImportPSK.xpj
copy \GoogleCode\MeshImport\compiler\xpj\MeshImportObj.xpj
copy \GoogleCode\MeshImport\compiler\xpj\MeshImportOgre.xpj
copy \GoogleCode\MeshImport\compiler\xpj\xpj.exe
copy \GoogleCode\MeshImport\compiler\xpj\build1.bat
cd ..
cd ..

md app
cd app
md MeshConvert
cd MeshConvert
copy \GoogleCode\MeshImport\app\MeshConvert\*.cpp
copy \GoogleCode\MeshImport\app\MeshConvert\*.c
copy \GoogleCode\MeshImport\app\MeshConvert\*.h
cd ..
cd ..

md docs
cd docs
md MeshImport
cd MeshImport
copy \GoogleCode\MeshImport\docs\MeshImport\MeshImport.txt
cd ..
cd ..

md src
cd src


md MeshImport
cd MeshImport
copy \GoogleCode\MeshImport\src\MeshImport\*.h
copy \GoogleCode\MeshImport\src\MeshImport\*.cpp
copy \GoogleCode\MeshImport\src\MeshImport\*.c
cd ..

md MeshImportFBX
cd MeshImportFBX
copy \GoogleCode\MeshImport\src\MeshImportFBX\*.h
copy \GoogleCode\MeshImport\src\MeshImportFBX\*.cpp
copy \GoogleCode\MeshImport\src\MeshImportFBX\*.c
cd ..

md MeshImportEzm
cd MeshImportEzm
copy \GoogleCode\MeshImport\src\MeshImportEzm\*.h
copy \GoogleCode\MeshImport\src\MeshImportEzm\*.cpp
copy \GoogleCode\MeshImport\src\MeshImportEzm\*.c
cd ..


md MeshImportPSK
cd MeshImportPSk
copy \GoogleCode\MeshImport\src\MeshImportPSK\*.h
copy \GoogleCode\MeshImport\src\MeshImportPSK\*.cpp
copy \GoogleCode\MeshImport\src\MeshImportPSK\*.c
cd ..

md MeshImportObj
cd MeshImportObj
copy \GoogleCode\MeshImport\src\MeshImportObj\*.h
copy \GoogleCode\MeshImport\src\MeshImportObj\*.cpp
copy \GoogleCode\MeshImport\src\MeshImportObj\*.c
cd ..

md MeshImportOgre
cd MeshImportOgre
copy \GoogleCode\MeshImport\src\MeshImportOgre\*.h
copy \GoogleCode\MeshImport\src\MeshImportOgre\*.cpp
copy \GoogleCode\MeshImport\src\MeshImportOgre\*.c
cd ..

cd ..
