#!/bin/bash
# Unit tests for Windows build script
# Tests dependency checking and error reporting

set -e

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_SCRIPT="$SCRIPT_DIR/../../scripts/windows-build.sh"
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

# Test 1: Script exists and is executable
test_script_exists() {
    print_test_header "Test 1: Script Existence and Permissions"
    
    if [ -f "$BUILD_SCRIPT" ]; then
        print_test_result "Build script exists" "PASS"
    else
        print_test_result "Build script exists" "FAIL"
        return 1
    fi
    
    if [ -x "$BUILD_SCRIPT" ]; then
        print_test_result "Build script is executable" "PASS"
    else
        print_test_result "Build script is executable" "FAIL"
        return 1
    fi
}

# Test 2: Help option works
test_help_option() {
    print_test_header "Test 2: Help Option"
    
    if "$BUILD_SCRIPT" --help > /dev/null 2>&1; then
        print_test_result "Help option works" "PASS"
    else
        print_test_result "Help option works" "FAIL"
        return 1
    fi
    
    # Check if help output contains expected sections
    local help_output=$("$BUILD_SCRIPT" --help 2>&1)
    
    if echo "$help_output" | grep -q "Usage:"; then
        print_test_result "Help contains usage section" "PASS"
    else
        print_test_result "Help contains usage section" "FAIL"
    fi
    
    if echo "$help_output" | grep -q "OPTIONS:"; then
        print_test_result "Help contains options section" "PASS"
    else
        print_test_result "Help contains options section" "FAIL"
    fi
    
    if echo "$help_output" | grep -q "REQUIREMENTS:"; then
        print_test_result "Help contains requirements section" "PASS"
    else
        print_test_result "Help contains requirements section" "FAIL"
    fi
}

# Test 3: MSYS2 environment checking
test_msys2_checking() {
    print_test_header "Test 3: MSYS2 Environment Checking"
    
    # Test if script checks for MSYSTEM variable
    if grep -q "MSYSTEM" "$BUILD_SCRIPT"; then
        print_test_result "Script checks for MSYSTEM variable" "PASS"
    else
        print_test_result "Script checks for MSYSTEM variable" "FAIL"
    fi
    
    # Test if script checks for pacman
    if grep -q "command -v pacman" "$BUILD_SCRIPT"; then
        print_test_result "Script checks for pacman package manager" "PASS"
    else
        print_test_result "Script checks for pacman package manager" "FAIL"
    fi
    
    # Test if script provides MSYS2 download URL
    if grep -q "msys2.org" "$BUILD_SCRIPT"; then
        print_test_result "Script provides MSYS2 download URL" "PASS"
    else
        print_test_result "Script provides MSYS2 download URL" "FAIL"
    fi
}

# Test 4: Dependency checking function
test_dependency_checking() {
    print_test_header "Test 4: Dependency Checking Logic"
    
    # Test if script has dependency list
    if grep -q "base_packages=(" "$BUILD_SCRIPT"; then
        print_test_result "Script defines dependency list" "PASS"
    else
        print_test_result "Script defines dependency list" "FAIL"
    fi
    
    # Test if script checks for required packages
    local required_deps=("cmake" "make" "gcc" "gtk3" "glib2" "libsodium" "openssl" "libssh" "freerdp" "libvncserver")
    local all_deps_found=true
    
    for dep in "${required_deps[@]}"; do
        if ! grep -q "$dep" "$BUILD_SCRIPT"; then
            all_deps_found=false
            echo "  Missing dependency: $dep"
            break
        fi
    done
    
    if [ "$all_deps_found" = true ]; then
        print_test_result "Script includes all required dependencies" "PASS"
    else
        print_test_result "Script includes all required dependencies" "FAIL"
    fi
    
    # Test if script handles package prefix based on MSYS2 environment
    if grep -q "mingw-w64-x86_64\|mingw-w64-ucrt-x86_64" "$BUILD_SCRIPT"; then
        print_test_result "Script handles MSYS2 package prefixes" "PASS"
    else
        print_test_result "Script handles MSYS2 package prefixes" "FAIL"
    fi
}

# Test 5: Error reporting for missing MSYS2
test_msys2_error_reporting() {
    print_test_header "Test 5: Error Reporting for Missing MSYS2"
    
    # Check if script has error message for missing MSYS2
    if grep -q "Not running in MSYS2 environment" "$BUILD_SCRIPT"; then
        print_test_result "Script has error message for missing MSYS2" "PASS"
    else
        print_test_result "Script has error message for missing MSYS2" "FAIL"
    fi
    
    # Check if script provides MSYS2 shell instructions
    if grep -q "MSYS2 MinGW 64-bit shell" "$BUILD_SCRIPT"; then
        print_test_result "Script provides MSYS2 shell instructions" "PASS"
    else
        print_test_result "Script provides MSYS2 shell instructions" "FAIL"
    fi
    
    # Check if script exits on missing MSYS2
    if grep -A 5 "Not running in MSYS2 environment" "$BUILD_SCRIPT" | grep -q "exit 1"; then
        print_test_result "Script exits when MSYS2 is missing" "PASS"
    else
        print_test_result "Script exits when MSYS2 is missing" "FAIL"
    fi
}

