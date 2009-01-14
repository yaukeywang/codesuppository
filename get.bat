@echo off
rem This batch file is used privately by the original author John W. Ratcliff to copy source between
rem a Perforce depot to the SourceForge SVN depot.
md bin
cd bin
md win32
cd win32

rem copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\bin\win32\*.pal
rem copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\bin\win32\TestMeshImport.exe
rem copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\bin\win32\TestMeshImport.exe.manifest
rem copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\bin\win32\Assimp32.dll
rem copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\bin\win32\MeshImport.dll
rem copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\bin\win32\MeshImportAssimp.dll
rem copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\bin\win32\MeshImportEzm.dll
rem copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\bin\win32\MeshImportFbx.dll
rem copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\bin\win32\MeshImportLeveller.dll
rem copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\bin\win32\MeshImportObj.dll
rem copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\bin\win32\MeshImportPsk.dll
rem copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\bin\win32\MeshImportOgre.dll
rem copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\bin\win32\MSVCP80.dll
rem copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\bin\win32\MSVCR80.dll
rem copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\bin\win32\d3dx*.dll
rem copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\bin\win32\ThreadFrac.exe
rem copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\bin\win32\CodeSuppository.exe


cd ..
cd ..

rem md ext
rem cd ext
rem md assimp
rem cd assimp
rem xcopy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\ext\assimp\*.* /s
rem cd ..
rem cd ..

rem md ext
rem cd ext
rem md DirectX_October2006
rem cd DirectX_October2006
rem xcopy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\ext\DirectX_October2006\*.* /s
rem cd ..
rem cd ..

md media
cd media
md MeshImport
cd MeshImport
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\media\MeshImport\*.*"
cd ..
cd ..

md media
cd media
md CodeSuppository
cd CodeSuppository
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\media\CodeSuppository\*.*"
cd ..
cd ..

md include
cd include


md RenderDebug
cd RenderDebug
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\RenderDebug\*.h
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\RenderDebug\*.cpp
cd ..

md Pd3d
cd Pd3d
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\Pd3d\*.h
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\Pd3d\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\Pd3d\*.dds
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\Pd3d\*.fx
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\Pd3d\FileCode.exe
cd ..

md PathSystem
cd PathSystem
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\PathSystem\*.h
cd ..

md MeshImport
cd MeshImport
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\MeshImport\*.h
cd ..

md MeshImportAssimp
cd MeshImportAssimp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\MeshImportAssimp\*.h
cd ..

md MeshImportEzm
cd MeshImportEzm
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\MeshImportEzm\*.h
cd ..


md MeshImportFbx
cd MeshImportFbx
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\MeshImportFbx\*.h
cd ..

md MeshImportLeveller
cd MeshImportLeveller
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\MeshImportLeveller\*.h
cd ..

md MeshImportObj
cd MeshImportObj
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\MeshImportObj\*.h
cd ..

md MeshImportPsk
cd MeshImportPsk
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\MeshImportPsk\*.h
cd ..

md MeshImportOgre
cd MeshImportOgre
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\MeshImportOgre\*.h
cd ..

md _private
cd _private
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\_private\*.h
cd ..

md common
cd common

md binding
cd binding
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\common\binding\binding.h
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\common\binding\binding.cpp
cd ..

md TinyXML
cd TinyXML
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\common\TinyXML\*.h
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\common\TinyXML\*.cpp
cd ..

md MemoryServices
cd MemoryServices
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\common\MemoryServices\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\common\MemoryServices\*.c
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\common\MemoryServices\*.h
cd ..

md snippets
cd snippets
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\common\snippets\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\common\snippets\*.c
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\common\snippets\*.h
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\common\snippets\*.inl
cd ..

md FileInterface
cd FileInterface
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\common\FileInterface\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\common\FileInterface\*.h
cd ..

md compression
cd compression
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\common\compression\*.c
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\common\compression\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\common\compression\*.h
cd ..

