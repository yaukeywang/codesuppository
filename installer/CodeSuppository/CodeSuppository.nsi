SetCompressor /SOLID /FINAL lzma
SetDateSave on
SetDatablockOptimize on
CRCCheck on
SilentInstall normal
XPStyle on

!define ShortName      "CodeSuppository"
!define LongName       "CodeSuppository - Mandelbrot Explorer by John W. Ratcliff"
!define ShortVersion   "1.1"
!define LongVersion    "v1.1"
Icon                   "Rocket.ico"
UninstallIcon          "Rocket.ico"
!define RootDir        "..\.."
!define TEMP           "c:\temp"

Name        "${LongName}"
OutFile     "${ShortName}_${ShortVersion}.exe"

InstallDir  "c:\Program Files\CodeSuppository"

InstallDirRegKey HKLM "Software\CodeSuppository" "Install_Dir"

CRCCheck force
BrandingText "CodeSuppository by John W. Ratcliff"

#-------------------------------------------
# License Page Info
#-------------------------------------------
PageEx license
LicenseText "License Agreement"
LicenseData license.txt
PageExEnd

#-------------------------------------------
# Other Page Declarations (including uninstall pages)
#-------------------------------------------
Page components "" .onSelChange ""
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles



#-------------------------------------------
# Executables, DLLs and Data section
#-------------------------------------------
Section "CodeSuppository Executable and Data" sec1

  # ----------Executables and DLLs---------
  SetOutPath "$INSTDIR\bin\win32"

## Install the two executables.
  File "${RootDir}\bin\win32\CodeSuppository.exe"
  File "${RootDir}\bin\win32\CodeSuppository.exe.Manifest"
  File "${RootDir}\bin\win32\ThreadFrac.exe"
  File "${RootDir}\bin\win32\ThreadFrac.exe.Manifest"
  File "${RootDir}\bin\win32\TestMeshImport.exe"
  File "${RootDir}\bin\win32\TestmeshImport.exe.Manifest"

## Install the various DLL components

##  File "${RootDir}\bin\win32\granny2.dll"
  File "${RootDir}\bin\win32\pd3d.dll"
##  File "${RootDir}\bin\win32\PhysXDevice.dll"
##  File "${RootDir}\bin\win32\physxcudart_20.dll"
  File "${RootDir}\bin\win32\RenderDebugPd3d.dll"
##  File "${RootDir}\bin\win32\ClientPhysics.dll"
  File "${RootDir}\bin\win32\MSVCP80.dll"
  File "${RootDir}\bin\win32\MSVCR80.dll"
  File "${RootDir}\bin\win32\D3DX9*.dll"
  File "${RootDir}\bin\win32\MeshImport.dll"
  File "${RootDir}\bin\win32\Assimp32.dll"
  File "${RootDir}\bin\win32\MeshImportAssimp.dll"
  File "${RootDir}\bin\win32\MeshImportEzm.dll"
##  File "${RootDir}\bin\win32\MeshImportGranny.dll"
  File "${RootDir}\bin\win32\MeshImportObj.dll"
  File "${RootDir}\bin\win32\MeshImportOgre.dll"
##  File "${RootDir}\bin\win32\a_character03.ezm"
##  File "${RootDir}\bin\win32\a_character03.aca"
##  File "${RootDir}\bin\win32\PLOAD.dll"
##  File "${RootDir}\bin\win32\PLOAD.pdb"
##  File "${RootDir}\bin\win32\PCORE.dll"
##  File "${RootDir}\bin\win32\PCORE.pdb"
##  File "${RootDir}\bin\win32\PCOOK.dll"
##  File "${RootDir}\bin\win32\PCOOK.pdb"
##  File "${RootDir}\bin\win32\APEX_debug.dll"
##  File "${RootDir}\bin\win32\APEX_release.dll"
##  File "${RootDir}\bin\win32\APEX_ClothingDEBUG.dll"
##  File "${RootDir}\bin\win32\APEX_Clothing.dll"

  File "${RootDir}\bin\win32\*.pal"
##  File "${RootDir}\bin\win32\*.dds"

## Install the demo media
  SetOutPath "$INSTDIR\media\MeshImport"
  File "${RootDir}\media\MeshImport\*.ezm"

  SetOutPath "$INSTDIR\media\CodeSuppository"
  File "${RootDir}\media\CodeSuppository\*.*"

