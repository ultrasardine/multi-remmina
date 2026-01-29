#!/bin/bash
# Unit tests for Windows installer NSIS script
# Tests installer structure, shortcuts, file associations, and uninstaller

set -e

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
INSTALLER_SCRIPT="$SCRIPT_DIR/../../scripts/windows-installer.nsi"
TESTS_PASSED=0
TESTS_FAILED=0

# Print test results
print_test_result() {
    local test_name="$1"
    local result="$2"
    
    if [ "$result" = "PASS" ]; then
        echo -e "${GREEN}✓ PASS${NC}: $test_name"
        TESTS_PASSED=$((TESTS_PASSED + 1))
    else
        echo -e "${RED}✗ FAIL${NC}: $test_name"
        TESTS_FAILED=$((TESTS_FAILED + 1))
    fi
}

print_test_header() {
    echo ""
    echo "=========================================="
    echo "$1"
    echo "=========================================="
}

# Test 1: Script exists
test_script_exists() {
    print_test_header "Test 1: Script Existence"
    
    if [ -f "$INSTALLER_SCRIPT" ]; then
        print_test_result "Installer script exists" "PASS"
    else
        print_test_result "Installer script exists" "FAIL"
        return 1
    fi
}

# Test 2: Basic NSIS structure
test_nsis_structure() {
    print_test_header "Test 2: NSIS Script Structure"
    
    # Check for MUI2 include
    if grep -q "!include.*MUI2.nsh" "$INSTALLER_SCRIPT"; then
        print_test_result "Script includes MUI2" "PASS"
    else
        print_test_result "Script includes MUI2" "FAIL"
    fi
    
    # Check for FileFunc include
    if grep -q "!include.*FileFunc.nsh" "$INSTALLER_SCRIPT"; then
        print_test_result "Script includes FileFunc" "PASS"
    else
        print_test_result "Script includes FileFunc" "FAIL"
    fi
    
    # Check for Name directive
    if grep -q "Name.*Multi-Remmina" "$INSTALLER_SCRIPT"; then
        print_test_result "Script defines application name" "PASS"
    else
        print_test_result "Script defines application name" "FAIL"
    fi
    
    # Check for OutFile directive
    if grep -q "OutFile.*multi-remmina-setup.exe" "$INSTALLER_SCRIPT"; then
        print_test_result "Script defines output file" "PASS"
    else
        print_test_result "Script defines output file" "FAIL"
    fi
    
    # Check for InstallDir directive
    if grep -q "InstallDir.*PROGRAMFILES64.*Multi-Remmina" "$INSTALLER_SCRIPT"; then
        print_test_result "Script defines installation directory" "PASS"
    else
        print_test_result "Script defines installation directory" "FAIL"
    fi
    
    # Check for admin execution level
    if grep -q "RequestExecutionLevel.*admin" "$INSTALLER_SCRIPT"; then
        print_test_result "Script requests admin privileges" "PASS"
    else
        print_test_result "Script requests admin privileges" "FAIL"
    fi
}

# Test 3: Version information
test_version_info() {
    print_test_header "Test 3: Version Information"
    
    # Check for version definition
    if grep -q "!define VERSION" "$INSTALLER_SCRIPT"; then
        print_test_result "Script defines version" "PASS"
    else
        print_test_result "Script defines version" "FAIL"
    fi
    
    # Check for VIProductVersion
    if grep -q "VIProductVersion" "$INSTALLER_SCRIPT"; then
        print_test_result "Script sets product version" "PASS"
    else
        print_test_result "Script sets product version" "FAIL"
    fi
    
    # Check for version keys
    if grep -q "VIAddVersionKey.*ProductName" "$INSTALLER_SCRIPT"; then
        print_test_result "Script sets ProductName version key" "PASS"
    else
        print_test_result "Script sets ProductName version key" "FAIL"
    fi
    
    if grep -q "VIAddVersionKey.*CompanyName" "$INSTALLER_SCRIPT"; then
        print_test_result "Script sets CompanyName version key" "PASS"
    else
        print_test_result "Script sets CompanyName version key" "FAIL"
    fi
    
    if grep -q "VIAddVersionKey.*FileVersion" "$INSTALLER_SCRIPT"; then
        print_test_result "Script sets FileVersion version key" "PASS"
    else
        print_test_result "Script sets FileVersion version key" "FAIL"
    fi
}

