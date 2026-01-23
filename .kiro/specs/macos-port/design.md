# Design Document: macOS Port

## Overview

This design document describes the technical approach for porting Remmina to macOS. The port will enable macOS users to use Remmina's remote desktop capabilities while maintaining code compatibility with the Linux version. The design focuses on five key areas:

1. **CMake Build System Adaptation** - Configuring the build system to detect macOS and apply platform-specific settings
2. **Platform Abstraction Layer** - Creating macOS-specific implementations for features that use Linux-specific libraries
3. **Source Code Modifications** - Updating existing code to handle platform differences using conditional compilation
4. **Application Bundle Creation** - Packaging Remmina as a standard macOS .app bundle
5. **Testing Strategy** - Ensuring correctness through property-based and unit testing

The design preserves the existing Linux functionality while adding macOS support through conditional compilation and platform abstraction layers.

## Architecture

### High-Level Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     Remmina Application                      │
│  ┌────────────────────────────────────────────────────────┐ │
│  │              GTK3 User Interface Layer                  │ │
│  └────────────────────────────────────────────────────────┘ │
│  ┌────────────────────────────────────────────────────────┐ │
│  │           Core Application Logic (Shared)              │ │
│  │  • Connection Management  • File Management            │ │
│  │  • Plugin System         • Preferences                 │ │
│  └────────────────────────────────────────────────────────┘ │
│  ┌──────────────────┬──────────────────┬─────────────────┐ │
│  │ Platform         │ Platform         │ Platform        │ │
│  │ Abstraction:     │ Abstraction:     │ Abstraction:    │ │
│  │ Secret Storage   │ Service Discovery│ System Tray     │ │
│  │                  │                  │                 │ │
│  │ Linux: libsecret │ Linux: Avahi     │ Linux: AppInd.  │ │
│  │ macOS: Keychain  │ macOS: Bonjour   │ macOS: StatusIcon│ │
│  └──────────────────┴──────────────────┴─────────────────┘ │
│  ┌────────────────────────────────────────────────────────┐ │
│  │              Protocol Plugins (Shared)                  │ │
│  │  • RDP (FreeRDP)  • VNC (libvncclient)  • SSH (libssh) │ │
│  └────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
         │                    │                    │
         ▼                    ▼                    ▼
┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐
│ macOS Keychain  │  │ macOS Bonjour   │  │ GTK StatusIcon  │
│ (Security.framework)│ (dns_sd.h)      │  │                 │
└─────────────────┘  └─────────────────┘  └─────────────────┘
```

### Build System Architecture

The CMake build system will detect the platform and configure accordingly:

```
CMakeLists.txt (Root)
    │
    ├─ Platform Detection (CMAKE_SYSTEM_NAME)
    │   ├─ Darwin (macOS) → Apply macOS configuration
    │   └─ Linux → Apply Linux configuration
    │
    ├─ Dependency Discovery
    │   ├─ macOS: Search Homebrew paths
    │   └─ Linux: Standard pkg-config
    │
    ├─ Feature Flags
    │   ├─ macOS: Disable Avahi, AppIndicator, Telepathy
    │   │         Enable macOS Keychain
    │   └─ Linux: Enable all Linux features
    │
    └─ Subdirectories
        ├─ src/ (Main application)
        ├─ plugins/ (Protocol plugins)
        └─ data/ (Resources)
```

## Components and Interfaces

### Component 1: CMake Platform Detection Module

**Purpose:** Detect macOS platform and apply appropriate build configuration.

**Location:** `CMakeLists.txt` (root)

**Interface:**
```cmake
# Platform detection sets these variables:
# - APPLE (boolean) - TRUE on macOS
# - CMAKE_SYSTEM_NAME (string) - "Darwin" on macOS
# - NO_UNDEFINED_FLAG (string) - Platform-specific linker flag