# Test 6: Error reporting for missing dependencies
test_dependency_error_reporting() {
    print_test_header "Test 6: Error Reporting for Missing Dependencies"
    
    # Check if script tracks missing packages
    if grep -q "missing_packages" "$BUILD_SCRIPT"; then
        print_test_result "Script tracks missing packages" "PASS"
    else
        print_test_result "Script tracks missing packages" "FAIL"
    fi
    
    # Check if script attempts to install missing packages
    if grep -q "pacman -S" "$BUILD_SCRIPT"; then
        print_test_result "Script installs missing packages via pacman" "PASS"
    else
        print_test_result "Script installs missing packages via pacman" "FAIL"
    fi
    
    # Check if script uses --noconfirm flag
    if grep -q "pacman -S --noconfirm" "$BUILD_SCRIPT"; then
        print_test_result "Script uses --noconfirm flag for automated installation" "PASS"
    else
        print_test_result "Script uses --noconfirm flag for automated installation" "FAIL"
    fi
    
    # Check if script exits on installation failure
    if grep -A 5 "Failed to install required packages" "$BUILD_SCRIPT" | grep -q "exit 1"; then
        print_test_result "Script exits on dependency installation failure" "PASS"
    else
        print_test_result "Script exits on dependency installation failure" "FAIL"
    fi
    
    # Check if script provides manual installation command
    if grep -q "Please run manually: pacman -S" "$BUILD_SCRIPT"; then
        print_test_result "Script provides manual installation command on failure" "PASS"
    else
        print_test_result "Script provides manual installation command on failure" "FAIL"
    fi
}

# Test 7: CMake configuration options
test_cmake_options() {
    print_test_header "Test 7: CMake Configuration Options"
    
    # Check for Windows-specific CMake options
    local cmake_options=(
        "MSYS Makefiles"
        "WITH_AVAHI=OFF"
        "WITH_APPINDICATOR=OFF"
        "WITH_TELEPATHY=OFF"
        "WITH_LIBSECRET=OFF"
        "WITH_WINDOWS_CREDENTIAL_MANAGER=ON"
    )
    
    local all_options_found=true
    for option in "${cmake_options[@]}"; do
        if ! grep -q "$option" "$BUILD_SCRIPT"; then
            all_options_found=false
            echo "  Missing option: $option"
            break
        fi
    done
    
    if [ "$all_options_found" = true ]; then
        print_test_result "Script includes all Windows-specific CMake options" "PASS"
    else
        print_test_result "Script includes all Windows-specific CMake options" "FAIL"
    fi
    
    # Check for MSYS Makefiles generator
    if grep -q "\-G \"MSYS Makefiles\"" "$BUILD_SCRIPT"; then
        print_test_result "Script uses MSYS Makefiles generator" "PASS"
    else
        print_test_result "Script uses MSYS Makefiles generator" "FAIL"
    fi
}

# Test 8: Parallel build configuration
test_parallel_build() {
    print_test_header "Test 8: Parallel Build Configuration"
    
    # Check if script detects CPU cores
    if grep -q "nproc" "$BUILD_SCRIPT"; then
        print_test_result "Script detects CPU core count" "PASS"
    else
        print_test_result "Script detects CPU core count" "FAIL"
    fi
    
    # Check if script uses parallel make
    if grep -q "make -j" "$BUILD_SCRIPT"; then
        print_test_result "Script uses parallel make" "PASS"
    else
        print_test_result "Script uses parallel make" "FAIL"
    fi
    
    # Check if script has fallback CPU count
    if grep -q "nproc.*||.*echo" "$BUILD_SCRIPT"; then
        print_test_result "Script has fallback CPU count" "PASS"
    else
        print_test_result "Script has fallback CPU count" "FAIL"
    fi
}

# Test 9: Build directory management
test_build_directory() {
    print_test_header "Test 9: Build Directory Management"
    
    # Check if script creates build directory
    if grep -q "mkdir -p" "$BUILD_SCRIPT"; then
        print_test_result "Script creates build directory" "PASS"
    else
        print_test_result "Script creates build directory" "FAIL"
    fi
    
    # Check if script cleans existing build directory
    if grep -q "rm -rf" "$BUILD_SCRIPT"; then
        print_test_result "Script cleans existing build directory" "PASS"
    else
        print_test_result "Script cleans existing build directory" "FAIL"
    fi
    
    # Check if script accepts custom build directory
    if grep -q "build-dir" "$BUILD_SCRIPT" || grep -q "build_dir" "$BUILD_SCRIPT"; then
        print_test_result "Script accepts custom build directory option" "PASS"
    else
        print_test_result "Script accepts custom build directory option" "FAIL"
    fi
}

