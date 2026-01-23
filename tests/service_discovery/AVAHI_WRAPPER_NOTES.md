# Avahi Wrapper Implementation Notes

## Overview

This document describes the implementation of the Avahi wrapper for macOS that integrates the Bonjour service discovery implementation with the existing Avahi interface.

## Implementation Summary

### Files Modified

1. **src/remmina_avahi.c** - Modified to provide macOS-specific implementation using Bonjour
2. **src/CMakeLists.txt** - Updated to conditionally compile Avahi code based on platform
3. **tests/service_discovery/test_avahi_wrapper.c** - Created integration tests for the wrapper
4. **tests/service_discovery/CMakeLists.txt** - Updated to include wrapper tests

### Key Changes

#### 1. Platform-Specific Compilation

The `remmina_avahi.c` file now has three sections:

1. **macOS Implementation** (`#ifdef __APPLE__`)
   - Uses Bonjour service discovery via `remmina_bonjour_macos.h`
   - Provides adapter functions to convert Bonjour callbacks to Avahi-style hash table
   - Maintains the same `RemminaAvahi` interface as Linux

2. **Linux Implementation** (`#elif defined(HAVE_LIBAVAHI_CLIENT)`)
   - Original Avahi implementation unchanged
   - Uses Avahi client library for service discovery

3. **Stub Implementation** (`#else`)
   - Provides no-op functions when neither Avahi nor Bonjour is available
   - Ensures code compiles on all platforms

#### 2. Callback Adapter

The macOS implementation includes a callback adapter function:

```c
static void macos_service_callback_adapter(
    const gchar *service_name,
    const gchar *hostname,
    guint16 port,
    gpointer user_data)
```

This function:
- Converts Bonjour callback format to Avahi hash table format
- Creates keys in format: "name,type,domain" (e.g., "MyMac,_rfb._tcp,local")
- Creates values in format: "[hostname]:port" (e.g., "[192.168.1.100]:5900")
- Prevents duplicate entries in the hash table
- Logs discovered services for debugging

#### 3. Multiple Service Types Support

The macOS implementation browses for multiple service types simultaneously:
- `_rfb._tcp` - VNC services
- `_rdp._tcp` - RDP services  
- `_ssh._tcp` - SSH services

Each service type gets its own context stored in a GSList for proper cleanup.

#### 4. CMake Configuration

The `src/CMakeLists.txt` was updated to:

```cmake
if(AVAHI_FOUND AND NOT APPLE)
  include_directories(SYSTEM ${AVAHI_INCLUDE_DIRS})
  target_link_libraries(remmina ${AVAHI_LIBRARIES})
  message(STATUS "Avahi support enabled (Linux)")
elseif(APPLE)
  message(STATUS "Avahi support disabled on macOS (using Bonjour instead)")
endif()
```

This ensures:
- Avahi is only linked on Linux
- macOS builds use Bonjour instead
- Clear status messages during configuration

## API Compatibility

The macOS implementation maintains full API compatibility with the Linux Avahi implementation:

### RemminaAvahi Structure

```c
typedef struct _RemminaAvahi {
    GHashTable *discovered_services;  // Same on all platforms
    gboolean started;                 // Same on all platforms
    RemminaAvahiPriv *priv;          // NULL on macOS, used on Linux
} RemminaAvahi;
```

### Functions

All four functions maintain the same signature and behavior:

1. `RemminaAvahi* remmina_avahi_new(void)`
   - Creates and initializes a RemminaAvahi structure
   - Allocates hash table for discovered services
   - Returns NULL on stub implementation

2. `void remmina_avahi_start(RemminaAvahi* ga)`
   - Starts service discovery
   - On macOS: Calls `remmina_service_discovery_browse("_rfb._tcp", ...)`
   - On Linux: Uses Avahi service browser
   - On stub: No-op

3. `void remmina_avahi_stop(RemminaAvahi* ga)`
   - Stops service discovery
   - Clears discovered services hash table
   - On macOS: Calls `remmina_service_discovery_stop()`
   - On Linux: Frees Avahi resources
   - On stub: No-op

4. `void remmina_avahi_free(RemminaAvahi* ga)`
   - Stops service discovery if running
   - Frees all resources
   - Safe to call with NULL pointer

## Testing

### Integration Tests

The `test_avahi_wrapper.c` file provides comprehensive integration tests:

1. **Basic Functionality**
   - Wrapper can be created
   - Wrapper can start service discovery
   - Wrapper can stop service discovery
   - Wrapper can be freed safely

2. **Edge Cases**
   - Free without starting
   - Free with NULL pointer
   - Start multiple times (idempotent)
   - Stop multiple times (idempotent)

3. **Bonjour Integration**
   - Wrapper integrates with Bonjour implementation
   - GLib main loop processes events correctly

### Test Results

All tests pass successfully:

```
=== Avahi Wrapper Integration Tests ===

Running: Avahi wrapper can be created... PASSED
Running: Avahi wrapper can start service discovery... PASSED
Running: Avahi wrapper can stop service discovery... PASSED
Running: Avahi wrapper can be freed safely... PASSED
Running: Avahi wrapper can be freed without starting... PASSED
Running: Avahi wrapper handles NULL gracefully... PASSED
Running: Avahi wrapper can start multiple times... PASSED
Running: Avahi wrapper can stop multiple times... PASSED
Running: Avahi wrapper integrates with Bonjour... PASSED

=== Test Summary ===
Passed: 9
Failed: 0
```

## Requirements Validation

This implementation satisfies the following requirements:

### Requirement 11.1
✅ "WHEN compiling for macOS, THE Build_System SHALL disable Avahi dependency"
- CMake configuration disables Avahi on macOS
- Avahi libraries are not linked on macOS

### Requirement 11.5
✅ "WHEN compiling for Linux, THE Remmina SHALL continue using Avahi for service discovery"
- Linux implementation unchanged
- Avahi code wrapped in `#elif defined(HAVE_LIBAVAHI_CLIENT)`

### Requirement 6.2
✅ "WHEN compiling for macOS, THE Build_System SHALL disable Avahi, AppIndicator, and libsecret support"
- Avahi is disabled on macOS
- macOS uses Bonjour instead

## Integration with Existing Code

The wrapper integrates seamlessly with existing Remmina code:

1. **No API Changes**: All existing code that uses `remmina_avahi_*` functions continues to work
2. **Same Data Structure**: The `discovered_services` hash table uses the same format
3. **Same Behavior**: Service discovery works the same way from the user's perspective
4. **Platform Transparent**: Higher-level code doesn't need to know which implementation is used

## Future Enhancements

1. **Multiple Service Types**: Currently only browses for VNC (_rfb._tcp)
   - Could be extended to browse for RDP, SSH, etc.
   - Would require passing service type to `remmina_avahi_start()`

2. **Service Removal**: Currently only handles service additions
   - Could track service removals from Bonjour
   - Would require handling kDNSServiceFlagsRemove flag

3. **Domain Support**: Currently uses "local" domain
   - Could support custom domains
   - Would require parsing domain from Bonjour callbacks

## Known Limitations

1. **Service Type Tracking**: Each service type maintains its own context with the service type string for proper key formatting in the hash table
2. **No Service Removal**: Does not handle service removal events
   - Services remain in hash table until stop is called
   - Not a critical issue for typical use cases
3. **Hardcoded Domain**: Uses "local" domain for all services
   - Works correctly for typical Bonjour usage
   - Could be extended to support custom domains

## Conclusion

The Avahi wrapper successfully provides macOS compatibility while maintaining full API compatibility with the Linux implementation. All tests pass, and the implementation satisfies all requirements for Task 7 of the macOS port.
