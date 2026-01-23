# CMake Configuration Tests

This directory contains unit tests for CMake build configuration, specifically for the macOS port.

## Test Coverage

### test_platform_detection.cmake
- **Validates**: Requirements 1.1
- **Purpose**: Verifies that CMake correctly detects macOS platform (Darwin)
- **Checks**:
  - `APPLE` variable is set to TRUE on macOS
  - `CMAKE_SYSTEM_NAME` is set to "Darwin"

### test_macos_features.cmake
- **Validates**: Requirements 1.2
- **Purpose**: Verifies that Linux-specific features are disabled on macOS
- **Checks**:
  - `WITH_AVAHI` is OFF
  - `WITH_APPINDICATOR` is OFF
  - `WITH_TELEPATHY` is OFF

### test_macos_frameworks.cmake
- **Validates**: Requirements 1.6
- **Purpose**: Verifies that macOS frameworks are found and linked
- **Checks**:
  - Security framework is found when `WITH_MACOS_KEYCHAIN=ON`
  - Cocoa framework is found
  - Framework paths are valid

### test_macos_rpath.cmake
- **Validates**: Requirements 1.4
- **Purpose**: Verifies RPATH configuration for application bundles
- **Checks**:
  - `CMAKE_MACOSX_RPATH` is ON
  - `CMAKE_INSTALL_RPATH` contains bundle-relative paths
  - RPATH uses `@executable_path` for bundle compatibility

## Running Tests

### On macOS

```bash
# Configure the build with tests enabled
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DWITH_TESTS=ON ..

# Run all CMake configuration tests
ctest -R "test_macos" --verbose

# Run specific test
ctest -R "test_macos_platform_detection" --verbose
```

### On Linux

The macOS-specific tests will be skipped automatically on Linux platforms.

```bash
# Configure and run tests
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DWITH_TESTS=ON ..
ctest --verbose
```

## Test Results

All tests should pass on macOS when:
- CMake correctly detects the Darwin platform
- Linux-specific features are disabled
- macOS frameworks (Security, Cocoa) are available
- RPATH is configured for application bundles

## Adding New Tests

To add new CMake configuration tests:

1. Create a new test script in this directory (e.g., `test_new_feature.cmake`)
2. Add the test to `CMakeLists.txt` using `add_test()`
3. Document the test in this README
4. Reference the requirement(s) being validated

## Notes

- These tests run during the CMake configuration phase
- Tests use CMake script mode (`-P` flag)
- Tests are platform-aware and skip when not applicable
- All tests should be idempotent and not modify the build system