# Configuration options set:
# - WITH_AVAHI (boolean) - OFF on macOS
# - WITH_APPINDICATOR (boolean) - OFF on macOS
# - WITH_TELEPATHY (boolean) - OFF on macOS
# - WITH_MACOS_KEYCHAIN (boolean) - ON on macOS
# - CMAKE_MACOSX_RPATH (boolean) - ON on macOS
# - CMAKE_INSTALL_RPATH (string) - "@executable_path/../Frameworks:@executable_path/../lib"
```

**Implementation Details:**
- Add platform detection block after existing platform checks
- Set macOS-specific CMake variables (RPATH, linker flags)
- Define feature flags based on platform
- Search Homebrew paths for dependencies (`/usr/local`, `/opt/homebrew`)
- Link macOS frameworks (Security, Cocoa) when needed

### Component 2: macOS Keychain Abstraction Layer

**Purpose:** Provide secure password storage using macOS Keychain instead of libsecret.

**Location:** 
- `src/remmina_keychain_macos.c` (implementation)
- `src/remmina_keychain_macos.h` (interface)

**Interface:**
```c
// Initialize Keychain support
gboolean remmina_keychain_macos_init(void);

// Store a password in Keychain
gboolean remmina_keychain_macos_store_password(
    const gchar *service,    // Service identifier (e.g., "Remmina")
    const gchar *account,    // Account identifier (e.g., connection profile path)
    const gchar *password,   // Password to store
    GError **error);         // Error output

// Retrieve a password from Keychain
gchar* remmina_keychain_macos_get_password(
    const gchar *service,
    const gchar *account,
    GError **error);

// Delete a password from Keychain
gboolean remmina_keychain_macos_delete_password(
    const gchar *service,
    const gchar *account,
    GError **error);
```

**Implementation Details:**
- Use Security framework APIs: `SecKeychainAddGenericPassword`, `SecKeychainFindGenericPassword`, `SecKeychainItemDelete`
- Map Remmina's service/account model to Keychain's generic password storage
- Return GError with descriptive messages on failure
- Handle OSStatus error codes from Security framework
- Free memory using `SecKeychainItemFreeContent` after retrieval

**Integration Point:**
- Modify `plugins/secret/src/glibsecret_plugin.c` to use Keychain on macOS:
```c
#ifdef __APPLE__
#include "remmina_keychain_macos.h"
#endif

static void remmina_plugin_secret_store_password(...) {
#ifdef __APPLE__
    remmina_keychain_macos_store_password(service, account, password, &error);
#else
    secret_password_store_sync(...);  // Existing libsecret code
#endif
}
```

### Component 3: Service Discovery Abstraction

**Purpose:** Provide network service discovery using macOS Bonjour instead of Avahi.

**Location:**
- `src/remmina_avahi.c` (existing, needs modification)
- `src/remmina_bonjour_macos.c` (new, macOS implementation)
- `src/remmina_bonjour_macos.h` (new, interface)

**Interface:**
```c
// Initialize service discovery
gboolean remmina_service_discovery_init(void);

// Start browsing for services of a specific type
gboolean remmina_service_discovery_browse(
    const gchar *service_type,  // e.g., "_rfb._tcp" for VNC
    RemminaServiceCallback callback,
    gpointer user_data);

// Stop browsing
void remmina_service_discovery_stop(void);

// Callback for discovered services
typedef void (*RemminaServiceCallback)(
    const gchar *service_name,
    const gchar *hostname,
    guint16 port,
    gpointer user_data);
```

**Implementation Details (macOS):**
- Use `<dns_sd.h>` (Bonjour C API) or NSNetService (requires Objective-C)
- Implement `DNSServiceBrowse` for service discovery
- Convert Bonjour service types to Remmina's internal format
- Integrate with GLib main loop using `DNSServiceRefSockFD` and `g_io_add_watch`

**Implementation Details (Linux):**
- Wrap existing Avahi code in `#ifndef __APPLE__` blocks
- Maintain current functionality unchanged

### Component 4: System Tray Abstraction

**Purpose:** Provide system tray integration using GTK StatusIcon on macOS.

**Location:** `src/remmina_applet_menu.c` (existing, needs modification)

**Interface:**
```c
// Create system tray icon
gboolean remmina_tray_icon_init(void);

// Update tray icon
void remmina_tray_icon_set_tooltip(const gchar *tooltip);

// Show/hide tray icon
void remmina_tray_icon_set_visible(gboolean visible);

// Destroy tray icon
void remmina_tray_icon_destroy(void);
```

**Implementation Details:**
- On macOS: Use `GtkStatusIcon` (deprecated but functional) or implement native NSStatusItem
- On Linux: Continue using AppIndicator where available, fall back to StatusIcon
- Conditional compilation:
```c
#ifdef __APPLE__
    // Use GTK StatusIcon
    status_icon = gtk_status_icon_new_from_icon_name("remmina");
#else
    // Use AppIndicator on Linux
    indicator = app_indicator_new(...);
#endif
```

