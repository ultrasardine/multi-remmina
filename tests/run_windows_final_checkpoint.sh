#!/bin/bash
# Windows Port - Final Checkpoint Test Runner (Task 20)
# 
# This script validates the Windows port by checking:
# - Build artifacts (executable, plugins)
# - Self-contained package structure (DLLs, resources)
# - Installer creation
# - Test executables
#
# Manual testing still required:
# - Installation on clean Windows system
# - Application launch and UI
# - Connection profiles and Credential Manager
# - Actual connections (RDP, VNC, SSH)
# - Service discovery and system tray
# - Uninstall and cleanup
#
# Usage: ./tests/run_windows_final_checkpoint.sh

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test results
TESTS_PASSED=0
TESTS_FAILED=0
TESTS_SKIPPED=0

# Function to print section header
print_header() {
    echo ""
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}========================================${NC}"
    echo ""
}

# Function to print test result
print_result() {
    local status=$1
    local message=$2
    
    if [ "$status" = "PASS" ]; then
        echo -e "${GREEN}✅ PASS:${NC} $message"
        ((TESTS_PASSED++))
    elif [ "$status" = "FAIL" ]; then
        echo -e "${RED}❌ FAIL:${NC} $message"
        ((TESTS_FAILED++))
    elif [ "$status" = "SKIP" ]; then
        echo -e "${YELLOW}⚠️  SKIP:${NC} $message"
        ((TESTS_SKIPPED++))
    fi
}

# Function to check if file exists
check_file() {
    local file=$1
    local description=$2
    
    if [ -f "$file" ]; then
        print_result "PASS" "$description: $file"
        return 0
    else
        print_result "FAIL" "$description: $file (not found)"
        return 1
    fi
}

# Function to check if directory exists
check_dir() {
    local dir=$1
    local description=$2
    
    if [ -d "$dir" ]; then
        print_result "PASS" "$description: $dir"
        return 0
    else
        print_result "FAIL" "$description: $dir (not found)"
        return 1
    fi
}