## Install the source code

  SetOutPath "$INSTDIR\installer\CodeSuppository"
  File "${RootDir}\installer\CodeSuppository\CodeSuppository.nsi"
  File "${RootDir}\installer\CodeSuppository\license.txt"
  File "${RootDir}\installer\CodeSuppository\rocket.ico"

  SetOutPath "$INSTDIR\src\Pd3d"
  File "${RootDir}\src\Pd3d\*.cpp"
  File "${RootDir}\src\Pd3d\*.h"
  File "${RootDir}\src\Pd3d\*.dds"
  File "${RootDir}\src\Pd3d\*.fx"
  File "${RootDir}\src\Pd3d\FileCode.exe"

  SetOutPath "$INSTDIR\src\RenderDebug"
  File "${RootDir}\src\RenderDebug\*.cpp"

  SetOutPath "$INSTDIR\src\common\comlayer"
  File "${RootDir}\src\common\comlayer\*.cpp"
  File "${RootDir}\src\common\comlayer\*.h"

  SetOutPath "$INSTDIR\src\common\dxut"
  File "${RootDir}\src\common\dxut\*.cpp"
  File "${RootDir}\src\common\dxut\*.h"
  File "${RootDir}\src\common\dxut\*.ico"

  SetOutPath "$INSTDIR\src\common\snippets"
  File "${RootDir}\src\common\snippets\*.cpp"
  File "${RootDir}\src\common\snippets\*.c"
  File "${RootDir}\src\common\snippets\*.h"

##  SetOutPath "$INSTDIR\src\ClientPhysics"
##  File "${RootDir}\src\ClientPhysics\*.cpp"
##  File "${RootDir}\src\ClientPhysics\*.h"

##  SetOutPath "$INSTDIR\src\HbPhysics"
##  File "${RootDir}\src\HbPhysics\SingleActor.h"

  SetOutPath "$INSTDIR\include\ClientPhysics"
  File "${RootDir}\include\ClientPhysics\*.h"

  SetOutPath "$INSTDIR\include\common\HeMath"
  File "${RootDir}\include\common\HeMath\*.h"
  File "${RootDir}\include\common\HeMath\*.cpp"

  SetOutPath "$INSTDIR\include\MeshImport"
  File "${RootDir}\include\MeshImport\*.h"
  File "${RootDir}\include\MeshImport\*.cpp"

  SetOutPath "$INSTDIR\src\common\tui"
  File "${RootDir}\src\common\tui\*.cpp"
  File "${RootDir}\src\common\tui\*.h"

  SetOutPath "$INSTDIR\src\common\ttmath"
  File "${RootDir}\src\common\ttmath\*.h"

  SetOutPath "$INSTDIR\src\common\rtin"
  File "${RootDir}\src\common\rtin\*.cpp"
  File "${RootDir}\src\common\rtin\*.h"

  SetOutPath "$INSTDIR\src\MeshImport"
  File "${RootDir}\src\MeshImport\*.cpp"
  File "${RootDir}\src\MeshImport\*.h"

  SetOutPath "$INSTDIR\src\MeshImportAssimp"
  File "${RootDir}\src\MeshImportAssimp\*.cpp"

  SetOutPath "$INSTDIR\src\MeshImportEzm"
  File "${RootDir}\src\MeshImportEzm\*.cpp"
  File "${RootDir}\src\MeshImportEzm\*.h"

  SetOutPath "$INSTDIR\src\MeshImportLeveller"
  File "${RootDir}\src\MeshImportLeveller\*.cpp"

  SetOutPath "$INSTDIR\src\MeshImportObj"
  File "${RootDir}\src\MeshImportObj\*.cpp"
  File "${RootDir}\src\MeshImportObj\*.h"

  SetOutPath "$INSTDIR\src\MeshImportOgre"
  File "${RootDir}\src\MeshImportOgre\*.cpp"
  File "${RootDir}\src\MeshImportOgre\*.h"


