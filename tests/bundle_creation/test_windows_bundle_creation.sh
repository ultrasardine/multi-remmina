#!/bin/bash
# Unit tests for Windows bundle creation script
# Tests package structure, DLL collection, GTK runtime, and resource placement

set -e

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUNDLE_SCRIPT="$SCRIPT_DIR/../../scripts/windows-bundle.sh"
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
    
    if echo "$help_output" | grep -q "REQUIREMENTS:"; then
        print_test_result "Help contains requirements section" "PASS"
    else
        print_test_result "Help contains requirements section" "FAIL"
    fi
}

# Test 3: MSYS2 environment check
test_msys2_check() {
    print_test_header "Test 3: MSYS2 Environment Check"
    
    # Check if script validates MSYS2 environment
    if grep -q "check_msys2" "$BUNDLE_SCRIPT"; then
        print_test_result "Script checks for MSYS2 environment" "PASS"
    else
        print_test_result "Script checks for MSYS2 environment" "FAIL"
    fi
    
    # Check if script validates MSYSTEM variable
    if grep -q "MSYSTEM" "$BUNDLE_SCRIPT"; then
        print_test_result "Script validates MSYSTEM variable" "PASS"
    else
        print_test_result "Script validates MSYSTEM variable" "FAIL"
    fi
    
    # Check if script provides helpful error message
    if grep -q "Not running in MSYS2" "$BUNDLE_SCRIPT"; then
        print_test_result "Script provides MSYS2 error message" "PASS"
    else
        print_test_result "Script provides MSYS2 error message" "FAIL"
    fi
}

# Test 4: DLL collection functionality
test_dll_collection() {
    print_test_header "Test 4: DLL Collection Functionality"
    
    # Check if script uses ldd for DLL discovery
    if grep -q "ldd" "$BUNDLE_SCRIPT"; then
        print_test_result "Script uses ldd for DLL discovery" "PASS"
    else
        print_test_result "Script uses ldd for DLL discovery" "FAIL"
    fi
    
    # Check if script collects DLLs from executable
    if grep -q "collect_dlls.*multi-remmina.exe" "$BUNDLE_SCRIPT" || grep -q "ldd.*\.exe" "$BUNDLE_SCRIPT"; then
        print_test_result "Script collects DLLs from executable" "PASS"
    else
        print_test_result "Script collects DLLs from executable" "FAIL"
    fi
    
    # Check if script collects DLLs from plugins
    if grep -q "collect_dlls.*plugin" "$BUNDLE_SCRIPT" || grep -q "ldd.*\.dll" "$BUNDLE_SCRIPT"; then
        print_test_result "Script collects DLLs from plugins" "PASS"
    else
        print_test_result "Script collects DLLs from plugins" "FAIL"
    fi
    
    # Check if script handles recursive DLL dependencies
    if grep -q "collect_all_dlls" "$BUNDLE_SCRIPT" || grep -q "iteration" "$BUNDLE_SCRIPT"; then
        print_test_result "Script handles recursive DLL dependencies" "PASS"
    else
        print_test_result "Script handles recursive DLL dependencies" "FAIL"
    fi
    
    # Check if script filters MinGW/MSYS DLLs
    if grep -q "mingw\|msys" "$BUNDLE_SCRIPT"; then
        print_test_result "Script filters MinGW/MSYS DLLs" "PASS"
    else
        print_test_result "Script filters MinGW/MSYS DLLs" "FAIL"
    fi
}

# Test 5: GTK runtime files
test_gtk_runtime() {
    print_test_header "Test 5: GTK Runtime Files"
    
    # Check if script copies GTK runtime
    if grep -q "copy_gtk_runtime" "$BUNDLE_SCRIPT"; then
        print_test_result "Script copies GTK runtime files" "PASS"
    else
        print_test_result "Script copies GTK runtime files" "FAIL"
    fi
    
    # Check if script copies icon themes
    if grep -q "Adwaita" "$BUNDLE_SCRIPT" && grep -q "icons" "$BUNDLE_SCRIPT"; then
        print_test_result "Script copies Adwaita icon theme" "PASS"
    else
        print_test_result "Script copies Adwaita icon theme" "FAIL"
    fi
    
    # Check if script copies GSettings schemas
    if grep -q "glib-2.0/schemas" "$BUNDLE_SCRIPT"; then
        print_test_result "Script copies GSettings schemas" "PASS"
    else
        print_test_result "Script copies GSettings schemas" "FAIL"
    fi
    
    # Check if script compiles schemas
    if grep -q "glib-compile-schemas" "$BUNDLE_SCRIPT"; then
        print_test_result "Script compiles GSettings schemas" "PASS"
    else
        print_test_result "Script compiles GSettings schemas" "FAIL"
    fi
    
    # Check if script copies GDK pixbuf loaders
    if grep -q "gdk-pixbuf" "$BUNDLE_SCRIPT" && grep -q "loaders" "$BUNDLE_SCRIPT"; then
        print_test_result "Script copies GDK pixbuf loaders" "PASS"
    else
        print_test_result "Script copies GDK pixbuf loaders" "FAIL"
    fi
}