md HeMath
cd HeMath
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\common\HeMath\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\common\HeMath\*.h
cd ..

md InstantMessage
cd InstantMessage
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\common\InstantMessage\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\common\InstantMessage\*.h
cd ..

md RandomNumbers
cd RandomNumbers
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\common\RandomNumbers\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\common\RandomNumbers\*.h
cd ..

md ResourceInterface
cd ResourceInterface
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\common\ResourceInterface\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\common\ResourceInterface\*.h
cd ..

md spatial_awareness_system
cd spatial_awareness_system
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\common\spatial_awareness_system\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\include\common\spatial_awareness_system\*.h
cd ..


cd ..
cd ..

md compiler
cd compiler
md vc8
cd vc8
REM copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\vc8\TestMeshImport.sln
REM copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\vc8\TestMeshImport.vcproj
REM copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\vc8\MeshImport.sln
REM copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\vc8\MeshImport.vcproj
REM copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\vc8\MeshImportAssimp.sln
REM copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\vc8\MeshImportAssimp.vcproj
REM copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\vc8\MeshImportEzm.sln
REM copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\vc8\MeshImportEzm.vcproj
REM copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\vc8\MeshImportFbx.sln
REM copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\vc8\MeshImportFbx.vcproj
REM copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\vc8\MeshImportLeveller.sln
REM copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\vc8\MeshImportLeveller.vcproj
REM copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\vc8\MeshImportObj.sln
REM copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\vc8\MeshImportObj.vcproj
REM copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\vc8\MeshImportPsk.sln
REM copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\vc8\MeshImportPsk.vcproj
REM copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\vc8\MeshImportOgre.sln
REM copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\vc8\MeshImportOgre.vcproj
REM copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\vc8\CodeSuppository.sln
REM copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\vc8\CodeSuppository.vcproj
REM copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\vc8\ThreadFrac.sln
REM copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\vc8\ThreadFrac.vcproj
REM copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\vc8\RenderDebugPd3d.sln
REM copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\vc8\RenderDebugPd3d.vcproj
REM copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\vc8\Pd3d.sln
REM copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\vc8\Pd3d.vcproj
cd ..
md xpj
cd xpj
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\xpj\PathSystem.xpj
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\xpj\CodeSuppository.xpj
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\xpj\ThreadFrac.xpj
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\xpj\RenderDebugPd3d.xpj
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\xpj\Pd3d.xpj
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\xpj\TestMeshImport.xpj
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\xpj\MeshImport.xpj
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\xpj\MeshImportAssimp.xpj
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\xpj\MeshImportEzm.xpj
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\xpj\MeshImportFbx.xpj
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\xpj\MeshImportLeveller.xpj
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\xpj\MeshImportObj.xpj
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\xpj\MeshImportPsk.xpj
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\xpj\MeshImportOgre.xpj
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\xpj\xpj.exe
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\xpj\pluginmaker.exe
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\compiler\xpj\build1.bat
cd ..
cd ..

md app
cd app

md shared
cd shared

md MeshSystem
cd MeshSystem
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\app\shared\MeshSystem\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\app\shared\MeshSystem\*.c
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\app\shared\MeshSystem\*.h
cd ..

md debugmsg
cd debugmsg
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\app\shared\debugmsg\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\app\shared\debugmsg\*.c
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\app\shared\debugmsg\*.h

cd ..
cd ..

md TestMeshImport
cd TestMeshImport
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\app\TestMeshImport\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\app\TestMeshImport\*.c
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\app\TestMeshImport\*.h
cd ..

md TestAwareness
cd TestAwareness
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\app\TestAwareness\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\app\TestAwareness\*.c
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\app\TestAwareness\*.h
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\app\TestAwareness\*.sln
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\app\TestAwareness\*.vcproj
cd ..