# Test 10: Error handling
test_error_handling() {
    print_test_header "Test 10: Error Handling"
    
    # Check if script uses set -e
    if grep -q "set -e" "$BUILD_SCRIPT"; then
        print_test_result "Script exits on error (set -e)" "PASS"
    else
        print_test_result "Script exits on error (set -e)" "FAIL"
    fi
    
    # Check if script validates CMake success
    if grep -A 5 "cmake" "$BUILD_SCRIPT" | grep -q "if.*\$?.*eq 0\|if.*cmake"; then
        print_test_result "Script validates CMake success" "PASS"
    else
        print_test_result "Script validates CMake success" "FAIL"
    fi
    
    # Check if script validates make success
    if grep -A 5 "make" "$BUILD_SCRIPT" | grep -q "if.*make"; then
        print_test_result "Script validates make success" "PASS"
    else
        print_test_result "Script validates make success" "FAIL"
    fi
}

# Test 11: Command line argument parsing
test_argument_parsing() {
    print_test_header "Test 11: Command Line Argument Parsing"
    
    # Check if script handles --help
    if grep -q "\-h\|--help" "$BUILD_SCRIPT"; then
        print_test_result "Script handles --help option" "PASS"
    else
        print_test_result "Script handles --help option" "FAIL"
    fi
    
    # Check if script handles build type
    if grep -q "build-type\|build_type" "$BUILD_SCRIPT"; then
        print_test_result "Script handles build type option" "PASS"
    else
        print_test_result "Script handles build type option" "FAIL"
    fi
    
    # Check if script validates build type
    if grep -q "Release\|Debug" "$BUILD_SCRIPT"; then
        print_test_result "Script validates build type values" "PASS"
    else
        print_test_result "Script validates build type values" "FAIL"
    fi
    
    # Check if script handles --skip-deps option
    if grep -q "skip-deps\|skip_deps" "$BUILD_SCRIPT"; then
        print_test_result "Script handles --skip-deps option" "PASS"
    else
        print_test_result "Script handles --skip-deps option" "FAIL"
    fi
    
    # Check if script handles --cmake-only option
    if grep -q "cmake-only\|cmake_only" "$BUILD_SCRIPT"; then
        print_test_result "Script handles --cmake-only option" "PASS"
    else
        print_test_result "Script handles --cmake-only option" "FAIL"
    fi
}

# Test 12: MSYS2 environment support
test_msys2_environment_support() {
    print_test_header "Test 12: MSYS2 Environment Support"
    
    # Check if script supports MINGW64
    if grep -q "MINGW64" "$BUILD_SCRIPT"; then
        print_test_result "Script supports MINGW64 environment" "PASS"
    else
        print_test_result "Script supports MINGW64 environment" "FAIL"
    fi
    
    # Check if script supports UCRT64
    if grep -q "UCRT64" "$BUILD_SCRIPT"; then
        print_test_result "Script supports UCRT64 environment" "PASS"
    else
        print_test_result "Script supports UCRT64 environment" "FAIL"
    fi
    
    # Check if script rejects unsupported environments
    if grep -q "Unsupported MSYS2 environment" "$BUILD_SCRIPT"; then
        print_test_result "Script rejects unsupported MSYS2 environments" "PASS"
    else
        print_test_result "Script rejects unsupported MSYS2 environments" "FAIL"
    fi
}

# Test 13: Output executable location
test_output_location() {
    print_test_header "Test 13: Output Executable Location"
    
    # Check if script reports executable location
    if grep -q "Executable location:" "$BUILD_SCRIPT"; then
        print_test_result "Script reports executable location" "PASS"
    else
        print_test_result "Script reports executable location" "FAIL"
    fi
    
    # Check if script mentions .exe extension
    if grep -q "multi-remmina.exe" "$BUILD_SCRIPT"; then
        print_test_result "Script uses correct Windows executable extension" "PASS"
    else
        print_test_result "Script uses correct Windows executable extension" "FAIL"
    fi
}

# Run all tests
main() {
    echo "=========================================="
    echo "Windows Build Script Unit Tests"
    echo "=========================================="
    echo "Testing: $BUILD_SCRIPT"
    
    test_script_exists
    test_help_option
    test_msys2_checking
    test_dependency_checking
    test_msys2_error_reporting
    test_dependency_error_reporting
    test_cmake_options
    test_parallel_build
    test_build_directory
    test_error_handling
    test_argument_parsing
    test_msys2_environment_support
    test_output_location
    
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