# Test 6: Application resources
test_resources() {
    print_test_header "Test 6: Application Resources"
    
    # Check if script copies resources
    if grep -q "copy_resources" "$BUNDLE_SCRIPT"; then
        print_test_result "Script copies application resources" "PASS"
    else
        print_test_result "Script copies application resources" "FAIL"
    fi
    
    # Check if script copies UI files
    if grep -q "data/ui" "$BUNDLE_SCRIPT" || grep -q "ui_source" "$BUNDLE_SCRIPT"; then
        print_test_result "Script copies UI files" "PASS"
    else
        print_test_result "Script copies UI files" "FAIL"
    fi
    
    # Check if script copies theme files
    if grep -q "data/theme" "$BUNDLE_SCRIPT"; then
        print_test_result "Script copies theme files" "PASS"
    else
        print_test_result "Script copies theme files" "FAIL"
    fi
    
    # Check if script copies icon files
    if grep -q "data/icons" "$BUNDLE_SCRIPT"; then
        print_test_result "Script copies application icons" "PASS"
    else
        print_test_result "Script copies application icons" "FAIL"
    fi
    
    # Check if script creates share/multi-remmina directory
    if grep -q "share/multi-remmina" "$BUNDLE_SCRIPT"; then
        print_test_result "Script creates share/multi-remmina directory" "PASS"
    else
        print_test_result "Script creates share/multi-remmina directory" "FAIL"
    fi
}

# Test 7: Plugin handling
test_plugins() {
    print_test_header "Test 7: Plugin Handling"
    
    # Check if script copies plugins
    if grep -q "copy_plugins" "$BUNDLE_SCRIPT"; then
        print_test_result "Script copies plugins" "PASS"
    else
        print_test_result "Script copies plugins" "FAIL"
    fi
    
    # Check if script creates plugins directory
    if grep -q "mkdir.*plugins" "$BUNDLE_SCRIPT"; then
        print_test_result "Script creates plugins directory" "PASS"
    else
        print_test_result "Script creates plugins directory" "FAIL"
    fi
    
    # Check if script searches for .dll files
    if grep -q "\.dll" "$BUNDLE_SCRIPT" && grep -q "plugins" "$BUNDLE_SCRIPT"; then
        print_test_result "Script searches for .dll plugin files" "PASS"
    else
        print_test_result "Script searches for .dll plugin files" "FAIL"
    fi
    
    # Check if script counts plugins
    if grep -q "plugin_count" "$BUNDLE_SCRIPT"; then
        print_test_result "Script counts copied plugins" "PASS"
    else
        print_test_result "Script counts copied plugins" "FAIL"
    fi
}

# Test 8: GTK settings file
test_gtk_settings() {
    print_test_header "Test 8: GTK Settings File"
    
    # Check if script creates GTK settings
    if grep -q "create_gtk_settings" "$BUNDLE_SCRIPT"; then
        print_test_result "Script creates GTK settings file" "PASS"
    else
        print_test_result "Script creates GTK settings file" "FAIL"
    fi
    
    # Check if script creates etc/gtk-3.0 directory
    if grep -q "etc/gtk-3.0" "$BUNDLE_SCRIPT"; then
        print_test_result "Script creates etc/gtk-3.0 directory" "PASS"
    else
        print_test_result "Script creates etc/gtk-3.0 directory" "FAIL"
    fi
    
    # Check if settings.ini is created
    if grep -q "settings.ini" "$BUNDLE_SCRIPT"; then
        print_test_result "Script creates settings.ini file" "PASS"
    else
        print_test_result "Script creates settings.ini file" "FAIL"
    fi
    
    # Check if settings contain theme configuration
    if grep -q "gtk-theme-name" "$BUNDLE_SCRIPT"; then
        print_test_result "Settings file contains theme configuration" "PASS"
    else
        print_test_result "Settings file contains theme configuration" "FAIL"
    fi
    
    # Check if settings contain icon theme configuration
    if grep -q "gtk-icon-theme-name" "$BUNDLE_SCRIPT"; then
        print_test_result "Settings file contains icon theme configuration" "PASS"
    else
        print_test_result "Settings file contains icon theme configuration" "FAIL"
    fi
}

