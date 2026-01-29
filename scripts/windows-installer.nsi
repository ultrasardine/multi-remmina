; Multi-Remmina Windows Installer Script
; Copyright (C) 2024 Multi-Remmina Contributors
; Licensed under GPLv2+

!include "MUI2.nsh"
!include "FileFunc.nsh"

; Installer configuration
Name "Multi-Remmina"
OutFile "multi-remmina-setup.exe"
InstallDir "$PROGRAMFILES64\Multi-Remmina"
InstallDirRegKey HKLM "Software\Multi-Remmina" "InstallDir"
RequestExecutionLevel admin

; Version information
!define VERSION "1.0.0"
!define COMPANY "Multi-Remmina Contributors"
!define URL "https://github.com/multi-remmina/multi-remmina"

VIProductVersion "${VERSION}.0"
VIAddVersionKey "ProductName" "Multi-Remmina"
VIAddVersionKey "CompanyName" "${COMPANY}"
VIAddVersionKey "FileDescription" "Multi-Remmina Remote Desktop Client Installer"
VIAddVersionKey "FileVersion" "${VERSION}"
VIAddVersionKey "ProductVersion" "${VERSION}"
VIAddVersionKey "LegalCopyright" "Copyright (C) 2024 ${COMPANY}"

; Modern UI configuration
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\nsis.bmp"
!define MUI_WELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\win.bmp"

; Installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "COPYING"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_RUN "$INSTDIR\multi-remmina.exe"
!define MUI_FINISHPAGE_RUN_TEXT "Launch Multi-Remmina"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

; Languages
!insertmacro MUI_LANGUAGE "English"

; Installer sections
Section "Multi-Remmina (required)" SecMain
    SectionIn RO
    
    SetOutPath "$INSTDIR"
    
    ; Copy all files from the self-contained package
    File /r "multi-remmina-win64\*.*"
    
    ; Store installation folder
    WriteRegStr HKLM "Software\Multi-Remmina" "InstallDir" "$INSTDIR"
    
    ; Create uninstaller
    WriteUninstaller "$INSTDIR\uninstall.exe"
    
    ; Add uninstall information to registry
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MultiRemmina" \
        "DisplayName" "Multi-Remmina"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MultiRemmina" \
        "DisplayVersion" "${VERSION}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MultiRemmina" \
        "Publisher" "${COMPANY}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MultiRemmina" \
        "URLInfoAbout" "${URL}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MultiRemmina" \
        "UninstallString" "$INSTDIR\uninstall.exe"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MultiRemmina" \
        "QuietUninstallString" "$INSTDIR\uninstall.exe /S"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MultiRemmina" \
        "InstallLocation" "$INSTDIR"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MultiRemmina" \
        "DisplayIcon" "$INSTDIR\multi-remmina.exe"
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MultiRemmina" \
        "NoModify" 1
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MultiRemmina" \
        "NoRepair" 1
    
    ; Calculate and store installation size
    ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
    IntFmt $0 "0x%08X" $0
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MultiRemmina" \
        "EstimatedSize" "$0"
    
SectionEnd

Section "Start Menu Shortcuts" SecStartMenu
    CreateDirectory "$SMPROGRAMS\Multi-Remmina"
    CreateShortcut "$SMPROGRAMS\Multi-Remmina\Multi-Remmina.lnk" \
        "$INSTDIR\multi-remmina.exe" \
        "" \
        "$INSTDIR\multi-remmina.exe" \
        0 \
        SW_SHOWNORMAL \
        "" \
        "Multi-Remmina Remote Desktop Client"
    CreateShortcut "$SMPROGRAMS\Multi-Remmina\Uninstall Multi-Remmina.lnk" \
        "$INSTDIR\uninstall.exe" \
        "" \
        "$INSTDIR\uninstall.exe" \
        0 \
        SW_SHOWNORMAL \
        "" \
        "Uninstall Multi-Remmina"