## DXSDK
  SetOutPath "$INSTDIR\ext\DirectX_October2006"
  File "${RootDir}\ext\DirectX_October2006\*.h"
  File "${RootDir}\ext\DirectX_October2006\*.inl"
  File "${RootDir}\ext\DirectX_October2006\*.lib"

  SetOutPath "$INSTDIR\ext\assimp"
  File "${RootDir}\ext\assimp\*.h"
  File "${RootDir}\ext\assimp\*.hpp"
  File "${RootDir}\ext\assimp\*.inl"
  File "${RootDir}\ext\assimp\*.lib"

  SetOutPath "$INSTDIR\ext\assimp\Compiler"
  File "${RootDir}\ext\assimp\Compiler\*.h"

  SetOutPath "$INSTDIR\ext\assimp\Compiler"
  File "${RootDir}\ext\assimp\Compiler\*.h"

  SetOutPath "$INSTDIR\ext\assimp\Compiler\MSVC"
  File "${RootDir}\ext\assimp\Compiler\MSVC\*.h"


##  SetOutPath "$INSTDIR\ext\APEX\framework\public"
##  File "${RootDir}\ext\APEX\framework\public\*.h"

###  SetOutPath "$INSTDIR\ext\APEX\lib\win32-PhysX_2.8.3"
###  File "${RootDir}\ext\APEX\lib\win32-PhysX_2.8.3\*.lib"

##  SetOutPath "$INSTDIR\ext\APEX\module\clothing\public"
##  File "${RootDir}\ext\APEX\module\clothing\public\*.h"


##  SetOutPath "$INSTDIR\ext\PhysX_2.8.3\cooking\include"
##  File "${RootDir}\ext\PhysX_2.8.3\cooking\include\*.h"

##  SetOutPath "$INSTDIR\ext\PhysX_2.8.3\Foundation\include"
##  File "${RootDir}\ext\PhysX_2.8.3\Foundation\include\*.h"

##  SetOutPath "$INSTDIR\ext\PhysX_2.8.3\lib\win32"
##  File "${RootDir}\ext\PhysX_2.8.3\lib\win32\*.lib"

##  SetOutPath "$INSTDIR\ext\PhysX_2.8.3\NxCharacter\include"
##  File "${RootDir}\ext\PhysX_2.8.3\NxCharacter\include\*.h"

##  SetOutPath "$INSTDIR\ext\PhysX_2.8.3\Physics\include"
##  File "${RootDir}\ext\PhysX_2.8.3\Physics\include\*.h"

##  SetOutPath "$INSTDIR\ext\PhysX_2.8.3\Physics\include\cloth"
##  File "${RootDir}\ext\PhysX_2.8.3\Physics\include\cloth\*.h"

##  SetOutPath "$INSTDIR\ext\PhysX_2.8.3\Physics\include\fluids"
##  File "${RootDir}\ext\PhysX_2.8.3\Physics\include\fluids\*.h"

##  SetOutPath "$INSTDIR\ext\PhysX_2.8.3\Physics\include\softbody"
##  File "${RootDir}\ext\PhysX_2.8.3\Physics\include\softbody\*.h"


##  SetOutPath "$INSTDIR\ext\PhysX_2.8.3\PhysXLoader\include"
##  File "${RootDir}\ext\PhysX_2.8.3\PhysXLoader\include\*.h"


## Install the header files.

  SetOutPath "$INSTDIR\include\common\snippets"
  File "${RootDir}\include\common\snippets\*.inl"
  File "${RootDir}\include\common\snippets\*.cpp"
  File "${RootDir}\include\common\snippets\*.c"
  File "${RootDir}\include\common\snippets\*.h"