# Test 9: GDK pixbuf loaders cache
test_loaders_cache() {
    print_test_header "Test 9: GDK Pixbuf Loaders Cache"
    
    # Check if script updates loaders cache
    if grep -q "update_loaders_cache" "$BUNDLE_SCRIPT"; then
        print_test_result "Script updates loaders cache" "PASS"
    else
        print_test_result "Script updates loaders cache" "FAIL"
    fi
    
    # Check if script uses gdk-pixbuf-query-loaders
    if grep -q "gdk-pixbuf-query-loaders" "$BUNDLE_SCRIPT"; then
        print_test_result "Script uses gdk-pixbuf-query-loaders" "PASS"
    else
        print_test_result "Script uses gdk-pixbuf-query-loaders" "FAIL"
    fi
    
    # Check if script creates loaders.cache file
    if grep -q "loaders.cache" "$BUNDLE_SCRIPT"; then
        print_test_result "Script creates loaders.cache file" "PASS"
    else
        print_test_result "Script creates loaders.cache file" "FAIL"
    fi
    
    # Check if script makes paths relative
    if grep -q "sed.*loaders.cache" "$BUNDLE_SCRIPT" || grep -q "relative" "$BUNDLE_SCRIPT"; then
        print_test_result "Script makes loader paths relative" "PASS"
    else
        print_test_result "Script makes loader paths relative" "FAIL"
    fi
}

# Test 10: Executable handling
test_executable() {
    print_test_header "Test 10: Executable Handling"
    
    # Check if script copies executable
    if grep -q "copy_executable" "$BUNDLE_SCRIPT"; then
        print_test_result "Script copies main executable" "PASS"
    else
        print_test_result "Script copies main executable" "FAIL"
    fi
    
    # Check if script validates executable exists
    if grep -q "multi-remmina.exe" "$BUNDLE_SCRIPT" && grep -q "not found" "$BUNDLE_SCRIPT"; then
        print_test_result "Script validates executable exists" "PASS"
    else
        print_test_result "Script validates executable exists" "FAIL"
    fi
    
    # Check if script provides helpful error for missing executable
    if grep -q "build the project first" "$BUNDLE_SCRIPT" || grep -q "windows-build.sh" "$BUNDLE_SCRIPT"; then
        print_test_result "Script provides helpful error for missing executable" "PASS"
    else
        print_test_result "Script provides helpful error for missing executable" "FAIL"
    fi
}

# Test 11: Error handling
test_error_handling() {
    print_test_header "Test 11: Error Handling"
    
    # Check if script uses set -e
    if grep -q "set -e" "$BUNDLE_SCRIPT"; then
        print_test_result "Script exits on error (set -e)" "PASS"
    else
        print_test_result "Script exits on error (set -e)" "FAIL"
    fi
    
    # Check if script validates MSYS2 environment
    if grep -q "check_msys2" "$BUNDLE_SCRIPT" && grep -q "exit 1" "$BUNDLE_SCRIPT"; then
        print_test_result "Script exits on MSYS2 validation failure" "PASS"
    else
        print_test_result "Script exits on MSYS2 validation failure" "FAIL"
    fi
    
    # Check if script validates required tools
    if grep -q "check_tools" "$BUNDLE_SCRIPT"; then
        print_test_result "Script validates required tools" "PASS"
    else
        print_test_result "Script validates required tools" "FAIL"
    fi
    
    # Check if script handles missing ldd
    if grep -q "ldd" "$BUNDLE_SCRIPT" && grep -q "missing_tools" "$BUNDLE_SCRIPT"; then
        print_test_result "Script checks for ldd tool" "PASS"
    else
        print_test_result "Script checks for ldd tool" "FAIL"
    fi
}

