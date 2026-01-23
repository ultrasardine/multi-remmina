# macOS Keychain Implementation Notes

## Overview

This implementation provides a platform abstraction layer for secure password storage on macOS using the native Keychain APIs from the Security framework.

## Implementation Details

### Core Files

1. **src/remmina_keychain_macos.h**
   - Header file defining the Keychain abstraction API
   - Uses GLib types for consistency with Remmina codebase
   - Guarded with `#ifdef __APPLE__` for macOS-only compilation

2. **src/remmina_keychain_macos.c**
   - Implementation using Security framework APIs
   - Functions:
     - `remmina_keychain_macos_init()` - Initialize (currently no-op, returns TRUE)
     - `remmina_keychain_macos_store_password()` - Store password using `SecKeychainAddGenericPassword`
     - `remmina_keychain_macos_get_password()` - Retrieve password using `SecKeychainFindGenericPassword`
     - `remmina_keychain_macos_delete_password()` - Delete password using `SecKeychainItemDelete`
   - Error handling: Converts OSStatus codes to GError
   - Handles duplicate items by deleting and re-adding

### Test Files

1. **tests/keychain/test_keychain_properties.c**
   - Property-based tests with 100 iterations each
   - Property 1: Keychain Password Round-Trip (validates Requirements 2.1, 2.2, 2.3, 2.6)
   - Property 2: Keychain Deletion Completeness (validates Requirements 2.3, 2.5)
   - Uses simple random string generation for test data

2. **tests/keychain/test_keychain_unit.c**
   - Unit tests for edge cases (validates Requirements 2.4, 2.5)
   - Tests:
     - Empty password storage
     - Special characters in passwords
     - Unicode characters in passwords
     - Non-existent password retrieval
     - Delete non-existent password
     - NULL parameter handling
     - Very long password (1000 characters)

3. **tests/keychain/CMakeLists.txt**
   - Build configuration for tests
   - Links Security and CoreFoundation frameworks
   - Adds tests to CTest

### Build System Changes

1. **CMakeLists.txt (root)**
   - Added `add_subdirectory(tests/keychain)` when `WITH_TESTS=ON` and `APPLE`

2. **src/CMakeLists.txt**
   - Added keychain source files to REMMINA_SRCS when `APPLE`
   - Linked Security and CoreFoundation frameworks when `APPLE`

## Design Decisions

### Error Handling

- All functions return GError for consistency with GLib conventions
- OSStatus error codes are mapped to GIO error codes:
  - `errSecItemNotFound` → `G_IO_ERROR_NOT_FOUND`
  - `errSecAuthFailed` → `G_IO_ERROR_PERMISSION_DENIED`
  - Other errors → `G_IO_ERROR_FAILED` with error code in message

### Duplicate Item Handling

When storing a password that already exists:
1. Attempt to add password
2. If `errSecDuplicateItem` is returned:
   - Find the existing item
   - Delete it
   - Retry the add operation
3. This ensures passwords can be updated without manual deletion

### Memory Management

- Uses GLib memory functions (`g_malloc0`, `g_free`) for consistency
- Properly frees Keychain-allocated memory using `SecKeychainItemFreeContent`
- Releases CoreFoundation objects using `CFRelease`

### Testing Strategy

- Property-based tests validate universal properties across random inputs
- Unit tests validate specific edge cases and error conditions
- Tests clean up after themselves by deleting created Keychain entries
- All test entries use service name "RemminaTest" for easy identification

## Next Steps

To integrate with the secret plugin (Task 4):
1. Modify `plugins/secret/src/glibsecret_plugin.c`
2. Add `#ifdef __APPLE__` blocks around libsecret code
3. Call `remmina_keychain_macos_*` functions on macOS
4. Update `plugins/secret/CMakeLists.txt` to link Security framework on macOS

## Testing on macOS

```bash
# Build with tests enabled
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DWITH_TESTS=ON ..
make

# Run all keychain tests
ctest -R Keychain -V

# Or run tests directly
./tests/keychain/test_keychain_properties
./tests/keychain/test_keychain_unit
```

## Requirements Validation

This implementation validates the following requirements:

- **2.1**: Store password in macOS Keychain ✓
- **2.2**: Retrieve password from macOS Keychain ✓
- **2.3**: Delete password from macOS Keychain ✓
- **2.4**: Return descriptive errors on failure ✓
- **2.5**: Return error when password not found ✓
- **2.6**: Round-trip property (store then retrieve returns same value) ✓

All requirements are covered by either property-based tests or unit tests.
