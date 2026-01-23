# Application Launch Tests

This directory contains unit tests for Remmina's application launch and desktop integration functionality.

## Overview

These tests verify that the Remmina application launches correctly on macOS and handles desktop integration appropriately. The tests cover:

- Main window display on launch
- Keyboard shortcuts functionality
- Graceful application quit
- Window state management
- macOS-specific application menu integration
- MIME type registration (disabled on macOS)

## Requirements Tested

- **Requirement 8.1**: Application launches and displays main window
- **Requirement 8.2**: Keyboard shortcuts respond correctly
- **Requirement 8.5**: Application quits gracefully

## Test Files

- `test_application_launch_unit.c` - Unit tests for application launch functionality
- `CMakeLists.txt` - Build configuration for tests

## Building and Running Tests

### Prerequisites

- GTK3 development libraries
- GLib development libraries
- CMake 3.10 or higher

### Build Tests

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make test_application_launch_unit
```

### Run Tests

```bash
# Run all application launch tests
./tests/application_launch/test_application_launch_unit

# Run with verbose output
./tests/application_launch/test_application_launch_unit --verbose

# Run specific test
./tests/application_launch/test_application_launch_unit -p /application_launch/main_window_display
```

### Run via CTest

```bash
ctest -R ApplicationLaunchUnit -V
```

## Test Coverage

### Main Window Display Test
- Creates a GTK window
- Verifies window properties (title, size)
- Tests window can be displayed

### Keyboard Shortcuts Test
- Creates accelerator group
- Registers keyboard shortcuts (Ctrl+Q, Ctrl+P, Ctrl+F)
- Simulates shortcut activation
- Verifies callbacks are triggered

### Graceful Quit Test
- Creates window with delete event handler
- Simulates window close event
- Verifies cleanup is performed

### Window State Management Test
- Tests window maximize functionality
- Tests window size management
- Verifies state persistence

### Application Menu Integration Test (macOS)
- Verifies application can launch without GDesktopAppInfo
- Tests macOS-specific menu handling

### MIME Type Registration Test (macOS)
- Verifies MIME type registration is disabled on macOS
- Ensures bundle handles file associations

## Platform-Specific Behavior

### macOS
- Application menu is handled by the OS
- MIME type registration is disabled (handled by bundle)
- Uses Cocoa and CoreFoundation frameworks

### Linux
- Uses GDesktopAppInfo for desktop integration
- MIME type registration is enabled
- Uses standard GTK application menu

## Troubleshooting

### Test Failures

If tests fail, check:

1. GTK3 is properly installed
2. Display server is available (X11 or Wayland on Linux, Quartz on macOS)
3. Required frameworks are linked (macOS)

### Running Tests Without Display

Some tests require a display server. On Linux, you can use Xvfb:

```bash
xvfb-run ./tests/application_launch/test_application_launch_unit
```

On macOS, tests should run without additional setup.

## Implementation Notes

- Tests use GTK test framework (`gtk_test_init`, `g_test_add_func`)
- Platform-specific code is wrapped in `#ifdef __APPLE__`
- Tests are minimal and focus on core functionality
- No mocking is used - tests validate real GTK behavior

## Related Files

- `src/remmina_main.c` - Main application window implementation
- `src/remmina.c` - Application entry point
- `.kiro/specs/macos-port/requirements.md` - Requirements document
- `.kiro/specs/macos-port/design.md` - Design document
