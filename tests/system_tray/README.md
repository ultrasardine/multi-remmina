# System Tray Integration Tests

This directory contains unit tests for the system tray integration functionality in Remmina.

## Overview

Remmina uses different system tray implementations depending on the platform:
- **Linux**: AppIndicator (libappindicator) for native system tray support
- **macOS**: GTK StatusIcon as a fallback when AppIndicator is not available
- **Other platforms**: GTK StatusIcon fallback

## Test Coverage

### Unit Tests (`test_system_tray_unit.c`)

Tests the StatusIcon implementation used on macOS and as a fallback:

1. **StatusIcon Creation** (Requirement 12.2)
   - Verifies StatusIcon can be created successfully
   - Tests property setting (title, tooltip, visibility)
   - Validates StatusIcon object type

2. **Click Handler** (Requirement 12.3)
   - Tests that activation signal triggers callbacks
   - Verifies click handlers are called correctly
   - Ensures proper signal connection

3. **Fallback Behavior** (Requirement 12.4)
   - Tests graceful handling when StatusIcon is unavailable
   - Verifies application continues without system tray
   - Tests with invalid icon names

4. **Popup Menu**
   - Tests menu creation and attachment
   - Verifies menu items can be added
   - Tests menu display functionality

5. **Visibility Toggle**
   - Tests showing and hiding the status icon
   - Verifies visibility state changes
   - Tests multiple toggle operations

## Building and Running Tests

### Build Tests

```bash
mkdir build && cd build
cmake -DWITH_TESTS=ON ..
make test_system_tray_unit
```

### Run Tests

```bash
# Run all system tray tests
./tests/system_tray/test_system_tray_unit

# Run with verbose output
./tests/system_tray/test_system_tray_unit --verbose

# Run specific test
./tests/system_tray/test_system_tray_unit -p /system_tray/statusicon_creation
```

### Run via CTest

```bash
ctest -R SystemTrayUnit -V
```

## Platform-Specific Behavior

### macOS
- Tests run with StatusIcon implementation
- AppIndicator tests are skipped
- Validates macOS-specific system tray behavior

### Linux
- Tests are skipped if AppIndicator is available
- StatusIcon tests only run in fallback mode
- AppIndicator functionality tested separately

## Requirements Validated

- **12.2**: StatusIcon creation on macOS
- **12.3**: Click handler triggers correct action  
- **12.4**: Fallback when StatusIcon unavailable

## Implementation Notes

- Tests use `gtk_test_init()` for GTK test environment
- Tests are conditional based on `HAVE_LIBAPPINDICATOR` define
- Tests skip gracefully on platforms where they don't apply
- All tests clean up resources properly

## Troubleshooting

### Tests Skip on macOS
- Verify `HAVE_LIBAPPINDICATOR` is not defined
- Check CMake configuration for macOS platform detection
- Ensure GTK3 is properly installed

### Tests Fail
- Check GTK3 version (minimum 3.14.0 required)
- Verify display server is available (X11/Wayland/Quartz)
- Run with `--verbose` flag for detailed output

## Future Enhancements

- Integration tests with actual Remmina icon code
- Tests for menu population and interaction
- Tests for icon state changes
- Performance tests for menu operations
