@echo off
rem This batch file is used privately by the original author John W. Ratcliff to copy source between
rem a Perforce depot to the SourceForge SVN depot.
md bin
cd bin
md win32
cd win32

rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\bin\win32\*.pal
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\bin\win32\TestMeshImport.exe
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\bin\win32\TestMeshImport.exe.manifest
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\bin\win32\Assimp32.dll
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\bin\win32\MeshImport.dll
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\bin\win32\MeshImportAssimp.dll
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\bin\win32\MeshImportEzm.dll
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\bin\win32\MeshImportLeveller.dll
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\bin\win32\MeshImportObj.dll
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\bin\win32\MeshImportOgre.dll
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\bin\win32\MSVCP80.dll
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\bin\win32\MSVCR80.dll
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\bin\win32\d3dx*.dll
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\bin\win32\ThreadFrac.exe
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\bin\win32\CodeSuppository.exe


cd ..
cd ..

rem md ext
rem cd ext
rem md assimp
rem cd assimp
rem xcopy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\ext\assimp\*.* /s
rem cd ..
rem cd ..

rem md ext
rem cd ext
rem md DirectX_October2006
rem cd DirectX_October2006
rem xcopy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\ext\DirectX_October2006\*.* /s
rem cd ..
rem cd ..

md media
cd media
md MeshImport
cd MeshImport
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\media\MeshImport\*.*"
cd ..
cd ..

md media
cd media
md CodeSuppository
cd CodeSuppository
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\media\CodeSuppository\*.*"
cd ..
cd ..

md include
cd include


md RenderDebug
cd RenderDebug
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\RenderDebug\*.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\RenderDebug\*.cpp
cd ..

md ClientPhysics
cd ClientPhysics
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\ClientPhysics\*.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\ClientPhysics\*.cpp
cd ..

md Pd3d
cd Pd3d
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\Pd3d\*.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\Pd3d\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\Pd3d\*.dds
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\Pd3d\*.fx
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\Pd3d\FileCode.exe
cd ..

md MeshImport
cd MeshImport
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\MeshImport\*.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\MeshImport\*.cpp
cd ..

md MeshImportAssimp
cd MeshImportAssimp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\MeshImportAssimp\*.h
cd ..

md MeshImportEzm
cd MeshImportEzm
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\MeshImportEzm\*.h
cd ..


md MeshImportLeveller
cd MeshImportLeveller
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\MeshImportLeveller\*.h
cd ..

md MeshImportObj
cd MeshImportObj
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\MeshImportObj\*.h
cd ..

md MeshImportOgre
cd MeshImportOgre
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\MeshImportOgre\*.h
cd ..

md _private
cd _private
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\_private\*.h
cd ..

md common
cd common

md binding
cd binding
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\binding\binding.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\binding\binding.cpp
cd ..

md AutoGeometry
cd AutoGeometry
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\AutoGeometry\*.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\AutoGeometry\*.cpp
cd ..

md TinyXML
cd TinyXML
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\TinyXML\*.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\TinyXML\*.cpp
cd ..

md MemoryServices
cd MemoryServices
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\MemoryServices\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\MemoryServices\*.c
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\MemoryServices\*.h
cd ..

md snippets
cd snippets
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\snippets\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\snippets\*.c
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\snippets\*.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\snippets\*.inl
cd ..

md FileInterface
cd FileInterface
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\FileInterface\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\FileInterface\*.h
cd ..

md compression
cd compression
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\compression\*.c
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\compression\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\compression\*.h
cd ..

md HeMath
cd HeMath
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\HeMath\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\HeMath\*.h
cd ..

md InstantMessage
cd InstantMessage
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\InstantMessage\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\InstantMessage\*.h
cd ..

md RandomNumbers
cd RandomNumbers
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\RandomNumbers\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\RandomNumbers\*.h
cd ..

md ResourceInterface
cd ResourceInterface
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\ResourceInterface\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\ResourceInterface\*.h
cd ..

md spatial_awareness_system
cd spatial_awareness_system
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\spatial_awareness_system\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\spatial_awareness_system\*.h
cd ..


cd ..
cd ..

md compiler
cd compiler
md vc8
cd vc8
REM copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\TestMeshImport.sln
REM copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\TestMeshImport.vcproj
REM copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\MeshImport.sln
REM copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\MeshImport.vcproj
REM copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\MeshImportAssimp.sln
REM copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\MeshImportAssimp.vcproj
REM copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\MeshImportEzm.sln
REM copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\MeshImportEzm.vcproj
REM copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\MeshImportLeveller.sln
REM copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\MeshImportLeveller.vcproj
REM copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\MeshImportObj.sln
REM copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\MeshImportObj.vcproj
REM copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\MeshImportOgre.sln
REM copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\MeshImportOgre.vcproj
REM copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\CodeSuppository.sln
REM copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\CodeSuppository.vcproj
REM copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\ThreadFrac.sln
REM copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\ThreadFrac.vcproj
REM copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\RenderDebugPd3d.sln
REM copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\RenderDebugPd3d.vcproj
REM copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\Pd3d.sln
REM copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\Pd3d.vcproj
cd ..
md xpj
cd xpj
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\xpj\CodeSuppository.xpj
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\xpj\ThreadFrac.xpj
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\xpj\RenderDebugPd3d.xpj
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\xpj\Pd3d.xpj
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\xpj\TestMeshImport.xpj
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\xpj\MeshImport.xpj
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\xpj\MeshImportAssimp.xpj
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\xpj\MeshImportEzm.xpj
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\xpj\MeshImportLeveller.xpj
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\xpj\MeshImportObj.xpj
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\xpj\MeshImportOgre.xpj
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\xpj\xpj.exe
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\xpj\pluginmaker.exe
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\xpj\build1.bat
cd ..
cd ..