# Test 4: Installer pages
test_installer_pages() {
    print_test_header "Test 4: Installer Pages"
    
    # Check for welcome page
    if grep -q "MUI_PAGE_WELCOME" "$INSTALLER_SCRIPT"; then
        print_test_result "Script includes welcome page" "PASS"
    else
        print_test_result "Script includes welcome page" "FAIL"
    fi
    
    # Check for license page
    if grep -q "MUI_PAGE_LICENSE" "$INSTALLER_SCRIPT"; then
        print_test_result "Script includes license page" "PASS"
    else
        print_test_result "Script includes license page" "FAIL"
    fi
    
    # Check for directory page
    if grep -q "MUI_PAGE_DIRECTORY" "$INSTALLER_SCRIPT"; then
        print_test_result "Script includes directory selection page" "PASS"
    else
        print_test_result "Script includes directory selection page" "FAIL"
    fi
    
    # Check for components page
    if grep -q "MUI_PAGE_COMPONENTS" "$INSTALLER_SCRIPT"; then
        print_test_result "Script includes components page" "PASS"
    else
        print_test_result "Script includes components page" "FAIL"
    fi
    
    # Check for instfiles page
    if grep -q "MUI_PAGE_INSTFILES" "$INSTALLER_SCRIPT"; then
        print_test_result "Script includes installation files page" "PASS"
    else
        print_test_result "Script includes installation files page" "FAIL"
    fi
    
    # Check for finish page
    if grep -q "MUI_PAGE_FINISH" "$INSTALLER_SCRIPT"; then
        print_test_result "Script includes finish page" "PASS"
    else
        print_test_result "Script includes finish page" "FAIL"
    fi
    
    # Check for finish page run option
    if grep -q "MUI_FINISHPAGE_RUN" "$INSTALLER_SCRIPT"; then
        print_test_result "Script includes run option on finish page" "PASS"
    else
        print_test_result "Script includes run option on finish page" "FAIL"
    fi
}

# Test 5: Uninstaller pages
test_uninstaller_pages() {
    print_test_header "Test 5: Uninstaller Pages"
    
    # Check for uninstall confirm page
    if grep -q "MUI_UNPAGE_CONFIRM" "$INSTALLER_SCRIPT"; then
        print_test_result "Script includes uninstall confirm page" "PASS"
    else
        print_test_result "Script includes uninstall confirm page" "FAIL"
    fi
    
    # Check for uninstall instfiles page
    if grep -q "MUI_UNPAGE_INSTFILES" "$INSTALLER_SCRIPT"; then
        print_test_result "Script includes uninstall files page" "PASS"
    else
        print_test_result "Script includes uninstall files page" "FAIL"
    fi
    
    # Check for uninstall finish page
    if grep -q "MUI_UNPAGE_FINISH" "$INSTALLER_SCRIPT"; then
        print_test_result "Script includes uninstall finish page" "PASS"
    else
        print_test_result "Script includes uninstall finish page" "FAIL"
    fi
}