### Component 5: Application Bundle Builder

**Purpose:** Package Remmina as a macOS .app bundle with all dependencies.

**Location:** `scripts/create-macos-bundle.sh` (new script)

**Bundle Structure:**
```
Remmina.app/
├── Contents/
│   ├── Info.plist              # Bundle metadata
│   ├── MacOS/
│   │   └── remmina             # Executable
│   ├── Resources/
│   │   ├── Remmina.icns        # Application icon
│   │   ├── ui/                 # Glade UI files
│   │   ├── theme/              # Color schemes
│   │   └── lib/
│   │       └── remmina/
│   │           └── plugins/    # Protocol plugins (.so files)
│   └── Frameworks/             # Bundled libraries (optional)
```

**Info.plist Structure:**
```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" 
    "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>remmina</string>
    <key>CFBundleIdentifier</key>
    <string>org.remmina.Remmina</string>
    <key>CFBundleName</key>
    <string>Remmina</string>
    <key>CFBundleVersion</key>
    <string>1.4.41</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>NSHighResolutionCapable</key>
    <true/>
    <key>LSMinimumSystemVersion</key>
    <string>10.15</string>
</dict>
</plist>
```

**Implementation:**
- Bash script to create bundle structure
- Copy executable to Contents/MacOS
- Copy plugins to Contents/Resources/lib/remmina/plugins
- Copy UI files and themes to Contents/Resources
- Generate Info.plist with version information
- Convert PNG icon to ICNS format using `iconutil`
- Optionally bundle GTK and dependencies using `dylibbundler`

### Component 6: Resource Path Resolution

**Purpose:** Locate resources (UI files, themes, plugins) within the application bundle.

**Location:** `src/remmina_file_manager.c`, `src/remmina_plugin_manager.c` (modifications)

**Interface:**
```c
// Get the bundle's Resources directory path
// Returns NULL if not in a bundle
// Caller must free with g_free()
gchar* remmina_get_bundle_resource_path(void);

// Get the resource directory (UI files, themes, etc.)
// Caller must free with g_free()
gchar* remmina_get_resource_dir(void);

// Get the plugin directory
// Caller must free with g_free()
gchar* remmina_get_plugin_dir(void);

// Get the UI directory
// Caller must free with g_free()
gchar* remmina_get_ui_dir(void);
```

**Implementation Details:**
```c
#ifdef __APPLE__
// Defined in remmina_bundle_macos.h
gchar* remmina_get_bundle_resource_path(void) {
    CFBundleRef bundle = CFBundleGetMainBundle();
    if (bundle) {
        CFURLRef resourceURL = CFBundleCopyResourcesDirectoryURL(bundle);
        char path[PATH_MAX];
        if (CFURLGetFileSystemRepresentation(resourceURL, TRUE, 
                                             (UInt8*)path, PATH_MAX)) {
            CFRelease(resourceURL);
            return g_strdup(path);
        }
        CFRelease(resourceURL);
    }
    return NULL;
}

gchar* remmina_get_resource_dir(void) {
    gchar *bundle_path = remmina_get_bundle_resource_path();
    if (bundle_path) {
        return bundle_path;  // Caller must free
    }
    return g_strdup(REMMINA_RUNTIME_DATADIR);  // Fallback
}

gchar* remmina_get_plugin_dir(void) {
    gchar *bundle_path = remmina_get_bundle_resource_path();
    if (bundle_path) {
        gchar *plugin_dir = g_build_filename(bundle_path, "lib", "remmina", "plugins", NULL);
        g_free(bundle_path);
        return plugin_dir;  // Caller must free
    }
    return g_strdup(REMMINA_RUNTIME_PLUGINDIR);  // Fallback
}

gchar* remmina_get_ui_dir(void) {
    gchar *bundle_path = remmina_get_bundle_resource_path();
    if (bundle_path) {
        gchar *ui_dir = g_build_filename(bundle_path, "ui", NULL);
        g_free(bundle_path);
        return ui_dir;  // Caller must free
    }
    return g_strdup(REMMINA_RUNTIME_UIDIR);  // Fallback
}
#else
// Existing Linux implementation
gchar* remmina_get_resource_dir(void) {
    return g_strdup(REMMINA_RUNTIME_DATADIR);
}
#endif
```

