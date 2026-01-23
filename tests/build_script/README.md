# Build Script Tests

This directory contains unit tests for the macOS build script (`scripts/macos-build.sh`).

## Test Coverage

The test suite validates:

1. **Script Existence and Permissions** - Verifies the script exists and is executable
2. **Help Option** - Tests the help/usage documentation
3. **Dependency Checking Logic** - Validates dependency detection and listing
4. **Homebrew Error Reporting** - Tests error messages for missing Homebrew
5. **Dependency Error Reporting** - Tests error handling for missing dependencies
6. **CMake Configuration Options** - Validates macOS-specific CMake flags
7. **Parallel Build Configuration** - Tests CPU core detection and parallel make
8. **Build Directory Management** - Tests directory creation and cleanup
9. **Error Handling** - Validates error detection and exit codes
10. **Command Line Argument Parsing** - Tests option parsing and validation

## Running Tests

```bash
# Run all build script tests
./tests/build_script/test_macos_build_script.sh
```

## Test Requirements

These tests validate Requirements 5.1, 5.2, 5.3, 5.4, and 5.5 from the macOS port specification:
- 5.1: Homebrew dependency installation
- 5.2: GTK3 and library detection
- 5.3: FreeRDP version detection
- 5.4: Missing dependency reporting
- 5.5: Successful build with all dependencies

## Test Results

All tests should pass on macOS systems with the build script properly configured.

Expected output:
```
==========================================
Test Summary
==========================================
Passed: 28
Failed: 0
Total: 28
==========================================
All tests passed!
```