# Test 6: Main installation section
test_main_section() {
    print_test_header "Test 6: Main Installation Section"
    
    # Check for main section
    if grep -q "Section.*Multi-Remmina.*required" "$INSTALLER_SCRIPT"; then
        print_test_result "Script defines main section" "PASS"
    else
        print_test_result "Script defines main section" "FAIL"
    fi
    
    # Check for SectionIn RO (required)
    if grep -q "SectionIn RO" "$INSTALLER_SCRIPT"; then
        print_test_result "Main section is marked as required" "PASS"
    else
        print_test_result "Main section is marked as required" "FAIL"
    fi
    
    # Check for SetOutPath
    if grep -q "SetOutPath.*INSTDIR" "$INSTALLER_SCRIPT"; then
        print_test_result "Script sets output path" "PASS"
    else
        print_test_result "Script sets output path" "FAIL"
    fi
    
    # Check for File /r (recursive copy)
    if grep -q "File /r.*multi-remmina-win64" "$INSTALLER_SCRIPT"; then
        print_test_result "Script copies all files recursively" "PASS"
    else
        print_test_result "Script copies all files recursively" "FAIL"
    fi
    
    # Check for WriteRegStr to store install dir
    if grep -q "WriteRegStr.*Software.*Multi-Remmina.*InstallDir" "$INSTALLER_SCRIPT"; then
        print_test_result "Script stores installation directory in registry" "PASS"
    else
        print_test_result "Script stores installation directory in registry" "FAIL"
    fi
    
    # Check for WriteUninstaller
    if grep -q "WriteUninstaller.*uninstall.exe" "$INSTALLER_SCRIPT"; then
        print_test_result "Script creates uninstaller" "PASS"
    else
        print_test_result "Script creates uninstaller" "FAIL"
    fi
}

# Test 7: Uninstall registry information
test_uninstall_registry() {
    print_test_header "Test 7: Uninstall Registry Information"
    
    # Check for DisplayName
    if grep -q "DisplayName" "$INSTALLER_SCRIPT" && grep -q "Uninstall.*MultiRemmina" "$INSTALLER_SCRIPT"; then
        print_test_result "Script writes DisplayName to registry" "PASS"
    else
        print_test_result "Script writes DisplayName to registry" "FAIL"
    fi
    
    # Check for DisplayVersion
    if grep -q "DisplayVersion" "$INSTALLER_SCRIPT" && grep -q "Uninstall.*MultiRemmina" "$INSTALLER_SCRIPT"; then
        print_test_result "Script writes DisplayVersion to registry" "PASS"
    else
        print_test_result "Script writes DisplayVersion to registry" "FAIL"
    fi
    
    # Check for Publisher
    if grep -q "Publisher" "$INSTALLER_SCRIPT" && grep -q "Uninstall.*MultiRemmina" "$INSTALLER_SCRIPT"; then
        print_test_result "Script writes Publisher to registry" "PASS"
    else
        print_test_result "Script writes Publisher to registry" "FAIL"
    fi
    
    # Check for UninstallString
    if grep -q "UninstallString" "$INSTALLER_SCRIPT" && grep -q "Uninstall.*MultiRemmina" "$INSTALLER_SCRIPT"; then
        print_test_result "Script writes UninstallString to registry" "PASS"
    else
        print_test_result "Script writes UninstallString to registry" "FAIL"
    fi
    
    # Check for QuietUninstallString
    if grep -q "QuietUninstallString" "$INSTALLER_SCRIPT" && grep -q "Uninstall.*MultiRemmina" "$INSTALLER_SCRIPT"; then
        print_test_result "Script writes QuietUninstallString to registry" "PASS"
    else
        print_test_result "Script writes QuietUninstallString to registry" "FAIL"
    fi
    
    # Check for InstallLocation
    if grep -q "InstallLocation" "$INSTALLER_SCRIPT" && grep -q "Uninstall.*MultiRemmina" "$INSTALLER_SCRIPT"; then
        print_test_result "Script writes InstallLocation to registry" "PASS"
    else
        print_test_result "Script writes InstallLocation to registry" "FAIL"
    fi
    
    # Check for DisplayIcon
    if grep -q "DisplayIcon" "$INSTALLER_SCRIPT" && grep -q "Uninstall.*MultiRemmina" "$INSTALLER_SCRIPT"; then
        print_test_result "Script writes DisplayIcon to registry" "PASS"
    else
        print_test_result "Script writes DisplayIcon to registry" "FAIL"
    fi
    
    # Check for NoModify
    if grep -q "NoModify" "$INSTALLER_SCRIPT" && grep -q "Uninstall.*MultiRemmina" "$INSTALLER_SCRIPT"; then
        print_test_result "Script writes NoModify to registry" "PASS"
    else
        print_test_result "Script writes NoModify to registry" "FAIL"
    fi
    
    # Check for NoRepair
    if grep -q "NoRepair" "$INSTALLER_SCRIPT" && grep -q "Uninstall.*MultiRemmina" "$INSTALLER_SCRIPT"; then
        print_test_result "Script writes NoRepair to registry" "PASS"
    else
        print_test_result "Script writes NoRepair to registry" "FAIL"
    fi
    
    # Check for EstimatedSize
    if grep -q "EstimatedSize" "$INSTALLER_SCRIPT" && grep -q "Uninstall.*MultiRemmina" "$INSTALLER_SCRIPT"; then
        print_test_result "Script writes EstimatedSize to registry" "PASS"
    else
        print_test_result "Script writes EstimatedSize to registry" "FAIL"
    fi
    
    # Check for GetSize function usage
    if grep -q "GetSize" "$INSTALLER_SCRIPT"; then
        print_test_result "Script calculates installation size" "PASS"
    else
        print_test_result "Script calculates installation size" "FAIL"
    fi
}

