# Profile Management Tests

This directory contains tests for Remmina connection profile management functionality, validating profile creation, persistence, and listing operations.

## Test Files

### test_profile_unit.c
Unit tests for profile management operations:
- **test_profile_creation**: Verifies profiles can be created with all parameters (name, protocol, server, username, resolution)
- **test_config_directory_creation**: Ensures config directory is created if needed with proper permissions
- **test_profile_save_failure**: Tests graceful handling of save failures (invalid paths)
- **test_profile_load_failure**: Tests graceful handling of load failures (non-existent files)
- **test_profile_save_load**: Validates complete save/load round-trip with data verification

### test_profile_properties.c
Property-based tests for profile management:
- **Property 5: Connection Profile Persistence Round-Trip** (100 iterations)
  - Validates: Requirements 7.1, 7.2, 7.3, 7.4
  - Generates random profile data (name, protocol, server, username, password, group, resolution)
  - Saves profile to disk
  - Loads profile from disk
  - Verifies all fields match original values
  
- **Property 6: Profile Listing Completeness** (100 iterations)
  - Validates: Requirement 7.6
  - Creates random set of 3-10 profiles
  - Lists all profiles in data directory
  - Verifies all created profiles appear in listing

## Building Tests

Tests are built when `WITH_TESTS=ON` is set during CMake configuration:

```bash
mkdir build && cd build
cmake -DWITH_TESTS=ON ..
make
```

## Running Tests

### Run all profile management tests:
```bash
ctest -R profile
```

### Run unit tests only:
```bash
./tests/profile_management/test_profile_unit
```

### Run property-based tests only:
```bash
./tests/profile_management/test_profile_properties
```

## Test Environment

Tests create a temporary directory for profile storage to avoid interfering with actual user profiles. The directory is cleaned up after tests complete.

## Platform Support

These tests work on both macOS and Linux:
- On macOS: Uses Keychain for password storage (tested separately in keychain tests)
- On Linux: Uses libsecret for password storage (tested separately in secret plugin tests)

## Requirements Validated

- **7.1**: Profile creation with all parameters
- **7.2**: Profile persistence to disk
- **7.3**: Password storage in secure storage (Keychain on macOS)
- **7.4**: Profile loading from disk
- **7.5**: Config directory creation with proper permissions
- **7.6**: Profile listing completeness

## Notes

- Password verification is handled by platform-specific tests (Keychain tests on macOS, secret plugin tests on Linux)
- Tests use random data generation to ensure comprehensive coverage
- Property-based tests run 100 iterations each to validate correctness across diverse inputs
