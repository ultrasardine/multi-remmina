# Service Discovery Implementation Notes

## Overview

This document describes the implementation of Bonjour service discovery for macOS as part of the Remmina macOS port.

## Implementation Summary

### Files Created

1. **src/remmina_bonjour_macos.h** - Header file defining the service discovery interface
2. **src/remmina_bonjour_macos.c** - Implementation using macOS Bonjour (dns_sd.h)
3. **tests/service_discovery/test_service_discovery_properties.c** - Property-based tests
4. **tests/service_discovery/test_service_discovery_unit.c** - Unit tests for edge cases
5. **tests/service_discovery/CMakeLists.txt** - Build configuration for tests
6. **tests/service_discovery/README.md** - Test documentation

### Key Design Decisions

#### 1. Bonjour API Choice
We chose to use the C-based `dns_sd.h` API instead of the Objective-C NSNetService API because:
- Keeps the codebase in pure C (consistent with Remmina)
- Avoids requiring Objective-C compilation
- Provides lower-level control over service discovery
- Easier integration with GLib main loop

#### 2. GLib Main Loop Integration
The implementation integrates Bonjour with GLib's main loop using:
- `DNSServiceRefSockFD()` to get the file descriptor
- `g_io_channel_unix_new()` to create a GIOChannel
- `g_io_add_watch()` to monitor the file descriptor
- `DNSServiceProcessResult()` to process events

This ensures service discovery events are handled asynchronously without blocking the UI.

#### 3. Context Management
We maintain two types of contexts:
- **BrowseContext**: Tracks active browse operations
- **ResolveContext**: Tracks service resolution operations

Each context includes:
- DNSServiceRef for the operation
- Callback function and user data
- GLib watch ID for cleanup

#### 4. Memory Management
- All contexts are allocated with `g_malloc0()` and freed with `g_free()`
- DNSServiceRef objects are deallocated with `DNSServiceRefDeallocate()`
- GLib watch sources are removed with `g_source_remove()`
- Global list of browse contexts is maintained for cleanup

#### 5. Error Handling
- All DNSService functions return DNSServiceErrorType
- Errors are logged with `g_warning()`
- Failed operations clean up resources and return FALSE
- Service discovery continues even if individual services fail to resolve

## API Design

### Interface Functions

```c
gboolean remmina_service_discovery_init(void);
```
- Initializes service discovery (no-op for Bonjour, always returns TRUE)
- Must be called before browsing

```c
gboolean remmina_service_discovery_browse(
    const gchar *service_type,
    RemminaServiceCallback callback,
    gpointer user_data);
```
- Starts browsing for services of the specified type
- Service types: "_rfb._tcp" (VNC), "_rdp._tcp" (RDP), "_ssh._tcp" (SSH)
- Callback is invoked for each discovered service
- Returns TRUE if browsing started successfully

```c
void remmina_service_discovery_stop(void);
```
- Stops all active browse operations
- Cleans up all resources
- Safe to call multiple times

### Callback Function

```c
typedef void (*RemminaServiceCallback)(
    const gchar *service_name,
    const gchar *hostname,
    guint16 port,
    gpointer user_data);
```
- Called when a service is discovered and resolved
- service_name: Human-readable name (e.g., "John's Mac")
- hostname: Resolved hostname or IP address
- port: Service port number (host byte order)

## Testing Strategy

### Property-Based Test (Property 8)

**Test:** Service Discovery Display  
**Validates:** Requirements 11.2, 11.3

The property test verifies:
- Service discovery can be initialized
- Browsing can be started for VNC services
- Discovered services have valid data (non-empty name, hostname, non-zero port)
- The system handles cases where no services are found

**Note:** This test runs only 5 iterations (instead of 100) because each iteration takes 3 seconds to browse for services.

### Unit Tests

The unit tests cover:
1. **Basic Functionality**
   - Initialization succeeds
   - Browse with valid parameters succeeds
   - Multiple browse operations can coexist

2. **Error Handling**
   - Browse with NULL service type fails gracefully
   - Browse with NULL callback fails gracefully
   - Browse with invalid service type format doesn't crash

3. **Resource Management**
   - Stop without init is safe
   - Stop after stop is safe

4. **Edge Cases (Requirement 11.4)**
   - Fallback to manual entry when no services found
   - Service resolution timeout handling (3-second timeout)
   - Invalid service data handling

## Integration with Remmina

### Build System Integration

The Bonjour files are added to the build in `src/CMakeLists.txt`:

```cmake
if(APPLE)
  list(APPEND REMMINA_SRCS
    "remmina_keychain_macos.c"
    "remmina_keychain_macos.h"
    "remmina_bonjour_macos.c"
    "remmina_bonjour_macos.h"
  )
  message(STATUS "Added macOS Keychain and Bonjour support files")
endif()
```

### Future Integration Steps

The next task (Task 7) will integrate this implementation with the existing Avahi code:

1. Modify `src/remmina_avahi.c` to wrap Avahi code in `#ifndef __APPLE__`
2. Provide stub functions on macOS that call the Bonjour implementation
3. Update `src/CMakeLists.txt` to conditionally compile Avahi code

## Known Limitations

1. **Service Types**: Currently only supports standard service types (_rfb._tcp, _rdp._tcp, _ssh._tcp)
2. **TXT Records**: Does not parse TXT records for additional service metadata
3. **Service Removal**: Does not handle service removal events (only additions)
4. **Network Changes**: Does not automatically restart browsing on network changes

## Future Enhancements

1. **TXT Record Parsing**: Extract additional metadata from TXT records
2. **Service Removal**: Handle kDNSServiceFlagsRemove flag in browse callback
3. **Network Monitoring**: Restart browsing when network interfaces change
4. **Service Filtering**: Allow filtering services by domain or interface
5. **Caching**: Cache discovered services to reduce network traffic

## Testing on macOS

### Prerequisites
- macOS 10.15 or later
- GLib 2.0 installed (via Homebrew)
- Bonjour-enabled services on the network

### Building Tests
```bash
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DWITH_TESTS=ON ..
make
```

### Running Tests
```bash
# Property tests
./tests/service_discovery/test_service_discovery_properties

# Unit tests
./tests/service_discovery/test_service_discovery_unit

# All tests
make test
```

### Creating Test Services

To test service discovery, you can:
1. Enable Screen Sharing on another Mac (advertises _rfb._tcp)
2. Use `dns-sd` command-line tool to advertise test services:
   ```bash
   dns-sd -R "Test VNC" _rfb._tcp . 5900
   ```

## References

- [Bonjour Overview](https://developer.apple.com/bonjour/)
- [DNS Service Discovery API](https://developer.apple.com/documentation/dnssd)
- [Remmina macOS Port Design Document](../../.kiro/specs/macos-port/design.md)
- [Service Discovery Requirements](../../.kiro/specs/macos-port/requirements.md)