md app
cd app

md shared
cd shared

md MeshSystem
cd MeshSystem
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\app\shared\MeshSystem\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\app\shared\MeshSystem\*.c
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\app\shared\MeshSystem\*.h
cd ..

md debugmsg
cd debugmsg
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\app\shared\debugmsg\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\app\shared\debugmsg\*.c
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\app\shared\debugmsg\*.h

cd ..
cd ..

md TestMeshImport
cd TestMeshImport
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\app\TestMeshImport\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\app\TestMeshImport\*.c
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\app\TestMeshImport\*.h
cd ..

md TestAwareness
cd TestAwareness
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\app\TestAwareness\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\app\TestAwareness\*.c
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\app\TestAwareness\*.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\app\TestAwareness\*.sln
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\app\TestAwareness\*.vcproj
cd ..

md CodeSuppository
cd CodeSuppository
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\app\CodeSuppository\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\app\CodeSuppository\*.c
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\app\CodeSuppository\*.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\app\CodeSuppository\*.manifest
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\app\CodeSuppository\*.rc
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\app\CodeSuppository\*.ico
cd ..

md ThreadFrac
cd ThreadFrac
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\app\ThreadFrac\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\app\ThreadFrac\*.c
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\app\ThreadFrac\*.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\app\ThreadFrac\*.manifest
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\app\ThreadFrac\*.rc
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\app\ThreadFrac\*.ico
cd ..


cd ..


md docs
cd docs
md MeshImport
cd MeshImport
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\docs\MeshImport\MeshImport.txt
cd ..
cd ..

md installer
cd installer

md MeshImport
cd MeshImport
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\installer\MeshImport\license.txt
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\installer\MeshImport\MeshImport.nsi
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\installer\MeshImport\rocket.ico
cd ..

md CodeSuppository
cd CodeSuppository
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\installer\CodeSuppository\license.txt
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\installer\CodeSuppository\CodeSuppository.nsi
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\installer\CodeSuppository\rocket.ico
cd ..

md ThreadFrac
cd ThreadFrac
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\installer\ThreadFrac\license.txt
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\installer\ThreadFrac\ThreadFrac.nsi
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\installer\ThreadFrac\rocket.ico
cd ..

cd ..

md src
cd src

md common
cd common

md snippets
cd snippets
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\common\snippets\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\common\snippets\*.c
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\common\snippets\*.h
cd ..

md ttmath
cd ttmath
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\common\ttmath\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\common\ttmath\*.c
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\common\ttmath\*.h
cd ..

md tui
cd tui
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\common\tui\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\common\tui\*.c
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\common\tui\*.h
cd ..

md dxut
cd dxut
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\common\dxut\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\common\dxut\*.c
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\common\dxut\*.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\common\dxut\*.ico
cd ..

md comlayer
cd comlayer
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\common\comlayer\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\common\comlayer\*.c
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\common\comlayer\*.h
cd ..

md rtin
cd rtin
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\common\rtin\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\common\rtin\*.c
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\common\rtin\*.h
cd ..


cd ..

md RenderDebug
cd RenderDebug
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\RenderDebug\*.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\RenderDebug\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\RenderDebug\*.c
cd ..


md Pd3d
cd Pd3d
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\Pd3d\*.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\Pd3d\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\Pd3d\*.c
cd ..

md MeshImport
cd MeshImport
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImport\*.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImport\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImport\*.c
cd ..

md MeshImportAssimp
cd MeshImportAssimp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImportAssimp\*.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImportAssimp\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImportAssimp\*.c
cd ..

md MeshImportEzm
cd MeshImportEzm
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImportEzm\*.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImportEzm\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImportEzm\*.c
cd ..


md MeshImportLeveller
cd MeshImportLeveller
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImportLeveller\*.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImportLeveller\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImportLeveller\*.c
cd ..

md MeshImportObj
cd MeshImportObj
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImportObj\*.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImportObj\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImportObj\*.c
cd ..

md MeshImportOgre
cd MeshImportOgre
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImportOgre\*.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImportOgre\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImportOgre\*.c
cd ..

cd ..

cd compiler
cd xpj
call build1 codesuppository.xpj