md CodeSuppository
cd CodeSuppository
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\app\CodeSuppository\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\app\CodeSuppository\*.c
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\app\CodeSuppository\*.h
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\app\CodeSuppository\*.manifest
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\app\CodeSuppository\*.rc
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\app\CodeSuppository\*.ico
cd ..

md ThreadFrac
cd ThreadFrac
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\app\ThreadFrac\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\app\ThreadFrac\*.c
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\app\ThreadFrac\*.h
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\app\ThreadFrac\*.manifest
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\app\ThreadFrac\*.rc
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\app\ThreadFrac\*.ico
cd ..


cd ..


md docs
cd docs
md MeshImport
cd MeshImport
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\docs\MeshImport\MeshImport.txt
cd ..
cd ..

md installer
cd installer

md MeshImport
cd MeshImport
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\installer\MeshImport\license.txt
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\installer\MeshImport\MeshImport.nsi
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\installer\MeshImport\rocket.ico
cd ..

md CodeSuppository
cd CodeSuppository
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\installer\CodeSuppository\license.txt
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\installer\CodeSuppository\CodeSuppository.nsi
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\installer\CodeSuppository\rocket.ico
cd ..

md ThreadFrac
cd ThreadFrac
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\installer\ThreadFrac\license.txt
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\installer\ThreadFrac\ThreadFrac.nsi
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\installer\ThreadFrac\rocket.ico
cd ..

cd ..

md src
cd src

md common
cd common

md snippets
cd snippets
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\common\snippets\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\common\snippets\*.c
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\common\snippets\*.h
cd ..

md ttmath
cd ttmath
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\common\ttmath\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\common\ttmath\*.c
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\common\ttmath\*.h
cd ..

md tui
cd tui
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\common\tui\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\common\tui\*.c
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\common\tui\*.h
cd ..

md dxut
cd dxut
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\common\dxut\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\common\dxut\*.c
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\common\dxut\*.h
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\common\dxut\*.ico
cd ..

md comlayer
cd comlayer
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\common\comlayer\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\common\comlayer\*.c
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\common\comlayer\*.h
cd ..

md rtin
cd rtin
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\common\rtin\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\common\rtin\*.c
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\common\rtin\*.h
cd ..


cd ..

md RenderDebug
cd RenderDebug
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\RenderDebug\*.h
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\RenderDebug\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\RenderDebug\*.c
cd ..


md Pd3d
cd Pd3d
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\Pd3d\*.h
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\Pd3d\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\Pd3d\*.c
cd ..

md MeshImport
cd MeshImport
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\MeshImport\*.h
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\MeshImport\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\MeshImport\*.c
cd ..

md MeshImportAssimp
cd MeshImportAssimp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\MeshImportAssimp\*.h
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\MeshImportAssimp\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\MeshImportAssimp\*.c
cd ..

md MeshImportEzm
cd MeshImportEzm
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\MeshImportEzm\*.h
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\MeshImportEzm\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\MeshImportEzm\*.c
cd ..


md MeshImportFbx
cd MeshImportFbx
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\MeshImportFbx\*.h
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\MeshImportFbx\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\MeshImportFbx\*.c
cd ..

md MeshImportLeveller
cd MeshImportLeveller
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\MeshImportLeveller\*.h
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\MeshImportLeveller\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\MeshImportLeveller\*.c
cd ..

md MeshImportObj
cd MeshImportObj
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\MeshImportObj\*.h
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\MeshImportObj\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\MeshImportObj\*.c
cd ..

md MeshImportPsk
cd MeshImportPsk
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\MeshImportPsk\*.h
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\MeshImportPsk\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\MeshImportPsk\*.c
cd ..

md MeshImportOgre
cd MeshImportOgre
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\MeshImportOgre\*.h
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\MeshImportOgre\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\MeshImportOgre\*.c
cd ..

md PathSystem
cd PathSystem
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\PathSystem\*.h
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\PathSystem\*.cpp
copy \p4\depot\HeroEngine\RENDERING-REFACTOR-MAIN\he_plugins\src\PathSystem\*.c
cd ..

cd ..