# Test 8: Start Menu shortcuts section
test_start_menu_section() {
    print_test_header "Test 8: Start Menu Shortcuts Section"
    
    # Check for Start Menu section
    if grep -q "Section.*Start Menu Shortcuts" "$INSTALLER_SCRIPT"; then
        print_test_result "Script defines Start Menu section" "PASS"
    else
        print_test_result "Script defines Start Menu section" "FAIL"
    fi
    
    # Check for CreateDirectory
    if grep -q "CreateDirectory.*SMPROGRAMS.*Multi-Remmina" "$INSTALLER_SCRIPT"; then
        print_test_result "Script creates Start Menu directory" "PASS"
    else
        print_test_result "Script creates Start Menu directory" "FAIL"
    fi
    
    # Check for application shortcut
    if grep -q "CreateShortcut.*SMPROGRAMS.*Multi-Remmina.*Multi-Remmina.lnk" "$INSTALLER_SCRIPT"; then
        print_test_result "Script creates application shortcut" "PASS"
    else
        print_test_result "Script creates application shortcut" "FAIL"
    fi
    
    # Check for uninstall shortcut
    if grep -q "CreateShortcut.*SMPROGRAMS.*Multi-Remmina.*Uninstall" "$INSTALLER_SCRIPT"; then
        print_test_result "Script creates uninstall shortcut" "PASS"
    else
        print_test_result "Script creates uninstall shortcut" "FAIL"
    fi
    
    # Check for shortcut parameters
    if grep -q "multi-remmina.exe" "$INSTALLER_SCRIPT" && grep -q "CreateShortcut" "$INSTALLER_SCRIPT"; then
        print_test_result "Shortcuts point to correct executable" "PASS"
    else
        print_test_result "Shortcuts point to correct executable" "FAIL"
    fi
}