SectionEnd

Section "Desktop Shortcut" SecDesktop
    CreateShortcut "$DESKTOP\Multi-Remmina.lnk" \
        "$INSTDIR\multi-remmina.exe" \
        "" \
        "$INSTDIR\multi-remmina.exe" \
        0 \
        SW_SHOWNORMAL \
        "" \
        "Multi-Remmina Remote Desktop Client"
SectionEnd

Section "File Associations" SecFileAssoc
    ; Register .remmina file extension
    WriteRegStr HKCR ".remmina" "" "MultiRemmina.Connection"
    WriteRegStr HKCR ".remmina" "Content Type" "application/x-remmina"
    WriteRegStr HKCR ".remmina" "PerceivedType" "document"
    
    ; Register MultiRemmina.Connection class
    WriteRegStr HKCR "MultiRemmina.Connection" "" "Multi-Remmina Connection Profile"
    WriteRegStr HKCR "MultiRemmina.Connection\DefaultIcon" "" "$INSTDIR\multi-remmina.exe,0"
    WriteRegStr HKCR "MultiRemmina.Connection\shell" "" "open"
    WriteRegStr HKCR "MultiRemmina.Connection\shell\open" "" "Open with Multi-Remmina"
    WriteRegStr HKCR "MultiRemmina.Connection\shell\open\command" "" '"$INSTDIR\multi-remmina.exe" "%1"'
    
    ; Notify shell of changes
    System::Call 'shell32.dll::SHChangeNotify(i, i, i, i) v (0x08000000, 0, 0, 0)'
SectionEnd

; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecMain} "Core Multi-Remmina application files (required)"
    !insertmacro MUI_DESCRIPTION_TEXT ${SecStartMenu} "Create shortcuts in the Start Menu"
    !insertmacro MUI_DESCRIPTION_TEXT ${SecDesktop} "Create a shortcut on the Desktop"
    !insertmacro MUI_DESCRIPTION_TEXT ${SecFileAssoc} "Associate .remmina files with Multi-Remmina"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

; Uninstaller section
Section "Uninstall"
    ; Remove files and directories
    RMDir /r "$INSTDIR\plugins"
    RMDir /r "$INSTDIR\share"
    RMDir /r "$INSTDIR\lib"
    RMDir /r "$INSTDIR\etc"
    Delete "$INSTDIR\*.dll"
    Delete "$INSTDIR\*.exe"
    Delete "$INSTDIR\uninstall.exe"
    RMDir "$INSTDIR"
    
    ; Remove Start Menu shortcuts
    Delete "$SMPROGRAMS\Multi-Remmina\Multi-Remmina.lnk"
    Delete "$SMPROGRAMS\Multi-Remmina\Uninstall Multi-Remmina.lnk"
    RMDir "$SMPROGRAMS\Multi-Remmina"
    
    ; Remove Desktop shortcut
    Delete "$DESKTOP\Multi-Remmina.lnk"
    
    ; Remove file associations
    DeleteRegKey HKCR ".remmina"
    DeleteRegKey HKCR "MultiRemmina.Connection"
    
    ; Notify shell of changes
    System::Call 'shell32.dll::SHChangeNotify(i, i, i, i) v (0x08000000, 0, 0, 0)'
    
    ; Remove registry keys
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MultiRemmina"
    DeleteRegKey HKLM "Software\Multi-Remmina"
    
SectionEnd

; Installer initialization
Function .onInit
    ; Check if already installed
    ReadRegStr $R0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MultiRemmina" "UninstallString"
    StrCmp $R0 "" done
    
    MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION \
        "Multi-Remmina is already installed. $\n$\nClick 'OK' to remove the previous version or 'Cancel' to cancel this installation." \
        IDOK uninst
    Abort
    
uninst:
    ClearErrors
    ExecWait '$R0 /S _?=$INSTDIR'
    
done:
FunctionEnd