## Data Models

### Keychain Entry Model

Represents a password stored in macOS Keychain:

```c
typedef struct {
    gchar *service;      // Service name (e.g., "Remmina")
    gchar *account;      // Account identifier (connection profile path)
    gchar *password;     // The actual password (retrieved from Keychain)
} RemminaKeychainEntry;
```

**Mapping to Keychain:**
- Service → `kSecAttrService` attribute
- Account → `kSecAttrAccount` attribute
- Password → `kSecValueData` (stored securely)

### Service Discovery Model

Represents a discovered network service:

```c
typedef struct {
    gchar *service_name;  // Human-readable name
    gchar *service_type;  // Type (e.g., "_rfb._tcp" for VNC)
    gchar *hostname;      // Resolved hostname
    guint16 port;         // Service port
    gchar *protocol;      // Remmina protocol (RDP, VNC, SSH)
} RemminaDiscoveredService;
```

**Bonjour Mapping:**
- service_name → Bonjour service name
- service_type → Bonjour service type
- hostname → Resolved from Bonjour TXT records
- port → Bonjour port number

### Build Configuration Model

Represents platform-specific build settings:

```c
typedef struct {
    gboolean is_macos;
    gboolean has_avahi;
    gboolean has_appindicator;
    gboolean has_keychain;
    gchar *plugin_dir;
    gchar *resource_dir;
} RemminaBuildConfig;
```

This is represented in CMake as configuration variables and in C code as preprocessor definitions.

## Correctness Properties

*A property is a characteristic or behavior that should hold true across all valid executions of a system—essentially, a formal statement about what the system should do. Properties serve as the bridge between human-readable specifications and machine-verifiable correctness guarantees.*

Before defining the correctness properties, I need to analyze which acceptance criteria are testable using the prework tool.


### Property Reflection

After analyzing all acceptance criteria, I've identified the following testable properties and eliminated redundancy:

**Properties to include:**
- Keychain round-trip (2.6) - Comprehensive test of storage/retrieval
- Plugin protocol availability (3.6) - Tests plugin loading and registration
- Resource loading from bundle (4.7) - Tests bundle structure and path resolution
- Platform abstraction equivalence (6.4) - Tests cross-platform functionality
- Connection profile persistence (7.2, 7.4 combined) - Tests save/load round-trip
- Service discovery display (11.3) - Tests Bonjour integration

**Redundancies eliminated:**
- Individual Keychain operations (2.1, 2.2, 2.3) are subsumed by round-trip property (2.6)
- Individual plugin loading tests (3.2, 3.3, 3.4) are covered by protocol availability property (3.6)
- Individual bundle structure tests (4.1-4.6) are covered by resource loading property (4.7)
- Profile password storage (7.3) is covered by Keychain round-trip (2.6) and profile persistence (7.4)

### Correctness Properties

Property 1: Keychain Password Round-Trip
*For any* service identifier, account identifier, and password string, storing the password in macOS Keychain then retrieving it using the same service and account identifiers should return the identical password value.
**Validates: Requirements 2.1, 2.2, 2.3, 2.6**

Property 2: Keychain Deletion Completeness
*For any* stored password in macOS Keychain, after deletion using the service and account identifiers, attempting to retrieve the password should return a "not found" error.
**Validates: Requirements 2.3, 2.5**

Property 3: Plugin Protocol Registration
*For any* successfully loaded protocol plugin, the plugin's protocol identifier should appear in the list of available protocols when creating a new connection.
**Validates: Requirements 3.1, 3.2, 3.3, 3.4, 3.6**

Property 4: Bundle Resource Resolution
*For any* resource file (UI definition, theme, icon) included in the application bundle, the resource should be locatable using the bundle-relative path resolution mechanism.
**Validates: Requirements 4.1, 4.2, 4.3, 4.4, 4.5, 4.6, 4.7**

Property 5: Connection Profile Persistence Round-Trip
*For any* connection profile with protocol, hostname, username, and password, saving the profile then loading it should produce a profile with identical values for all fields.
**Validates: Requirements 7.1, 7.2, 7.3, 7.4**

Property 6: Profile Listing Completeness
*For any* set of saved connection profiles in the config directory, the list of profiles displayed in the main window should contain exactly those profiles (no more, no less).
**Validates: Requirements 7.6**

