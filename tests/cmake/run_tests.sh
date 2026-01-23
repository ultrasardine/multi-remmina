#!/bin/bash
# Remmina CMake Configuration Test Runner
#
# This script runs CMake configuration tests for the macOS port

set -e

echo "=========================================="
echo "Remmina CMake Configuration Tests"
echo "=========================================="
echo ""

# Check if we're on macOS
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo "✓ Running on macOS"
    IS_MACOS=true
else
    echo "ℹ Not running on macOS, some tests will be skipped"
    IS_MACOS=false
fi

echo ""

# Check for CMake
if ! command -v cmake &> /dev/null; then
    echo "✗ CMake not found. Please install CMake to run tests."
    exit 1
fi

echo "✓ CMake found: $(cmake --version | head -n1)"
echo ""

# Create build directory for tests
TEST_BUILD_DIR="build_test_$$"
mkdir -p "$TEST_BUILD_DIR"
cd "$TEST_BUILD_DIR"

echo "Configuring build with tests enabled..."
echo "=========================================="

# Configure with tests enabled
if cmake -DCMAKE_BUILD_TYPE=Debug -DWITH_TESTS=ON .. ; then
    echo ""
    echo "✓ CMake configuration successful"
else
    echo ""
    echo "✗ CMake configuration failed"
    cd ..
    rm -rf "$TEST_BUILD_DIR"
    exit 1
fi

echo ""
echo "Running tests..."
echo "=========================================="

# Run tests
if ctest --output-on-failure; then
    echo ""
    echo "✓ All tests passed"
    TEST_RESULT=0
else
    echo ""
    echo "✗ Some tests failed"
    TEST_RESULT=1
fi

# Cleanup
cd ..
rm -rf "$TEST_BUILD_DIR"

echo ""
echo "=========================================="
if [ $TEST_RESULT -eq 0 ]; then
    echo "✓ Test suite completed successfully"
else
    echo "✗ Test suite completed with failures"
fi
echo "=========================================="

exit $TEST_RESULT
