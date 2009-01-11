SetCompressor /SOLID /FINAL lzma
SetDateSave on
SetDatablockOptimize on
CRCCheck on
SilentInstall normal
XPStyle on

!define ShortName      "ThreadFrac"
!define LongName       "ThreadFrac - Mandelbrot Explorer by John W. Ratcliff"
!define ShortVersion   "1.0"
!define LongVersion    "v1.0"
Icon                   "Rocket.ico"
UninstallIcon          "Rocket.ico"
!define RootDir        "..\.."
!define TEMP           "c:\temp"

Name        "${LongName}"
OutFile     "${ShortName}_${ShortVersion}.exe"

InstallDir  "c:\Program Files\ThreadFrac"

InstallDirRegKey HKLM "Software\ThreadFrac" "Install_Dir"

CRCCheck force
BrandingText "ThreadFrac by John W. Ratcliff"

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
Section "ThreadFrac Executable and Data" sec1

  # ----------Executables and DLLs---------
  SetOutPath "$INSTDIR\bin\win32"

## Install the two executables.
  File "${RootDir}\bin\win32\ThreadFrac.exe"
  File "${RootDir}\bin\win32\ThreadFrac.exe.Manifest"

## Install the various DLL components

  File "${RootDir}\bin\win32\Pd3d.dll"
  File "${RootDir}\bin\win32\RenderDebugPd3d.dll"

##  File "${RootDir}\bin\win32\MSVCM80.dll"
  File "${RootDir}\bin\win32\MSVCP80.dll"
  File "${RootDir}\bin\win32\MSVCR80.dll"
  File "${RootDir}\bin\win32\D3DX9_31.dll"
  File "${RootDir}\bin\win32\*.pal"


## Install the demo media
  SetOutPath "$INSTDIR\media\CodeSuppository"
  File "${RootDir}\media\CodeSuppository\ThreadFrac.psc"

  CreateDirectory "$SMPROGRAMS\ThreadFrac"

  # add shortcut to ThreadFrac executable
  CreateShortCut  "$SMPROGRAMS\ThreadFrac\ThreadFrac.exe.lnk" "$INSTDIR\bin\win32\ThreadFrac.exe" "" "" "" "SW_SHOWNORMAL" ""


SectionEnd

#-------------------------------------------
# Registry keys, shortcuts, etc. section
#-------------------------------------------
Section "-RegistryInfo" sec6 ; the - sign indicates it is hidden

  # Write the install dir into the registry
  WriteRegStr HKLM "SOFTWARE\John W. Ratcliff\ThreadFrac" "Install_Dir" "$INSTDIR"

  # Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ThreadFrac" "DisplayName" "ThreadFrac"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ThreadFrac" "UninstallString" '"$INSTDIR\ThreadFrac_uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ThreadFrac" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ThreadFrac" "NoRepair" 1
  WriteUninstaller "ThreadFrac_uninstall.exe"

  # ThreadFrac projects
  CreateDirectory "$SMPROGRAMS\ThreadFrac"

  CreateShortCut "$SMPROGRAMS\ThreadFrac\ThreadFrac_uninstall.lnk" "$INSTDIR\ThreadFrac_uninstall.exe" "" "$INSTDIR\ThreadFrac_uninstall.exe" 0

SectionEnd


#-------------------------------------------
# Uninstaller Section
#-------------------------------------------
Section "Uninstall"

  # Remove registry keys
  DeleteRegKey HKLM "SOFTWARE\John W. Ratcliff\ThreadFrac"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ThreadFrac"

  # Remove uninstaller and its link
  Delete $INSTDIR\ThreadFrac_uninstall.exe

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

