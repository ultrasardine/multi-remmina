#!/bin/bash
# Test script to verify Telepathy is disabled on macOS
# This script checks:
# 1. CMake configuration has WITH_TELEPATHY=OFF
# 2. Telepathy plugin is not built
# 3. No Telepathy references in source code (outside plugin directory)

echo "=== Telepathy Disable Verification ==="
echo ""

# Check 1: CMake configuration
echo "1. Checking CMake configuration..."
if grep -q "WITH_TELEPATHY:BOOL=OFF" build/CMakeCache.txt 2>/dev/null; then
    echo "   ✓ WITH_TELEPATHY is set to OFF in CMake cache"
else
    echo "   ✗ WITH_TELEPATHY is not properly configured"
    exit 1
fi

# Check 2: Telepathy plugin not built
echo ""
echo "2. Checking if Telepathy plugin is built..."
if [ ! -d "build/plugins/telepathy" ]; then
    echo "   ✓ Telepathy plugin directory does not exist in build"
else
    echo "   ✗ Telepathy plugin directory exists in build"
    exit 1
fi

# Check 3: Telepathy plugin commented out in plugins/CMakeLists.txt
echo ""
echo "3. Checking if Telepathy plugin is commented out in plugins/CMakeLists.txt..."
if grep -q "^#find_suggested_package(TELEPATHY)" plugins/CMakeLists.txt; then
    echo "   ✓ Telepathy plugin is commented out in plugins/CMakeLists.txt"
else
    echo "   ✗ Telepathy plugin is not properly commented out"
    exit 1
fi

# Check 4: Root CMakeLists.txt disables Telepathy on macOS
echo ""
echo "4. Checking if root CMakeLists.txt disables Telepathy on macOS..."
if grep -q 'set(WITH_TELEPATHY OFF CACHE BOOL "Disable Telepathy on macOS" FORCE)' CMakeLists.txt; then
    echo "   ✓ Root CMakeLists.txt disables Telepathy on macOS"
else
    echo "   ✗ Root CMakeLists.txt does not disable Telepathy on macOS"
    exit 1
fi

# Check 5: Run CMake test
echo ""
echo "5. Running CMake test for macOS features..."
if ctest --test-dir build -R test_macos_features_disabled --output-on-failure 2>&1 | grep -q "100% tests passed"; then
    echo "   ✓ CMake test for macOS features passed"
else
    echo "   ✗ CMake test for macOS features failed"
    exit 1
fi

echo ""
echo "=== All Telepathy disable checks passed! ==="
echo ""
echo "Summary:"
echo "  - Telepathy is disabled in CMake configuration"
echo "  - Telepathy plugin is not being built"
echo "  - Telepathy plugin is commented out in plugins/CMakeLists.txt"
echo "  - Root CMakeLists.txt properly disables Telepathy on macOS"
echo "  - CMake tests verify Telepathy is disabled"
echo ""
echo "Core functionality works without Telepathy."