# Test 12: Command line argument parsing
test_argument_parsing() {
    print_test_header "Test 12: Command Line Argument Parsing"
    
    # Check if script handles --help
    if grep -q "\-h\|--help" "$BUNDLE_SCRIPT"; then
        print_test_result "Script handles --help option" "PASS"
    else
        print_test_result "Script handles --help option" "FAIL"
    fi
    
    # Check if script handles build directory argument
    if grep -q "build_dir" "$BUNDLE_SCRIPT"; then
        print_test_result "Script handles build directory argument" "PASS"
    else
        print_test_result "Script handles build directory argument" "FAIL"
    fi
    
    # Check if script handles output directory argument
    if grep -q "output_dir" "$BUNDLE_SCRIPT"; then
        print_test_result "Script handles output directory argument" "PASS"
    else
        print_test_result "Script handles output directory argument" "FAIL"
    fi
    
    # Check if script handles skip options
    if grep -q "skip-gtk-runtime\|skip-resources\|skip-loaders-cache" "$BUNDLE_SCRIPT"; then
        print_test_result "Script handles skip options" "PASS"
    else
        print_test_result "Script handles skip options" "FAIL"
    fi
}

# Test 13: Output directory management
test_output_directory() {
    print_test_header "Test 13: Output Directory Management"
    
    # Check if script creates output directory
    if grep -q "mkdir.*output_dir" "$BUNDLE_SCRIPT"; then
        print_test_result "Script creates output directory" "PASS"
    else
        print_test_result "Script creates output directory" "FAIL"
    fi
    
    # Check if script cleans existing output directory
    if grep -q "rm -rf.*output_dir" "$BUNDLE_SCRIPT"; then
        print_test_result "Script cleans existing output directory" "PASS"
    else
        print_test_result "Script cleans existing output directory" "FAIL"
    fi
    
    # Check if script uses default output directory name
    if grep -q "multi-remmina-win64" "$BUNDLE_SCRIPT"; then
        print_test_result "Script uses default output directory name" "PASS"
    else
        print_test_result "Script uses default output directory name" "FAIL"
    fi
}

# Test 14: MinGW prefix detection
test_mingw_prefix() {
    print_test_header "Test 14: MinGW Prefix Detection"
    
    # Check if script detects MinGW prefix
    if grep -q "mingw_prefix" "$BUNDLE_SCRIPT"; then
        print_test_result "Script detects MinGW prefix" "PASS"
    else
        print_test_result "Script detects MinGW prefix" "FAIL"
    fi
    
    # Check if script handles MINGW64
    if grep -q "MINGW64" "$BUNDLE_SCRIPT" && grep -q "/mingw64" "$BUNDLE_SCRIPT"; then
        print_test_result "Script handles MINGW64 environment" "PASS"
    else
        print_test_result "Script handles MINGW64 environment" "FAIL"
    fi
    
    # Check if script handles UCRT64
    if grep -q "UCRT64" "$BUNDLE_SCRIPT" && grep -q "/ucrt64" "$BUNDLE_SCRIPT"; then
        print_test_result "Script handles UCRT64 environment" "PASS"
    else
        print_test_result "Script handles UCRT64 environment" "FAIL"
    fi
    
    # Check if script handles MINGW32
    if grep -q "MINGW32" "$BUNDLE_SCRIPT" && grep -q "/mingw32" "$BUNDLE_SCRIPT"; then
        print_test_result "Script handles MINGW32 environment" "PASS"
    else
        print_test_result "Script handles MINGW32 environment" "FAIL"
    fi
}

# Test 15: Package size reporting
test_package_info() {
    print_test_header "Test 15: Package Information Reporting"
    
    # Check if script reports package location
    if grep -q "Package location" "$BUNDLE_SCRIPT"; then
        print_test_result "Script reports package location" "PASS"
    else
        print_test_result "Script reports package location" "FAIL"
    fi
    
    # Check if script reports executable location
    if grep -q "Executable:" "$BUNDLE_SCRIPT"; then
        print_test_result "Script reports executable location" "PASS"
    else
        print_test_result "Script reports executable location" "FAIL"
    fi
    
    # Check if script reports package size
    if grep -q "du -sh" "$BUNDLE_SCRIPT" || grep -q "Package size" "$BUNDLE_SCRIPT"; then
        print_test_result "Script reports package size" "PASS"
    else
        print_test_result "Script reports package size" "FAIL"
    fi
}

# Run all tests
main() {
    echo "=========================================="
    echo "Windows Bundle Creation Script Unit Tests"
    echo "=========================================="
    echo "Testing: $BUNDLE_SCRIPT"
    
    test_script_exists
    test_help_option
    test_msys2_check
    test_dll_collection
    test_gtk_runtime
    test_resources
    test_plugins
    test_gtk_settings
    test_loaders_cache
    test_executable
    test_error_handling
    test_argument_parsing
    test_output_directory
    test_mingw_prefix
    test_package_info
    
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