##  SetOutPath "$INSTDIR\include\HeroWorld"
##  File "${RootDir}\include\HeroWorld\CollisionGroupFlag.h"

  SetOutPath "$INSTDIR\include\common\TinyXML"
  File "${RootDir}\include\common\TinyXML\*.cpp"
  File "${RootDir}\include\common\TinyXML\*.h"

  SetOutPath "$INSTDIR\include\common\compression"
  File "${RootDir}\include\common\compression\*.cpp"
  File "${RootDir}\include\common\compression\*.c"
  File "${RootDir}\include\common\compression\*.h"

  SetOutPath "$INSTDIR\include\common\FileInterface"
  File "${RootDir}\include\common\FileInterface\*.cpp"
  File "${RootDir}\include\common\FileInterface\*.h"

  SetOutPath "$INSTDIR\include\common\MemoryServices"
  File "${RootDir}\include\common\MemoryServices\*.cpp"
  File "${RootDir}\include\common\MemoryServices\*.h"

  SetOutPath "$INSTDIR\include\common\RandomNumbers"
  File "${RootDir}\include\common\RandomNumbers\*.cpp"
  File "${RootDir}\include\common\RandomNumbers\*.h"

  SetOutPath "$INSTDIR\include\common\AutoGeometry"
  File "${RootDir}\include\common\AutoGeometry\*.cpp"
  File "${RootDir}\include\common\AutoGeometry\*.h"

  SetOutPath "$INSTDIR\include\common\spatial_awareness_system"
  File "${RootDir}\include\common\spatial_awareness_system\*.cpp"
  File "${RootDir}\include\common\spatial_awareness_system\*.h"

  SetOutPath "$INSTDIR\include\Pd3d"
  File "${RootDir}\include\Pd3d\*.h"

  SetOutPath "$INSTDIR\include\RenderDebug"
  File "${RootDir}\include\RenderDebug\*.h"

  SetOutPath "$INSTDIR\include\common\binding"
  File "${RootDir}\include\common\binding\*.h"
  File "${RootDir}\include\common\binding\*.cpp"

  SetOutPath "$INSTDIR\include\common\ResourceInterface"
  File "${RootDir}\include\common\ResourceInterface\*.h"
  File "${RootDir}\include\common\ResourceInterface\*.cpp"


## Install the application source code.
  SetOutPath "$INSTDIR\app\CodeSuppository"
  File "${RootDir}\app\CodeSuppository\*.manifest"
  File "${RootDir}\app\CodeSuppository\*.rc"
  File "${RootDir}\app\CodeSuppository\*.ico"
  File "${RootDir}\app\CodeSuppository\*.cpp"
  File "${RootDir}\app\CodeSuppository\*.h"

  SetOutPath "$INSTDIR\app\TestMeshImport"
  File "${RootDir}\app\TestMeshImport\*.cpp"

  SetOutPath "$INSTDIR\app\ThreadFrac"
  File "${RootDir}\app\ThreadFrac\*.manifest"
  File "${RootDir}\app\ThreadFrac\*.rc"
  File "${RootDir}\app\ThreadFrac\*.ico"
  File "${RootDir}\app\ThreadFrac\*.cpp"
  File "${RootDir}\app\ThreadFrac\*.h"

  SetOutPath "$INSTDIR\app\TestAwareness"
  File "${RootDir}\app\TestAwareness\*.cpp"
  File "${RootDir}\app\TestAwareness\*.h"
  File "${RootDir}\app\TestAwareness\*.sln"
  File "${RootDir}\app\TestAwareness\*.vcproj"

  SetOutPath "$INSTDIR\app\TestAwareness\Release"
  File "${RootDir}\app\TestAwareness\Release\*.exe"


  SetOutPath "$INSTDIR\app\shared\debugmsg"
  File "${RootDir}\app\shared\debugmsg\*.cpp"
  File /nonfatal "${RootDir}\app\shared\debugmsg\*.c"
  File "${RootDir}\app\shared\debugmsg\*.h"

  SetOutPath "$INSTDIR\app\shared\MeshSystem"
  File "${RootDir}\app\shared\MeshSystem\*.cpp"
  File "${RootDir}\app\shared\MeshSystem\*.h"

# Install the project builder files
  SetOutPath "$INSTDIR\compiler\xpj"
  File "${RootDir}\compiler\xpj\MeshImport.xpj"
  File "${RootDir}\compiler\xpj\MeshImportAssimp.xpj"
  File "${RootDir}\compiler\xpj\MeshImportEzm.xpj"
  File "${RootDir}\compiler\xpj\MeshImportLeveller.xpj"
  File "${RootDir}\compiler\xpj\MeshImportObj.xpj"
  File "${RootDir}\compiler\xpj\MeshImportOgre.xpj"
  File "${RootDir}\compiler\xpj\CodeSuppository.xpj"
##  File "${RootDir}\compiler\xpj\ClientPhysics.xpj"
  File "${RootDir}\compiler\xpj\Pd3d.xpj"
  File "${RootDir}\compiler\xpj\RenderDebugPd3d.xpj"
  File "${RootDir}\compiler\xpj\xpj.exe"
  File "${RootDir}\compiler\xpj\build1.bat"