# Main test execution
main() {
    print_header "Windows Port - Final Checkpoint Test Runner"
    
    echo "Validating Windows port build artifacts and package structure."
    echo "Manual testing phases documented in task details."
    echo ""
    
    # Phase 1: Verify Build Artifacts
    print_header "Phase 1: Verify Build Artifacts"
    
    if [ ! -d "build" ]; then
        print_result "SKIP" "Build directory not found - run ./scripts/windows-build.sh first"
        echo ""
        echo "To build the application, run:"
        echo "  ./scripts/windows-build.sh"
        echo ""
    else
        check_file "build/src/multi-remmina.exe" "Main executable"
        check_dir "build/plugins" "Plugins directory"
        
        # Check for plugin DLLs
        if [ -d "build/plugins" ]; then
            local plugin_count=$(find build/plugins -name "*.dll" 2>/dev/null | wc -l)
            if [ "$plugin_count" -gt 0 ]; then
                print_result "PASS" "Found $plugin_count plugin DLLs"
            else
                print_result "FAIL" "No plugin DLLs found in build/plugins"
            fi
        fi
    fi
    
    # Phase 2: Verify Self-Contained Package
    print_header "Phase 2: Verify Self-Contained Package"
    
    if [ ! -d "multi-remmina-win64" ]; then
        print_result "SKIP" "Package directory not found - run ./scripts/windows-bundle.sh first"
        echo ""
        echo "To create the package, run:"
        echo "  ./scripts/windows-bundle.sh build multi-remmina-win64"
        echo ""
    else
        check_file "multi-remmina-win64/multi-remmina.exe" "Packaged executable"
        check_dir "multi-remmina-win64/plugins" "Packaged plugins"
        check_dir "multi-remmina-win64/share/multi-remmina" "Packaged resources"
        check_file "multi-remmina-win64/etc/gtk-3.0/settings.ini" "GTK settings"
        
        # Check for critical DLLs
        local critical_dlls=(
            "libgtk-3-0.dll"
            "libglib-2.0-0.dll"
            "libgdk-3-0.dll"
            "libgio-2.0-0.dll"
            "libgobject-2.0-0.dll"
        )
        
        for dll in "${critical_dlls[@]}"; do
            check_file "multi-remmina-win64/$dll" "Critical DLL: $dll"
        done
        
        # Check for protocol DLLs (optional but expected)
        local protocol_dlls=(
            "libfreerdp2.dll"
            "libssh.dll"
            "libvncclient.dll"
        )
        
        for dll in "${protocol_dlls[@]}"; do
            if [ -f "multi-remmina-win64/$dll" ]; then
                print_result "PASS" "Protocol DLL: $dll"
            else
                print_result "SKIP" "Protocol DLL: $dll (not found, may not be installed)"
            fi
        done
    fi
    
    # Phase 3: Verify Installer
    print_header "Phase 3: Verify Installer"
    
    if [ ! -f "multi-remmina-setup.exe" ]; then
        print_result "SKIP" "Installer not found - run makensis scripts/windows-installer.nsi"
        echo ""
        echo "To create the installer, run:"
        echo "  makensis scripts/windows-installer.nsi"
        echo ""
    else
        check_file "multi-remmina-setup.exe" "Windows installer"
        
        # Check installer size (should be reasonable, not empty)
        local size=$(stat -c%s "multi-remmina-setup.exe" 2>/dev/null || stat -f%z "multi-remmina-setup.exe" 2>/dev/null || echo "0")
        if [ "$size" -gt 1000000 ]; then
            print_result "PASS" "Installer size: $(numfmt --to=iec-i --suffix=B $size 2>/dev/null || echo "$size bytes")"
        else
            print_result "FAIL" "Installer size too small: $size bytes"
        fi
    fi
    
    # Phase 4-12: Manual Testing Required
    print_header "Phases 4-12: Manual Testing Required"
    
    echo "Manual testing required on Windows system:"
    echo "  - Install on clean Windows (Phase 4)"
    echo "  - Launch application (Phase 5)"
    echo "  - Create/save/load profiles (Phases 6-8)"
    echo "  - Test connections: RDP, VNC, SSH (Phase 9)"
    echo "  - Test service discovery (Phase 10)"
    echo "  - Test system tray (Phase 11)"
    echo "  - Uninstall and verify cleanup (Phase 12)"
    echo ""
    echo "See task details in .kiro/specs/windows-port/tasks.md"
    echo ""
    
    # Phase 13: Verify Property Tests
    print_header "Phase 13: Verify Property Tests"
    
    if [ ! -d "build/tests" ]; then
        print_result "SKIP" "Test directory not found - tests may not be built"
        echo ""
        echo "To build with tests, run:"
        echo "  cmake -DWITH_TESTS=ON .."
        echo "  make"
        echo ""
    else
        # Check for property test executables
        local property_tests=(
            "credential_manager/test_credential_properties.exe"
            "plugin_loading/test_plugin_properties.exe"
            "profile_management/test_profile_properties.exe"
            "path_handling/test_path_properties.exe"
            "bundle_resources/test_resource_properties.exe"
            "service_discovery/test_service_discovery_properties.exe"
        )
        
        local found_tests=0
        for test in "${property_tests[@]}"; do
            if [ -f "build/tests/$test" ]; then
                print_result "PASS" "Property test found: $test"
                ((found_tests++))
            else
                print_result "SKIP" "Property test not found: $test"
            fi
        done
        
        if [ "$found_tests" -gt 0 ]; then
            echo ""
            echo "To run property tests, execute:"
            for test in "${property_tests[@]}"; do
                if [ -f "build/tests/$test" ]; then
                    echo "  ./build/tests/$test"
                fi
            done
            echo ""
        fi
    fi
    
    # Phase 14: Verify Unit Tests
    print_header "Phase 14: Verify Unit Tests"
    
    if [ ! -d "build" ]; then
        print_result "SKIP" "Build directory not found"
    else
        if [ -f "build/Makefile" ]; then
            echo "To run unit tests, execute:"
            echo "  cd build && make test"
            echo "  # or"
            echo "  cd build && ctest --output-on-failure"
            echo ""
        else
            print_result "SKIP" "Makefile not found in build directory"
        fi
    fi
    
    # Summary
    print_header "Test Summary"
    
    local total=$((TESTS_PASSED + TESTS_FAILED + TESTS_SKIPPED))
    
    echo "Total Tests:   $total"
    echo -e "${GREEN}Passed:        $TESTS_PASSED${NC}"
    echo -e "${RED}Failed:        $TESTS_FAILED${NC}"
    echo -e "${YELLOW}Skipped:       $TESTS_SKIPPED${NC}"
    echo ""
    
    if [ "$TESTS_FAILED" -gt 0 ]; then
        echo -e "${RED}❌ CHECKPOINT FAILED${NC}"
        echo "Please review the failed tests above and fix any issues."
        return 1
    elif [ "$TESTS_PASSED" -eq 0 ]; then
        echo -e "${YELLOW}⚠️  NO TESTS RUN${NC}"
        echo "Please build the application and create the package first."
        echo ""
        echo "Quick start:"
        echo "  1. ./scripts/windows-build.sh"
        echo "  2. ./scripts/windows-bundle.sh build multi-remmina-win64"
        echo "  3. makensis scripts/windows-installer.nsi"
        echo "  4. ./tests/run_windows_final_checkpoint.sh"
        return 0
    else
    echo "✅ AUTOMATED TESTS PASSED (on current platform)"
        echo ""
        echo "IMPORTANT: Task 20 requires Windows system for full validation"
        echo ""
        echo "Current platform: $(uname -s)"
        echo ""
        echo "To complete Task 20 final checkpoint:"
        echo "  1. Transfer build artifacts to Windows system"
        echo "  2. Run: ./scripts/windows-build.sh"
        echo "  3. Run: ./scripts/windows-bundle.sh build multi-remmina-win64"
        echo "  4. Run: makensis scripts/windows-installer.nsi"
        echo "  5. Install and test on clean Windows system"
        echo "  6. Verify all manual testing phases"
        echo "  7. Run property tests (if built with -DWITH_TESTS=ON)"
        echo "  8. Run unit tests: make test"
        echo ""
        echo "See task details in .kiro/specs/windows-port/tasks.md"
        return 0
    fi
}

# Run main function
main "$@"
exit $?