# Test 9: Desktop shortcut section
test_desktop_section() {
    print_test_header "Test 9: Desktop Shortcut Section"
    
    # Check for Desktop section
    if grep -q "Section.*Desktop Shortcut" "$INSTALLER_SCRIPT"; then
        print_test_result "Script defines Desktop shortcut section" "PASS"
    else
        print_test_result "Script defines Desktop shortcut section" "FAIL"
    fi
    
    # Check for desktop shortcut creation
    if grep -q "CreateShortcut.*DESKTOP.*Multi-Remmina.lnk" "$INSTALLER_SCRIPT"; then
        print_test_result "Script creates desktop shortcut" "PASS"
    else
        print_test_result "Script creates desktop shortcut" "FAIL"
    fi
    
    # Check that desktop section is optional (not SectionIn RO)
    local desktop_section=$(sed -n '/Section.*Desktop Shortcut/,/^SectionEnd/p' "$INSTALLER_SCRIPT")
    if ! echo "$desktop_section" | grep -q "SectionIn RO"; then
        print_test_result "Desktop shortcut section is optional" "PASS"
    else
        print_test_result "Desktop shortcut section is optional" "FAIL"
    fi
}

# Test 10: File associations section
test_file_associations() {
    print_test_header "Test 10: File Associations Section"
    
    # Check for File Associations section
    if grep -q "Section.*File Associations" "$INSTALLER_SCRIPT"; then
        print_test_result "Script defines file associations section" "PASS"
    else
        print_test_result "Script defines file associations section" "FAIL"
    fi
    
    # Check for .remmina extension registration
    if grep -q "WriteRegStr.*HKCR.*\\.remmina" "$INSTALLER_SCRIPT"; then
        print_test_result "Script registers .remmina extension" "PASS"
    else
        print_test_result "Script registers .remmina extension" "FAIL"
    fi
    
    # Check for MultiRemmina.Connection class
    if grep -q "WriteRegStr.*HKCR.*MultiRemmina.Connection" "$INSTALLER_SCRIPT"; then
        print_test_result "Script registers MultiRemmina.Connection class" "PASS"
    else
        print_test_result "Script registers MultiRemmina.Connection class" "FAIL"
    fi
    
    # Check for DefaultIcon
    if grep -q "WriteRegStr.*HKCR.*MultiRemmina.Connection.*DefaultIcon" "$INSTALLER_SCRIPT"; then
        print_test_result "Script sets default icon for .remmina files" "PASS"
    else
        print_test_result "Script sets default icon for .remmina files" "FAIL"
    fi
    
    # Check for shell open command
    if grep -q "WriteRegStr.*HKCR.*MultiRemmina.Connection.*shell.*open.*command" "$INSTALLER_SCRIPT"; then
        print_test_result "Script registers shell open command" "PASS"
    else
        print_test_result "Script registers shell open command" "FAIL"
    fi
    
    # Check for %1 parameter in command
    if grep -q "multi-remmina.exe.*%1" "$INSTALLER_SCRIPT"; then
        print_test_result "Open command includes file parameter" "PASS"
    else
        print_test_result "Open command includes file parameter" "FAIL"
    fi
    
    # Check for SHChangeNotify call
    if grep -q "SHChangeNotify" "$INSTALLER_SCRIPT"; then
        print_test_result "Script notifies shell of file association changes" "PASS"
    else
        print_test_result "Script notifies shell of file association changes" "FAIL"
    fi
}

