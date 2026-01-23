# Plugin Loading Tests

This directory contains tests for verifying that plugin loading works correctly in the Remmina application bundle, particularly on macOS.

## Test Files

### Property-Based Tests

- **test_plugin_properties.c**: Property-based tests that verify universal properties about plugin loading
  - Property 3: Plugin Protocol Registration - Validates that loaded plugins register protocols correctly

### Unit Tests

- **test_plugin_unit.c**: Unit tests for specific edge cases and error conditions
  - Test plugin loading with missing dependencies
  - Test plugin loading with corrupted files
  - Test error logging for failed plugins
  - Test plugin directory discovery in bundle
  - Test plugin type verification

## Requirements Validated

These tests validate the following requirements from the macOS port specification:

- **Requirement 3.1**: Plugin loading from bundle plugin directory
- **Requirement 3.2**: RDP plugin registration
- **Requirement 3.3**: VNC plugin registration
- **Requirement 3.4**: SSH plugin registration
- **Requirement 3.5**: Error handling for failed plugin loads
- **Requirement 3.6**: Protocol availability in connection creation

## Building and Running

### Build Tests

```bash
cd build
cmake -DWITH_TESTS=ON ..
make test_plugin_properties test_plugin_unit
```

### Run Property-Based Tests

```bash
./tests/plugin_loading/test_plugin_properties
```

This runs 100 iterations of the plugin protocol registration property test.

### Run Unit Tests

```bash
./tests/plugin_loading/test_plugin_unit
```

This runs all unit tests for plugin loading edge cases.

### Run All Plugin Tests

```bash
ctest -L plugin
```

## Test Design

### Property-Based Testing

The property tests verify universal properties that should hold for all plugin loading scenarios:

1. **Plugin Protocol Registration**: For any successfully loaded protocol plugin, the plugin should:
   - Have a valid name and version
   - Have required function pointers (init, open_connection, close_connection)
   - Be retrievable by name through the plugin manager
   - Appear in the list of available protocols

### Unit Testing

The unit tests verify specific behaviors and edge cases:

1. **Missing Dependency**: Attempting to load a non-existent plugin file should fail gracefully
2. **Corrupted File**: Attempting to load a corrupted plugin file should fail gracefully
3. **Error Logging**: Failed plugin loads should be logged without crashing
4. **Directory Discovery**: Plugin directory should be correctly identified (bundle on macOS, standard on Linux)
5. **Type Verification**: Different plugin types should be distinguishable

## Platform-Specific Behavior

### macOS

On macOS, plugins are loaded from the application bundle:
- Plugin directory: `Remmina.app/Contents/Resources/lib/remmina/plugins/`
- Uses `remmina_get_plugin_dir()` to locate bundle plugin directory

### Linux

On Linux, plugins are loaded from the standard installation directory:
- Plugin directory: `/usr/lib/remmina/plugins/` or `/usr/local/lib/remmina/plugins/`
- Uses `REMMINA_RUNTIME_PLUGINDIR` CMake variable

## Expected Plugins

The tests check for the following protocol plugins:

- **RDP**: Remote Desktop Protocol (requires FreeRDP)
- **VNC**: Virtual Network Computing (requires libvncclient)
- **SSH**: Secure Shell (requires libssh)

Note: These plugins may not be available if their dependencies are not installed. The tests will log warnings but not fail if expected plugins are missing.

## Troubleshooting

### No Plugins Found

If the property test reports "No protocol plugins found":
1. Verify plugins are installed in the correct directory
2. Check that plugin dependencies are installed (FreeRDP, libvncclient, libssh)
3. Verify the plugin directory path is correct for your platform

### Plugin Load Failures

If plugins fail to load:
1. Check the console output for error messages
2. Verify plugin files have correct permissions
3. Ensure all plugin dependencies are available
4. On macOS, verify the bundle structure is correct

### Test Failures

If tests fail:
1. Run with verbose output to see detailed error messages
2. Check that the plugin manager is initialized correctly
3. Verify the test environment is set up properly
4. Ensure GTK is initialized before running tests

## Implementation Notes

- Tests use the actual plugin manager implementation, not mocks
- Property tests run 100 iterations to ensure comprehensive coverage
- Unit tests create temporary directories for test files
- All tests clean up after themselves
- Tests are designed to work on both macOS and Linux
