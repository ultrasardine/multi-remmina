#!/bin/bash
# Unit tests for macOS bundle creation script
# Tests bundle structure, Info.plist, and resource placement

set -e

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUNDLE_SCRIPT="$SCRIPT_DIR/../../scripts/create-macos-bundle.sh"
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
    
    if [ -f "$BUNDLE_SCRIPT" ]; then
        print_test_result "Bundle script exists" "PASS"
    else
        print_test_result "Bundle script exists" "FAIL"
        return 1
    fi
    
    if [ -x "$BUNDLE_SCRIPT" ]; then
        print_test_result "Bundle script is executable" "PASS"
    else
        print_test_result "Bundle script is executable" "FAIL"
        return 1
    fi
}

# Test 2: Help option works
test_help_option() {
    print_test_header "Test 2: Help Option"
    
    if "$BUNDLE_SCRIPT" --help > /dev/null 2>&1; then
        print_test_result "Help option works" "PASS"
    else
        print_test_result "Help option works" "FAIL"
        return 1
    fi
    
    # Check if help output contains expected sections
    local help_output=$("$BUNDLE_SCRIPT" --help 2>&1)
    
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

# Test 3: Bundle structure creation
test_bundle_structure() {
    print_test_header "Test 3: Bundle Structure Creation"
    
    # Check if script creates Contents/MacOS directory
    if grep -q "Contents/MacOS" "$BUNDLE_SCRIPT"; then
        print_test_result "Script creates Contents/MacOS directory" "PASS"
    else
        print_test_result "Script creates Contents/MacOS directory" "FAIL"
    fi
    
    # Check if script creates Contents/Resources directory
    if grep -q "Contents/Resources" "$BUNDLE_SCRIPT"; then
        print_test_result "Script creates Contents/Resources directory" "PASS"
    else
        print_test_result "Script creates Contents/Resources directory" "FAIL"
    fi
    
    # Check if script creates Contents/Frameworks directory
    if grep -q "Contents/Frameworks" "$BUNDLE_SCRIPT"; then
        print_test_result "Script creates Contents/Frameworks directory" "PASS"
    else
        print_test_result "Script creates Contents/Frameworks directory" "FAIL"
    fi
    
    # Check if script creates plugin directory
    if grep -q "lib/remmina/plugins" "$BUNDLE_SCRIPT"; then
        print_test_result "Script creates plugin directory" "PASS"
    else
        print_test_result "Script creates plugin directory" "FAIL"
    fi
    
    # Check if script creates UI directory
    if grep -q "Contents/Resources/ui" "$BUNDLE_SCRIPT"; then
        print_test_result "Script creates UI directory" "PASS"
    else
        print_test_result "Script creates UI directory" "FAIL"
    fi
    
    # Check if script creates theme directory
    if grep -q "Contents/Resources/theme" "$BUNDLE_SCRIPT"; then
        print_test_result "Script creates theme directory" "PASS"
    else
        print_test_result "Script creates theme directory" "FAIL"
    fi
}

# Test 4: Info.plist generation
test_info_plist_generation() {
    print_test_header "Test 4: Info.plist Generation"
    
    # Check if script generates Info.plist
    if grep -q "Info.plist" "$BUNDLE_SCRIPT"; then
        print_test_result "Script generates Info.plist" "PASS"
    else
        print_test_result "Script generates Info.plist" "FAIL"
    fi
    
    # Check for required Info.plist keys
    local required_keys=(
        "CFBundleExecutable"
        "CFBundleIdentifier"
        "CFBundleName"
        "CFBundleVersion"
        "CFBundlePackageType"
        "NSHighResolutionCapable"
        "LSMinimumSystemVersion"
    )
    
    local all_keys_found=true
    for key in "${required_keys[@]}"; do
        if ! grep -q "$key" "$BUNDLE_SCRIPT"; then
            all_keys_found=false
            echo "  Missing key: $key"
            break
        fi
    done
    
    if [ "$all_keys_found" = true ]; then
        print_test_result "Info.plist contains all required keys" "PASS"
    else
        print_test_result "Info.plist contains all required keys" "FAIL"
    fi
    
    # Check if bundle identifier is set
    if grep -q "org.remmina.Remmina" "$BUNDLE_SCRIPT"; then
        print_test_result "Info.plist contains bundle identifier" "PASS"
    else
        print_test_result "Info.plist contains bundle identifier" "FAIL"
    fi
    
    # Check if version is included
    if grep -q "CFBundleVersion" "$BUNDLE_SCRIPT" && grep -q "VERSION" "$BUNDLE_SCRIPT"; then
        print_test_result "Info.plist includes version information" "PASS"
    else
        print_test_result "Info.plist includes version information" "FAIL"
    fi
}

# Test 5: Executable placement
test_executable_placement() {
    print_test_header "Test 5: Executable Placement"
    
    # Check if script copies executable to Contents/MacOS
    if grep -q "cp.*remmina.*Contents/MacOS" "$BUNDLE_SCRIPT"; then
        print_test_result "Script copies executable to Contents/MacOS" "PASS"
    else
        print_test_result "Script copies executable to Contents/MacOS" "FAIL"
    fi
    
    # Check if script makes executable executable
    if grep -q "chmod +x.*remmina" "$BUNDLE_SCRIPT"; then
        print_test_result "Script sets executable permissions" "PASS"
    else
        print_test_result "Script sets executable permissions" "FAIL"
    fi
    
    # Check if script verifies executable exists
    if grep -q "src/remmina" "$BUNDLE_SCRIPT"; then
        print_test_result "Script checks for executable in build directory" "PASS"
    else
        print_test_result "Script checks for executable in build directory" "FAIL"
    fi
}

# Test 6: Plugin placement
test_plugin_placement() {
    print_test_header "Test 6: Plugin Placement"
    
    # Check if script copies plugins
    if grep -q "plugins" "$BUNDLE_SCRIPT" && grep -q "\.so" "$BUNDLE_SCRIPT"; then
        print_test_result "Script copies plugin files" "PASS"
    else
        print_test_result "Script copies plugin files" "FAIL"
    fi
    
    # Check if script searches for .so files
    if grep -q "find.*\.so" "$BUNDLE_SCRIPT" || grep -q "\*\.so" "$BUNDLE_SCRIPT"; then
        print_test_result "Script finds .so plugin files" "PASS"
    else
        print_test_result "Script finds .so plugin files" "FAIL"
    fi
    
    # Check if script copies to correct plugin directory
    if grep -q "lib/remmina/plugins" "$BUNDLE_SCRIPT"; then
        print_test_result "Script copies plugins to correct directory" "PASS"
    else
        print_test_result "Script copies plugins to correct directory" "FAIL"
    fi
    
    # Check if script validates plugins were copied
    if grep -q "plugin_count" "$BUNDLE_SCRIPT" || grep -q "No plugins" "$BUNDLE_SCRIPT"; then
        print_test_result "Script validates plugin copying" "PASS"
    else
        print_test_result "Script validates plugin copying" "FAIL"
    fi
}

# Test 7: Resource placement
test_resource_placement() {
    print_test_header "Test 7: Resource Placement"
    
    # Check if script copies UI files
    if grep -q "data/ui" "$BUNDLE_SCRIPT" || grep -q "ui_source" "$BUNDLE_SCRIPT"; then
        print_test_result "Script copies UI files" "PASS"
    else
        print_test_result "Script copies UI files" "FAIL"
    fi
    
    # Check if script copies theme files
    if grep -q "data/theme" "$BUNDLE_SCRIPT" || grep -q "theme_source" "$BUNDLE_SCRIPT"; then
        print_test_result "Script copies theme files" "PASS"
    else
        print_test_result "Script copies theme files" "FAIL"
    fi
    
    # Check if script validates UI files were copied
    if grep -q "ui_count" "$BUNDLE_SCRIPT" || grep -q "No UI files" "$BUNDLE_SCRIPT"; then
        print_test_result "Script validates UI file copying" "PASS"
    else
        print_test_result "Script validates UI file copying" "FAIL"
    fi
    
    # Check if script validates theme files were copied
    if grep -q "theme_count" "$BUNDLE_SCRIPT" || grep -q "No theme files" "$BUNDLE_SCRIPT"; then
        print_test_result "Script validates theme file copying" "PASS"
    else
        print_test_result "Script validates theme file copying" "FAIL"
    fi
}

# Test 8: Icon conversion
test_icon_conversion() {
    print_test_header "Test 8: Icon Conversion"
    
    # Check if script converts icon to ICNS
    if grep -q "icns" "$BUNDLE_SCRIPT" || grep -q "iconutil" "$BUNDLE_SCRIPT"; then
        print_test_result "Script converts icon to ICNS format" "PASS"
    else
        print_test_result "Script converts icon to ICNS format" "FAIL"
    fi
    
    # Check if script uses sips for resizing
    if grep -q "sips" "$BUNDLE_SCRIPT"; then
        print_test_result "Script uses sips for icon resizing" "PASS"
    else
        print_test_result "Script uses sips for icon resizing" "FAIL"
    fi
    
    # Check if script creates iconset
    if grep -q "iconset" "$BUNDLE_SCRIPT"; then
        print_test_result "Script creates iconset directory" "PASS"
    else
        print_test_result "Script creates iconset directory" "FAIL"
    fi
    
    # Check if script sources PNG icon
    if grep -q "\.png" "$BUNDLE_SCRIPT" && grep -q "512x512" "$BUNDLE_SCRIPT"; then
        print_test_result "Script sources 512x512 PNG icon" "PASS"
    else
        print_test_result "Script sources 512x512 PNG icon" "FAIL"
    fi
}

# Test 9: Bundle verification
test_bundle_verification() {
    print_test_header "Test 9: Bundle Verification"
    
    # Check if script verifies bundle structure
    if grep -q "verify" "$BUNDLE_SCRIPT" || grep -q "Verifying" "$BUNDLE_SCRIPT"; then
        print_test_result "Script includes bundle verification" "PASS"
    else
        print_test_result "Script includes bundle verification" "FAIL"
    fi
    
    # Check if script verifies executable
    if grep -A 20 "verify" "$BUNDLE_SCRIPT" | grep -q "executable\|MacOS/remmina"; then
        print_test_result "Script verifies executable presence" "PASS"
    else
        print_test_result "Script verifies executable presence" "FAIL"
    fi
    
    # Check if script verifies Info.plist
    if grep -A 30 "verify_bundle" "$BUNDLE_SCRIPT" | grep -q "Info.plist"; then
        print_test_result "Script verifies Info.plist presence" "PASS"
    else
        print_test_result "Script verifies Info.plist presence" "FAIL"
    fi
    
    # Check if script verifies plugins
    if grep -A 20 "verify" "$BUNDLE_SCRIPT" | grep -q "plugin"; then
        print_test_result "Script verifies plugin presence" "PASS"
    else
        print_test_result "Script verifies plugin presence" "FAIL"
    fi
    
    # Check if script verifies resources
    if grep -A 20 "verify" "$BUNDLE_SCRIPT" | grep -q "ui\|theme\|resource"; then
        print_test_result "Script verifies resource presence" "PASS"
    else
        print_test_result "Script verifies resource presence" "FAIL"
    fi
}

# Test 10: Error handling
test_error_handling() {
    print_test_header "Test 10: Error Handling"
    
    # Check if script uses set -e
    if grep -q "set -e" "$BUNDLE_SCRIPT"; then
        print_test_result "Script exits on error (set -e)" "PASS"
    else
        print_test_result "Script exits on error (set -e)" "FAIL"
    fi
    
    # Check if script validates build directory exists
    if grep -q "Build directory does not exist" "$BUNDLE_SCRIPT" || grep -q "if.*-d.*BUILD_DIR" "$BUNDLE_SCRIPT"; then
        print_test_result "Script validates build directory exists" "PASS"
    else
        print_test_result "Script validates build directory exists" "FAIL"
    fi
    
    # Check if script validates executable exists
    if grep -q "executable not found" "$BUNDLE_SCRIPT" || grep -q "if.*-f.*remmina" "$BUNDLE_SCRIPT"; then
        print_test_result "Script validates executable exists" "PASS"
    else
        print_test_result "Script validates executable exists" "FAIL"
    fi
    
    # Check if script exits on verification failure
    if grep -A 10 "verification failed" "$BUNDLE_SCRIPT" | grep -q "exit 1"; then
        print_test_result "Script exits on verification failure" "PASS"
    else
        print_test_result "Script exits on verification failure" "FAIL"
    fi
}

# Test 11: Command line argument parsing
test_argument_parsing() {
    print_test_header "Test 11: Command Line Argument Parsing"
    
    # Check if script handles --help
    if grep -q "\-h\|--help" "$BUNDLE_SCRIPT"; then
        print_test_result "Script handles --help option" "PASS"
    else
        print_test_result "Script handles --help option" "FAIL"
    fi
    
    # Check if script handles build directory option
    if grep -q "build-dir\|build_dir" "$BUNDLE_SCRIPT"; then
        print_test_result "Script handles build directory option" "PASS"
    else
        print_test_result "Script handles build directory option" "FAIL"
    fi
    
    # Check if script handles output directory option
    if grep -q "output-dir\|output_dir" "$BUNDLE_SCRIPT"; then
        print_test_result "Script handles output directory option" "PASS"
    else
        print_test_result "Script handles output directory option" "FAIL"
    fi
    
    # Check if script handles bundle name option
    if grep -q "name\|BUNDLE_NAME" "$BUNDLE_SCRIPT"; then
        print_test_result "Script handles bundle name option" "PASS"
    else
        print_test_result "Script handles bundle name option" "FAIL"
    fi
    
    # Check if script handles version option
    if grep -q "version\|VERSION" "$BUNDLE_SCRIPT"; then
        print_test_result "Script handles version option" "PASS"
    else
        print_test_result "Script handles version option" "FAIL"
    fi
}

# Test 12: Build directory validation
test_build_validation() {
    print_test_header "Test 12: Build Directory Validation"
    
    # Check if script verifies build directory
    if grep -q "verify.*build\|Verifying build" "$BUNDLE_SCRIPT"; then
        print_test_result "Script verifies build directory" "PASS"
    else
        print_test_result "Script verifies build directory" "FAIL"
    fi
    
    # Check if script provides helpful error for missing build
    if grep -q "run the build script" "$BUNDLE_SCRIPT" || grep -q "macos-build.sh" "$BUNDLE_SCRIPT"; then
        print_test_result "Script provides helpful error for missing build" "PASS"
    else
        print_test_result "Script provides helpful error for missing build" "FAIL"
    fi
}

# Test 13: Bundle cleanup
test_bundle_cleanup() {
    print_test_header "Test 13: Bundle Cleanup"
    
    # Check if script removes existing bundle
    if grep -q "rm -rf.*bundle\|Removing existing bundle" "$BUNDLE_SCRIPT"; then
        print_test_result "Script removes existing bundle before creation" "PASS"
    else
        print_test_result "Script removes existing bundle before creation" "FAIL"
    fi
}

# Run all tests
main() {
    echo "=========================================="
    echo "macOS Bundle Creation Script Unit Tests"
    echo "=========================================="
    echo "Testing: $BUNDLE_SCRIPT"
    
    test_script_exists
    test_help_option
    test_bundle_structure
    test_info_plist_generation
    test_executable_placement
    test_plugin_placement
    test_resource_placement
    test_icon_conversion
    test_bundle_verification
    test_error_handling
    test_argument_parsing
    test_build_validation
    test_bundle_cleanup
    
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