# Test 11: Section descriptions
test_section_descriptions() {
    print_test_header "Test 11: Section Descriptions"
    
    # Check for MUI_FUNCTION_DESCRIPTION_BEGIN
    if grep -q "MUI_FUNCTION_DESCRIPTION_BEGIN" "$INSTALLER_SCRIPT"; then
        print_test_result "Script defines section descriptions" "PASS"
    else
        print_test_result "Script defines section descriptions" "FAIL"
    fi
    
    # Check for main section description
    if grep -q "MUI_DESCRIPTION_TEXT.*SecMain" "$INSTALLER_SCRIPT"; then
        print_test_result "Script describes main section" "PASS"
    else
        print_test_result "Script describes main section" "FAIL"
    fi
    
    # Check for Start Menu description
    if grep -q "MUI_DESCRIPTION_TEXT.*SecStartMenu" "$INSTALLER_SCRIPT"; then
        print_test_result "Script describes Start Menu section" "PASS"
    else
        print_test_result "Script describes Start Menu section" "FAIL"
    fi
    
    # Check for Desktop description
    if grep -q "MUI_DESCRIPTION_TEXT.*SecDesktop" "$INSTALLER_SCRIPT"; then
        print_test_result "Script describes Desktop section" "PASS"
    else
        print_test_result "Script describes Desktop section" "FAIL"
    fi
    
    # Check for File Associations description
    if grep -q "MUI_DESCRIPTION_TEXT.*SecFileAssoc" "$INSTALLER_SCRIPT"; then
        print_test_result "Script describes File Associations section" "PASS"
    else
        print_test_result "Script describes File Associations section" "FAIL"
    fi
    
    # Check for MUI_FUNCTION_DESCRIPTION_END
    if grep -q "MUI_FUNCTION_DESCRIPTION_END" "$INSTALLER_SCRIPT"; then
        print_test_result "Script closes section descriptions" "PASS"
    else
        print_test_result "Script closes section descriptions" "FAIL"
    fi
}

# Test 12: Uninstaller section
test_uninstaller_section() {
    print_test_header "Test 12: Uninstaller Section"
    
    # Check for Uninstall section
    if grep -q "Section.*Uninstall" "$INSTALLER_SCRIPT"; then
        print_test_result "Script defines uninstall section" "PASS"
    else
        print_test_result "Script defines uninstall section" "FAIL"
    fi
    
    # Check for directory removal
    if grep -q "RMDir /r.*INSTDIR" "$INSTALLER_SCRIPT"; then
        print_test_result "Script removes installation directory" "PASS"
    else
        print_test_result "Script removes installation directory" "FAIL"
    fi
    
    # Check for plugins removal
    if grep -q "RMDir.*plugins" "$INSTALLER_SCRIPT"; then
        print_test_result "Script removes plugins directory" "PASS"
    else
        print_test_result "Script removes plugins directory" "FAIL"
    fi
    
    # Check for share removal
    if grep -q "RMDir.*share" "$INSTALLER_SCRIPT"; then
        print_test_result "Script removes share directory" "PASS"
    else
        print_test_result "Script removes share directory" "FAIL"
    fi
    
    # Check for DLL removal
    if grep -q "Delete.*INSTDIR.*\\.dll" "$INSTALLER_SCRIPT"; then
        print_test_result "Script removes DLL files" "PASS"
    else
        print_test_result "Script removes DLL files" "FAIL"
    fi
    
    # Check for executable removal
    if grep -q "Delete.*INSTDIR.*\\.exe" "$INSTALLER_SCRIPT"; then
        print_test_result "Script removes executable files" "PASS"
    else
        print_test_result "Script removes executable files" "FAIL"
    fi
    
    # Check for Start Menu removal
    if grep -q "Delete.*SMPROGRAMS.*Multi-Remmina" "$INSTALLER_SCRIPT"; then
        print_test_result "Script removes Start Menu shortcuts" "PASS"
    else
        print_test_result "Script removes Start Menu shortcuts" "FAIL"
    fi
    
    # Check for Desktop shortcut removal
    if grep -q "Delete.*DESKTOP.*Multi-Remmina.lnk" "$INSTALLER_SCRIPT"; then
        print_test_result "Script removes desktop shortcut" "PASS"
    else
        print_test_result "Script removes desktop shortcut" "FAIL"
    fi
    
    # Check for file association removal
    if grep -q "DeleteRegKey.*HKCR.*\\.remmina" "$INSTALLER_SCRIPT"; then
        print_test_result "Script removes .remmina file association" "PASS"
    else
        print_test_result "Script removes .remmina file association" "FAIL"
    fi
    
    if grep -q "DeleteRegKey.*HKCR.*MultiRemmina.Connection" "$INSTALLER_SCRIPT"; then
        print_test_result "Script removes MultiRemmina.Connection class" "PASS"
    else
        print_test_result "Script removes MultiRemmina.Connection class" "FAIL"
    fi
    
    # Check for registry key removal
    if grep -q "DeleteRegKey.*Uninstall.*MultiRemmina" "$INSTALLER_SCRIPT"; then
        print_test_result "Script removes uninstall registry key" "PASS"
    else
        print_test_result "Script removes uninstall registry key" "FAIL"
    fi
    
    if grep -q "DeleteRegKey.*Software.*Multi-Remmina" "$INSTALLER_SCRIPT"; then
        print_test_result "Script removes application registry key" "PASS"
    else
        print_test_result "Script removes application registry key" "FAIL"
    fi
}

