#!/bin/bash
# Validation script for macOS CMake configuration
# This script validates that the CMakeLists.txt has been properly configured for macOS

set -e

echo "=========================================="
echo "Validating macOS CMake Configuration"
echo "=========================================="
echo ""

CMAKELISTS="CMakeLists.txt"
ERRORS=0

# Check 1: Platform detection for Darwin
echo "Checking platform detection..."
if grep -q "if(APPLE)" "$CMAKELISTS"; then
    echo "✓ Found APPLE platform detection"
else
    echo "✗ Missing APPLE platform detection"
    ERRORS=$((ERRORS + 1))
fi

# Check 2: macOS-specific RPATH configuration
echo "Checking RPATH configuration..."
if grep -q "CMAKE_MACOSX_RPATH" "$CMAKELISTS"; then
    echo "✓ Found CMAKE_MACOSX_RPATH configuration"
else
    echo "✗ Missing CMAKE_MACOSX_RPATH configuration"
    ERRORS=$((ERRORS + 1))
fi

if grep -q "@executable_path" "$CMAKELISTS"; then
    echo "✓ Found @executable_path in RPATH"
else
    echo "✗ Missing @executable_path in RPATH"
    ERRORS=$((ERRORS + 1))
fi

# Check 3: Homebrew path search
echo "Checking Homebrew path configuration..."
if grep -q "/opt/homebrew" "$CMAKELISTS"; then
    echo "✓ Found /opt/homebrew path (Apple Silicon)"
else
    echo "✗ Missing /opt/homebrew path"
    ERRORS=$((ERRORS + 1))
fi

if grep -q "/usr/local" "$CMAKELISTS"; then
    echo "✓ Found /usr/local path (Intel)"
else
    echo "✗ Missing /usr/local path"
    ERRORS=$((ERRORS + 1))
fi

# Check 4: Linux-specific features disabled
echo "Checking Linux-specific feature flags..."
if grep -q "WITH_AVAHI OFF" "$CMAKELISTS"; then
    echo "✓ Found WITH_AVAHI OFF for macOS"
else
    echo "✗ Missing WITH_AVAHI OFF configuration"
    ERRORS=$((ERRORS + 1))
fi

if grep -q "WITH_APPINDICATOR OFF" "$CMAKELISTS"; then
    echo "✓ Found WITH_APPINDICATOR OFF for macOS"
else
    echo "✗ Missing WITH_APPINDICATOR OFF configuration"
    ERRORS=$((ERRORS + 1))
fi

if grep -q "WITH_TELEPATHY OFF" "$CMAKELISTS"; then
    echo "✓ Found WITH_TELEPATHY OFF for macOS"
else
    echo "✗ Missing WITH_TELEPATHY OFF configuration"
    ERRORS=$((ERRORS + 1))
fi

# Check 5: macOS frameworks
echo "Checking macOS framework configuration..."
if grep -q "Security" "$CMAKELISTS"; then
    echo "✓ Found Security framework configuration"
else
    echo "✗ Missing Security framework configuration"
    ERRORS=$((ERRORS + 1))
fi

if grep -q "Cocoa" "$CMAKELISTS"; then
    echo "✓ Found Cocoa framework configuration"
else
    echo "✗ Missing Cocoa framework configuration"
    ERRORS=$((ERRORS + 1))
fi

# Check 6: WITH_MACOS_KEYCHAIN option
echo "Checking macOS Keychain option..."
if grep -q "WITH_MACOS_KEYCHAIN" "$CMAKELISTS"; then
    echo "✓ Found WITH_MACOS_KEYCHAIN option"
else
    echo "✗ Missing WITH_MACOS_KEYCHAIN option"
    ERRORS=$((ERRORS + 1))
fi

echo ""
echo "=========================================="
if [ $ERRORS -eq 0 ]; then
    echo "✓ All validation checks passed"
    echo "=========================================="
    exit 0
else
    echo "✗ $ERRORS validation check(s) failed"
    echo "=========================================="
    exit 1
fi
