; NSIS Installer Script for vscpl1drv-can232
; VSCP Level I driver for CAN232 devices

!define PRODUCT_NAME "VSCP CAN232 Driver"
!define PRODUCT_VERSION "1.1.1"
!define PRODUCT_PUBLISHER "VSCP Project"
!define PRODUCT_WEB_SITE "https://github.com/vscp/vscpl1drv-can232"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\vscpl1drv-can232.dll"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; MUI Settings
!include "MUI2.nsh"

; Installer attributes
Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "vscpl1drv-can232-${PRODUCT_VERSION}-installer.exe"
InstallDir "$PROGRAMFILES\VSCP\drivers\level1"
ShowInstDetails show
ShowUnInstDetails show

; MUI Settings
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

; Language
!insertmacro MUI_LANGUAGE "English"

; Installer sections
Section "VSCP CAN232 Driver" SEC01
    SetOutPath "$INSTDIR"
    SetOverwrite on
    
    ; Install DLL
    File "can232drv.dll"
    
    ; Install import library if available
    ${If} ${FileExists} "can232drv.lib"
        File "can232drv.lib"
    ${EndIf}
    
    ; Install debug symbols if available
    ${If} ${FileExists} "can232drv.pdb"
        File "can232drv.pdb"
    ${EndIf}
    
    ; Install static library if available
    ${If} ${FileExists} "can232drv.a"
        File "can232drv.a"
    ${EndIf}
    
    SetOutPath "$INSTDIR\include"
    File "can232obj.h"
    File "dlldrvobj.h"
    
    SetOutPath "$INSTDIR\doc"
    File "..\README.md"
    File "..\LICENSE"
    
    ; Create uninstaller
    WriteUninstaller "$INSTDIR\Uninstall.exe"
    
    ; Write registry
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "${PRODUCT_NAME}"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\Uninstall.exe"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
    WriteRegDWord ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "NoModify" 1
    WriteRegDWord ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "NoRepair" 1
SectionEnd

Section -AdditionalIcons
    SetOutPath "$SMPROGRAMS"
    CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
    CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
    CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Driver Directory.lnk" "$INSTDIR"
SectionEnd

Section -Post
    WriteRegStr HKCU "Software\${PRODUCT_NAME}" "" $INSTDIR
SectionEnd

; Uninstaller section
Section Uninstall
    Delete "$INSTDIR\can232drv.dll"
    Delete "$INSTDIR\can232drv.lib"
    Delete "$INSTDIR\can232drv.pdb"
    Delete "$INSTDIR\can232drv.a"
    Delete "$INSTDIR\include\can232obj.h"
    Delete "$INSTDIR\include\dlldrvobj.h"
    Delete "$INSTDIR\doc\README.md"
    Delete "$INSTDIR\doc\LICENSE"
    Delete "$INSTDIR\Uninstall.exe"
    
    RMDir "$INSTDIR\include"
    RMDir "$INSTDIR\doc"
    RMDir "$INSTDIR"
    RMDir "$PROGRAMFILES\VSCP\drivers"
    RMDir "$PROGRAMFILES\VSCP"
    
    Delete "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk"
    Delete "$SMPROGRAMS\${PRODUCT_NAME}\Driver Directory.lnk"
    RMDir "$SMPROGRAMS\${PRODUCT_NAME}"
    
    DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
    DeleteRegKey HKCU "Software\${PRODUCT_NAME}"
    SetAutoClose true
SectionEnd