# Test 13: Installer initialization
test_installer_init() {
    print_test_header "Test 13: Installer Initialization"
    
    # Check for .onInit function
    if grep -q "Function .onInit" "$INSTALLER_SCRIPT"; then
        print_test_result "Script defines initialization function" "PASS"
    else
        print_test_result "Script defines initialization function" "FAIL"
    fi
    
    # Check for existing installation detection
    if grep -q "ReadRegStr" "$INSTALLER_SCRIPT" && grep -q "UninstallString" "$INSTALLER_SCRIPT"; then
        print_test_result "Script checks for existing installation" "PASS"
    else
        print_test_result "Script checks for existing installation" "FAIL"
    fi
    
    # Check for MessageBox on existing installation
    if grep -q "MessageBox" "$INSTALLER_SCRIPT" && grep -q "already installed" "$INSTALLER_SCRIPT"; then
        print_test_result "Script prompts user about existing installation" "PASS"
    else
        print_test_result "Script prompts user about existing installation" "FAIL"
    fi
    
    # Check for ExecWait to run uninstaller
    if grep -q "ExecWait" "$INSTALLER_SCRIPT"; then
        print_test_result "Script can uninstall previous version" "PASS"
    else
        print_test_result "Script can uninstall previous version" "FAIL"
    fi
}

# Test 14: Language support
test_language_support() {
    print_test_header "Test 14: Language Support"
    
    # Check for MUI_LANGUAGE
    if grep -q "MUI_LANGUAGE.*English" "$INSTALLER_SCRIPT"; then
        print_test_result "Script includes English language" "PASS"
    else
        print_test_result "Script includes English language" "FAIL"
    fi
}

# Test 15: License file reference
test_license_reference() {
    print_test_header "Test 15: License File Reference"
    
    # Check for license file in MUI_PAGE_LICENSE
    if grep -q "MUI_PAGE_LICENSE.*COPYING" "$INSTALLER_SCRIPT"; then
        print_test_result "Script references COPYING license file" "PASS"
    else
        print_test_result "Script references COPYING license file" "FAIL"
    fi
}

# Run all tests
main() {
    echo "=========================================="
    echo "Windows Installer Script Unit Tests"
    echo "=========================================="
    echo "Testing: $INSTALLER_SCRIPT"
    
    test_script_exists
    test_nsis_structure
    test_version_info
    test_installer_pages
    test_uninstaller_pages
    test_main_section
    test_uninstall_registry
    test_start_menu_section
    test_desktop_section
    test_file_associations
    test_section_descriptions
    test_uninstaller_section
    test_installer_init
    test_language_support
    test_license_reference
    
    echo ""
    echo "=========================================="
    echo "Test Summary"
    echo "=========================================="
    echo -e "${GREEN}Passed: $TESTS_PASSED${NC}"
    echo -e "${RED}Failed: $TESTS_FAILED${NC}"
    echo "Total: $((TESTS_PASSED + TESTS_FAILED))"
    echo "=========================================="
    
    if [ $TESTS_FAILED -eq 0 ]; then
        echo -e "${GREEN}All tests passed!${NC}"
        exit 0
    else
        echo -e "${RED}Some tests failed!${NC}"
        exit 1
    fi
}

# Run main function
main
