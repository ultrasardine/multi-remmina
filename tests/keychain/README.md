# Keychain Tests

This directory contains property-based and unit tests for the macOS Keychain abstraction layer.

## Test Files

- `test_keychain_properties.c` - Property-based tests (100 iterations each)
  - Property 1: Keychain Password Round-Trip
  - Property 2: Keychain Deletion Completeness

- `test_keychain_unit.c` - Unit tests for edge cases
  - Empty password storage
  - Special characters in passwords
  - Unicode characters in passwords
  - Non-existent password retrieval
  - Delete non-existent password
  - NULL parameter handling
  - Very long password (1000 characters)

## Building Tests (macOS only)

From the project root directory:

```bash
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make test_keychain_properties test_keychain_unit
```

## Running Tests

### Run all keychain tests via CTest:
```bash
cd build
ctest -R Keychain -V
```

### Run property-based tests directly:
```bash
cd build
./tests/keychain/test_keychain_properties
```

### Run unit tests directly:
```bash
cd build
./tests/keychain/test_keychain_unit
```

## Test Requirements

- macOS 10.15 or later
- GLib 2.0
- Security framework (automatically linked on macOS)
- CoreFoundation framework (automatically linked on macOS)

## Notes

- These tests will create and delete entries in your macOS Keychain
- All test entries use the service name "RemminaTest" for easy identification
- Tests clean up after themselves by deleting created entries
- Property-based tests run 100 iterations with random data
- Tests are only compiled and run on macOS (guarded by `#ifdef __APPLE__`)

## Troubleshooting

If tests fail with Keychain access errors:
1. Check that the test executable has permission to access the Keychain
2. You may need to approve Keychain access in System Preferences
3. Ensure no other process is locking the Keychain

If tests fail to compile:
1. Ensure you're building on macOS
2. Check that GLib development files are installed: `brew install glib`
3. Verify CMake found the Security framework
