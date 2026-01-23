# Service Discovery Tests

This directory contains tests for the macOS Bonjour service discovery implementation.

## Overview

The service discovery tests verify that Remmina can discover network services (VNC, RDP, SSH) using macOS Bonjour APIs. These tests are only available on macOS.

## Test Files

- `test_service_discovery_properties.c` - Property-based tests for service discovery
- `test_service_discovery_unit.c` - Unit tests for edge cases
- `CMakeLists.txt` - Build configuration

## Property-Based Tests

### Property 8: Service Discovery Display
**Validates Requirements:** 11.2, 11.3

Tests that services discovered via Bonjour appear in the list with correct information:
- Service name is non-empty
- Hostname is non-empty
- Port is valid (non-zero)

**Note:** This test runs fewer iterations (5 instead of 100) because each iteration takes 3 seconds to browse for services.

## Unit Tests

### Edge Cases
- Fallback to manual entry when Bonjour unavailable
- Service resolution timeout handling
- Invalid service data handling

## Building and Running

### Build Tests
```bash
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DWITH_TESTS=ON ..
make
```

### Run Property Tests
```bash
./tests/service_discovery/test_service_discovery_properties
```

### Run Unit Tests
```bash
./tests/service_discovery/test_service_discovery_unit
```

### Run All Tests
```bash
make test
# or
ctest -V
```

## Requirements

- macOS 10.15 or later
- GLib 2.0
- Bonjour (included with macOS)

## Test Environment

For meaningful results, ensure there are discoverable services on your network:
- VNC servers advertising `_rfb._tcp`
- RDP servers advertising `_rdp._tcp`
- SSH servers advertising `_ssh._tcp`

You can use macOS Screen Sharing or other remote desktop software to create test services.

## Troubleshooting

### No Services Discovered
This is not necessarily a failure - the test verifies that browsing works, even if no services are found. To test with actual services:
1. Enable Screen Sharing on another Mac
2. Run the test again

### Test Timeout
The property test waits 3 seconds per iteration for service discovery. This is normal behavior.

### Permission Issues
Bonjour requires network access. Ensure your firewall settings allow mDNS traffic.