Property 7: Platform Abstraction Functional Equivalence
*For any* platform-abstracted operation (password storage, service discovery, system tray), the operation should succeed on both macOS and Linux platforms with equivalent results.
**Validates: Requirements 6.4**

Property 8: Service Discovery Display
*For any* service discovered via Bonjour on macOS, the service should appear in the available services list with correct name, hostname, and port information.
**Validates: Requirements 11.2, 11.3**

Property 9: Icon Resource Loading
*For any* icon referenced in the UI, the icon should be successfully loaded from the application bundle's Resources directory.
**Validates: Requirements 8.4**

## Error Handling

### Build System Errors

**Missing Dependencies:**
- CMake shall check for required dependencies (GTK3, GLib, libsodium, etc.)
- If a required dependency is missing, CMake shall output: "Could not find [package]. Please install via: brew install [package]"
- Build shall fail with clear error message indicating which packages are needed

**Platform Detection Failures:**
- If platform cannot be determined, default to Linux configuration
- Log warning: "Unable to determine platform, using Linux defaults"

**Framework Linking Errors (macOS):**
- If Security framework cannot be found, error: "Security framework required for macOS Keychain support"
- If Cocoa framework cannot be found, error: "Cocoa framework required for macOS bundle support"

### Runtime Errors

**Keychain Errors:**
- `errSecItemNotFound` (-25300): Return GError with message "Password not found in Keychain"
- `errSecAuthFailed` (-25293): Return GError with message "Authentication failed accessing Keychain"
- `errSecDuplicateItem` (-25299): Delete existing item and retry store operation
- All other OSStatus codes: Return GError with message "Keychain operation failed with error code: [code]"

**Plugin Loading Errors:**
- Plugin file not found: Log "Plugin not found: [path]" and continue
- Plugin symbol resolution failure: Log "Failed to load plugin [name]: [error]" and continue
- Plugin initialization failure: Log "Plugin [name] initialization failed" and continue
- Application shall continue loading remaining plugins after any plugin failure

**Resource Loading Errors:**
- UI file not found: Display error dialog "UI definition file missing: [filename]" and exit
- Theme file not found: Log warning "Theme file not found: [filename]", use default theme
- Icon not found: Log warning "Icon not found: [name]", use fallback icon

**Service Discovery Errors:**
- Bonjour service unavailable: Log "Service discovery unavailable, manual entry only" and disable discovery UI
- Service resolution timeout: Log "Service resolution timeout for [service]" and skip service
- Invalid service data: Log "Invalid service data received" and skip service

**Connection Profile Errors:**
- Config directory creation failure: Display error "Cannot create config directory: [error]" and exit
- Profile save failure: Display error "Cannot save connection profile: [error]"
- Profile load failure: Log warning "Cannot load profile [filename]: [error]" and skip profile
- Password retrieval failure: Prompt user to re-enter password

### Error Recovery Strategies

**Graceful Degradation:**
- If service discovery fails, allow manual hostname entry
- If system tray fails, continue without tray icon
- If theme loading fails, use built-in default theme
- If icon loading fails, use text labels

**User Notification:**
- Critical errors (cannot create config directory): Modal error dialog, then exit
- Important errors (cannot save profile): Modal error dialog, allow retry
- Minor errors (icon not found): Log warning, continue silently
- Debug errors (plugin load failure): Log to console if debug enabled

**Retry Logic:**
- Keychain operations: Retry once on `errSecDuplicateItem` after deletion
- Service discovery: Retry resolution up to 3 times with 1-second delay
- Resource loading: No retry (fail fast)
- Plugin loading: No retry (skip and continue)

## Testing Strategy

### Dual Testing Approach

This project will use both unit testing and property-based testing to ensure comprehensive coverage:

**Unit Tests:**
- Specific examples demonstrating correct behavior
- Edge cases and error conditions
- Integration points between components
- Platform-specific code paths

**Property-Based Tests:**
- Universal properties that hold for all inputs
- Comprehensive input coverage through randomization
- Minimum 100 iterations per property test
- Each test tagged with feature name and property number

### Property-Based Testing Configuration