## Install the VC8 solution and project files.
  SetOutPath "$INSTDIR\compiler\VC8"
  File "${RootDir}\compiler\VC8\*.sln"
  File "${RootDir}\compiler\VC8\*.vcproj"
##  File "${RootDir}\compiler\VC8\postbuild_clientphysics_public.bat"
##  Rename $INSTDIR\compiler\VC8\postbuild_clientphysics_public.bat  $INSTDIR\compiler\VC8\postbuild_clientphysics.bat


  CreateDirectory "$SMPROGRAMS\CodeSuppository"

  # add shortcut to CodeSuppository executable
  CreateShortCut  "$SMPROGRAMS\CodeSuppository\CodeSuppository.exe.lnk" "$INSTDIR\bin\win32\CodeSuppository.exe" "" "" "" "SW_SHOWNORMAL" ""

   CreateDirectory "$SMPROGRAMS\CodeSuppository\Projects"

  CreateDirectory "$SMPROGRAMS\CodeSuppository\Projects\VC8"
  CreateShortCut "$SMPROGRAMS\CodeSuppository\Projects\VC8\CodeSuppository.sln.lnk" "$INSTDIR\compiler\VC8\CodeSuppository.sln" "" "" "" "SW_SHOWNORMAL" ""

SectionEnd

#-------------------------------------------
# Registry keys, shortcuts, etc. section
#-------------------------------------------
Section "-RegistryInfo" sec6 ; the - sign indicates it is hidden

  # Write the install dir into the registry
  WriteRegStr HKLM "SOFTWARE\John W. Ratcliff\CodeSuppository" "Install_Dir" "$INSTDIR"

  # Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CodeSuppository" "DisplayName" "CodeSuppository"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CodeSuppository" "UninstallString" '"$INSTDIR\CodeSuppository_uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CodeSuppository" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CodeSuppository" "NoRepair" 1
  WriteUninstaller "CodeSuppository_uninstall.exe"

  # CodeSuppository projects
  CreateDirectory "$SMPROGRAMS\CodeSuppository"

  CreateShortCut "$SMPROGRAMS\CodeSuppository\CodeSuppository_uninstall.lnk" "$INSTDIR\CodeSuppository_uninstall.exe" "" "$INSTDIR\CodeSuppository_uninstall.exe" 0

SectionEnd


#-------------------------------------------
# Uninstaller Section
#-------------------------------------------
Section "Uninstall"

  # Remove registry keys
  DeleteRegKey HKLM "SOFTWARE\John W. Ratcliff\CodeSuppository"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CodeSuppository"

  # Remove uninstaller and its link
  Delete $INSTDIR\CodeSuppository_uninstall.exe

  # Remove all files

  RMDir /r $INSTDIR

SectionEnd


#-------------------------------------------
# Function .onSelChange
#-------------------------------------------
Function .onSelChange
  # Here we detect if all of the sections are unchecked
  # If they are, then we disable the Next button

  ; get all of the flags
  SectionGetFlags ${sec1} $1
  SectionGetFlags ${sec2} $2
  SectionGetFlags ${sec5} $4
  SectionGetFlags ${sec7} $5
  SectionGetFlags ${sec8} $6
  SectionGetFlags ${sec9} $7
  SectionGetFlags ${sec10} $8
  IntOp $9 $1 + $2
  IntOp $9 $9 + $3
  IntCmp $9 0 DisableSix EnableSix EnableSix
EnableSix:
  SectionSetFlags ${sec6} 1
  Goto ContinueOn
DisableSix:
  SectionSetFlags ${sec6} 0
ContinueOn:

  IntOp $0 $1 + $2
  IntOp $0 $0 + $3
  IntOp $0 $0 + $4
  IntOp $0 $0 + $5
  IntOp $0 $0 + $6
  IntOp $0 $0 + $7
  IntOp $0 $0 + $8

  IntCmp $0 0 DisWin EnWin EnWin

DisWin:
  GetDlgItem $0 $HWNDPARENT 1
  EnableWindow $0 0
  Goto FnEnd

EnWin:
  GetDlgItem $0 $HWNDPARENT 1
  EnableWindow $0 1

FnEnd:

FunctionEnd

