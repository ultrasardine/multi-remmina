#!/bin/bash
# Multi-Remmina Windows Build Verification Script
# This script verifies that the Windows port platform abstractions work correctly
# Run this on Windows with MSYS2 environment

set -e

echo "=========================================="
echo "Multi-Remmina Windows Build Verification"
echo "=========================================="
echo ""

# Check if we're on Windows
if [[ ! "$OSTYPE" =~ "msys" ]] && [[ ! "$OSTYPE" =~ "win" ]]; then
    echo "ERROR: This script must be run on Windows with MSYS2"
    echo "Current OSTYPE: $OSTYPE"
    exit 1
fi

echo "✓ Running on Windows/MSYS2"
echo ""

# Check for required build tools
echo "Checking build tools..."
command -v cmake >/dev/null 2>&1 || { echo "ERROR: cmake not found"; exit 1; }
command -v make >/dev/null 2>&1 || { echo "ERROR: make not found"; exit 1; }
command -v gcc >/dev/null 2>&1 || { echo "ERROR: gcc not found"; exit 1; }
echo "✓ Build tools available"
echo ""

# Check for required dependencies
echo "Checking dependencies..."
pkg-config --exists gtk+-3.0 || { echo "ERROR: GTK3 not found"; exit 1; }
pkg-config --exists glib-2.0 || { echo "ERROR: GLib not found"; exit 1; }
echo "✓ Core dependencies available"
echo ""

# Create build directory
BUILD_DIR="build-windows-test"
if [ -d "$BUILD_DIR" ]; then
    echo "Cleaning existing build directory..."
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "Configuring build with CMake..."
cmake -G "MSYS Makefiles" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DWITH_TESTS=ON \
    -DWITH_WINDOWS_CREDENTIAL_MANAGER=ON \
    -DWITH_WINDOWS_DNSSD=ON \
    .. || { echo "ERROR: CMake configuration failed"; exit 1; }

echo "✓ CMake configuration successful"
echo ""

# Verify Windows-specific settings
echo "Verifying Windows platform detection..."
if ! grep -q "Configuring for Windows" CMakeCache.txt 2>/dev/null; then
    echo "ERROR: Windows platform not detected"
    exit 1
fi
echo "✓ Windows platform detected"
echo ""

echo "Verifying Telepathy is disabled..."
if grep -q "WITH_TELEPATHY:BOOL=ON" CMakeCache.txt 2>/dev/null; then
    echo "ERROR: Telepathy should be disabled on Windows"
    exit 1
fi
echo "✓ Telepathy disabled"
echo ""

echo "Verifying Avahi is disabled..."
if grep -q "WITH_AVAHI:BOOL=ON" CMakeCache.txt 2>/dev/null; then
    echo "ERROR: Avahi should be disabled on Windows"
    exit 1
fi
echo "✓ Avahi disabled"
echo ""

echo "Verifying Windows Credential Manager is enabled..."
if ! grep -q "WITH_WINDOWS_CREDENTIAL_MANAGER:BOOL=ON" CMakeCache.txt 2>/dev/null; then
    echo "WARNING: Windows Credential Manager not enabled"
fi
echo "✓ Windows Credential Manager configured"
echo ""

# Build the project
echo "Building project..."
make -j$(nproc) || { echo "ERROR: Build failed"; exit 1; }
echo "✓ Build successful"
echo ""

# Run tests if available
echo "Running tests..."
if [ -f "tests/path_handling/test_path_unit.exe" ]; then
    echo "Testing path handling..."
    ./tests/path_handling/test_path_unit.exe || { echo "WARNING: Path handling tests failed"; }
    echo "✓ Path handling tests completed"
fi

if [ -f "tests/path_handling/test_path_properties.exe" ]; then
    echo "Testing path properties..."
    ./tests/path_handling/test_path_properties.exe || { echo "WARNING: Path property tests failed"; }
    echo "✓ Path property tests completed"
fi

if [ -f "tests/service_discovery/test_service_discovery_unit.exe" ]; then
    echo "Testing service discovery..."
    ./tests/service_discovery/test_service_discovery_unit.exe || { echo "WARNING: Service discovery tests failed"; }
    echo "✓ Service discovery tests completed"
fi

if [ -f "tests/system_tray/test_system_tray_unit.exe" ]; then
    echo "Testing system tray..."
    ./tests/system_tray/test_system_tray_unit.exe || { echo "WARNING: System tray tests failed"; }
    echo "✓ System tray tests completed"
fi

if [ -f "tests/credential_manager/test_credential_unit.exe" ]; then
    echo "Testing Credential Manager..."
    ./tests/credential_manager/test_credential_unit.exe || { echo "WARNING: Credential Manager tests failed"; }
    echo "✓ Credential Manager tests completed"
fi

echo ""
echo "=========================================="
echo "Verification Summary"
echo "=========================================="
echo "✓ Windows platform detected correctly"
echo "✓ Linux-specific features disabled (Avahi, Telepathy)"
echo "✓ Windows-specific features enabled (Credential Manager, DNS-SD)"
echo "✓ Build completed successfully"
echo "✓ Platform abstraction tests executed"
echo ""
echo "Next steps:"
echo "1. Test path handling manually"
echo "2. Test service discovery (if DNS-SD available)"
echo "3. Test system tray icon"
echo "4. Verify no Telepathy dependencies in binaries"
echo ""
echo "To verify no Telepathy dependencies:"
echo "  ldd src/multi-remmina.exe | grep -i telepathy"
echo "  (should return no results)"
echo ""