**Library Selection:**
- C: Use `theft` library (https://github.com/silentbicycle/theft) for property-based testing
- Alternative: `QuickCheck` for C (https://github.com/mcandre/qc)

**Test Configuration:**
- Minimum 100 iterations per property test (due to randomization)
- Each property test must reference its design document property
- Tag format: `/* Feature: macos-port, Property N: [property text] */`

**Property Test Implementation:**

Example for Property 1 (Keychain Round-Trip):
```c
/* Feature: macos-port, Property 1: Keychain Password Round-Trip */
static enum theft_trial_res prop_keychain_roundtrip(struct theft *t, void *arg1) {
    struct keychain_test_data *data = (struct keychain_test_data *)arg1;
    GError *error = NULL;
    gchar *retrieved_password = NULL;
    
    // Store password
    gboolean store_result = remmina_keychain_macos_store_password(
        data->service, data->account, data->password, &error);
    if (!store_result) {
        return THEFT_TRIAL_FAIL;
    }
    
    // Retrieve password
    retrieved_password = remmina_keychain_macos_get_password(
        data->service, data->account, &error);
    if (!retrieved_password) {
        return THEFT_TRIAL_FAIL;
    }
    
    // Compare
    if (g_strcmp0(data->password, retrieved_password) != 0) {
        g_free(retrieved_password);
        return THEFT_TRIAL_FAIL;
    }
    
    // Cleanup
    g_free(retrieved_password);
    remmina_keychain_macos_delete_password(data->service, data->account, NULL);
    
    return THEFT_TRIAL_PASS;
}
```

### Unit Test Coverage

**Build System Tests:**
- Test CMake platform detection on macOS
- Test feature flags are set correctly
- Test RPATH configuration
- Test Homebrew path search
- Test framework linking

**Keychain Integration Tests:**
- Test password storage with empty password (edge case)
- Test password storage with special characters (edge case)
- Test password retrieval for non-existent item (edge case)
- Test error handling for Keychain failures (edge case)

**Plugin Loading Tests:**
- Test plugin discovery in bundle
- Test plugin loading with missing dependency (edge case)
- Test plugin loading with corrupted file (edge case)
- Test protocol registration after load

**Bundle Creation Tests:**
- Test Info.plist generation
- Test executable placement
- Test plugin placement
- Test resource placement
- Test icon conversion

**Resource Loading Tests:**
- Test UI file loading from bundle
- Test theme file loading from bundle
- Test icon loading from bundle
- Test resource loading with missing file (edge case)

**Service Discovery Tests:**
- Test Bonjour service browsing
- Test service resolution
- Test service display in UI
- Test fallback to manual entry (edge case)

**Connection Profile Tests:**
- Test profile creation
- Test profile saving
- Test profile loading
- Test profile listing
- Test config directory creation (edge case)

### Integration Testing

**Cross-Platform Tests:**
- Run same test suite on macOS and Linux
- Verify platform abstraction provides equivalent functionality
- Test that Linux builds are not broken by macOS changes

**End-to-End Tests:**
- Build application on macOS
- Create application bundle
- Launch bundle
- Create connection profile
- Save profile with password
- Verify password stored in Keychain
- Load profile
- Verify password retrieved from Keychain
- Connect to remote system (RDP, VNC, SSH)

### Test Execution

**Local Development:**
```bash
# Build with tests
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DWITH_TESTS=ON ..
make

# Run unit tests
make test

# Run property-based tests
./tests/test_keychain_properties
./tests/test_plugin_properties
./tests/test_profile_properties
```

**Continuous Integration:**
- Run tests on macOS (GitHub Actions: macos-latest)
- Run tests on Linux (GitHub Actions: ubuntu-latest)
- Verify no regressions on Linux
- Verify macOS-specific features work correctly

### Test Data Generation

**For Property-Based Tests:**
- Generate random service names (alphanumeric, 1-64 characters)
- Generate random account names (alphanumeric, 1-64 characters)
- Generate random passwords (printable ASCII, 1-256 characters)
- Generate random hostnames (valid DNS format)
- Generate random port numbers (1-65535)
- Generate random protocol names (from available plugins)

**For Edge Case Tests:**
- Empty strings
- Very long strings (>1000 characters)
- Special characters (Unicode, control characters)
- Invalid paths
- Missing files
- Corrupted data

### Success Criteria

Tests pass when:
- All property-based tests pass 100 iterations without failure
- All unit tests pass
- All edge case tests pass
- Integration tests complete successfully on both macOS and Linux
- No memory leaks detected (using Valgrind on Linux, Instruments on macOS)
- No crashes or segfaults during test execution
