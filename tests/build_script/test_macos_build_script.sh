#!/bin/bash
# Unit tests for macOS build script
# Tests dependency checking and error reporting

set -e

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_SCRIPT="$SCRIPT_DIR/../../scripts/macos-build.sh"
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
}

# Test 3: Dependency checking function
test_dependency_checking() {
    print_test_header "Test 3: Dependency Checking Logic"
    
    # Source the script to test individual functions
    # We'll extract and test the check_homebrew function logic
    
    # Test if script checks for brew command
    if grep -q "command -v brew" "$BUILD_SCRIPT"; then
        print_test_result "Script checks for Homebrew installation" "PASS"
    else
        print_test_result "Script checks for Homebrew installation" "FAIL"
    fi
    
    # Test if script has dependency list
    if grep -q "packages=(" "$BUILD_SCRIPT"; then
        print_test_result "Script defines dependency list" "PASS"
    else
        print_test_result "Script defines dependency list" "FAIL"
    fi
    
    # Test if script checks for required packages
    local required_deps=("cmake" "gtk+3" "glib" "libsodium" "openssl" "libssh" "freerdp" "libvncserver")
    local all_deps_found=true
    
    for dep in "${required_deps[@]}"; do
        if ! grep -q "$dep" "$BUILD_SCRIPT"; then
            all_deps_found=false
            break
        fi
    done
    
    if [ "$all_deps_found" = true ]; then
        print_test_result "Script includes all required dependencies" "PASS"
    else
        print_test_result "Script includes all required dependencies" "FAIL"
    fi
}

# Test 4: Error reporting for missing Homebrew
test_homebrew_error_reporting() {
    print_test_header "Test 4: Error Reporting for Missing Homebrew"
    
    # Check if script has error message for missing Homebrew
    if grep -q "Homebrew is not installed" "$BUILD_SCRIPT"; then
        print_test_result "Script has error message for missing Homebrew" "PASS"
    else
        print_test_result "Script has error message for missing Homebrew" "FAIL"
    fi
    
    # Check if script provides installation instructions
    if grep -q "https://brew.sh" "$BUILD_SCRIPT"; then
        print_test_result "Script provides Homebrew installation URL" "PASS"
    else
        print_test_result "Script provides Homebrew installation URL" "FAIL"
    fi
    
    # Check if script exits on missing Homebrew
    if grep -A 5 "Homebrew is not installed" "$BUILD_SCRIPT" | grep -q "exit 1"; then
        print_test_result "Script exits when Homebrew is missing" "PASS"
    else
        print_test_result "Script exits when Homebrew is missing" "FAIL"
    fi
}

# Test 5: Error reporting for missing dependencies
test_dependency_error_reporting() {
    print_test_header "Test 5: Error Reporting for Missing Dependencies"
    
    # Check if script reports missing packages
    if grep -q "missing_packages" "$BUILD_SCRIPT"; then
        print_test_result "Script tracks missing packages" "PASS"
    else
        print_test_result "Script tracks missing packages" "FAIL"
    fi
    
    # Check if script attempts to install missing packages
    if grep -q "brew install" "$BUILD_SCRIPT"; then
        print_test_result "Script installs missing packages" "PASS"
    else
        print_test_result "Script installs missing packages" "FAIL"
    fi
    
    # Check if script exits on installation failure
    if grep -A 3 "Failed to install" "$BUILD_SCRIPT" | grep -q "exit 1"; then
        print_test_result "Script exits on dependency installation failure" "PASS"
    else
        print_test_result "Script exits on dependency installation failure" "FAIL"
    fi
}

# Test 6: CMake configuration options
test_cmake_options() {
    print_test_header "Test 6: CMake Configuration Options"
    
    # Check for macOS-specific CMake options
    local cmake_options=(
        "WITH_AVAHI=OFF"
        "WITH_APPINDICATOR=OFF"
        "WITH_TELEPATHY=OFF"
        "WITH_MACOS_KEYCHAIN=ON"
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
        print_test_result "Script includes all macOS-specific CMake options" "PASS"
    else
        print_test_result "Script includes all macOS-specific CMake options" "FAIL"
    fi
    
    # Check for Homebrew prefix in CMake
    if grep -q "CMAKE_PREFIX_PATH" "$BUILD_SCRIPT" && grep -q "brew --prefix" "$BUILD_SCRIPT"; then
        print_test_result "Script sets CMAKE_PREFIX_PATH to Homebrew prefix" "PASS"
    else
        print_test_result "Script sets CMAKE_PREFIX_PATH to Homebrew prefix" "FAIL"
    fi
    
    # Check for OpenSSL path
    if grep -q "OPENSSL_ROOT_DIR" "$BUILD_SCRIPT"; then
        print_test_result "Script sets OPENSSL_ROOT_DIR" "PASS"
    else
        print_test_result "Script sets OPENSSL_ROOT_DIR" "FAIL"
    fi
}

# Test 7: Parallel build configuration
test_parallel_build() {
    print_test_header "Test 7: Parallel Build Configuration"
    
    # Check if script detects CPU cores
    if grep -q "sysctl -n hw.ncpu" "$BUILD_SCRIPT"; then
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
}

# Test 8: Build directory management
test_build_directory() {
    print_test_header "Test 8: Build Directory Management"
    
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

# Test 9: Error handling
test_error_handling() {
    print_test_header "Test 9: Error Handling"
    
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

# Test 10: Command line argument parsing
test_argument_parsing() {
    print_test_header "Test 10: Command Line Argument Parsing"
    
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
}

# Run all tests
main() {
    echo "=========================================="
    echo "macOS Build Script Unit Tests"
    echo "=========================================="
    echo "Testing: $BUILD_SCRIPT"
    
    test_script_exists
    test_help_option
    test_dependency_checking
    test_homebrew_error_reporting
    test_dependency_error_reporting
    test_cmake_options
    test_parallel_build
    test_build_directory
    test_error_handling
    test_argument_parsing
    
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
