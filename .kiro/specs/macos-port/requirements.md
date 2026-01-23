# Requirements Document: macOS Port

## Introduction

This document specifies the requirements for porting Remmina, a GTK-based remote desktop client, to run natively on macOS. The port will enable macOS users to connect to remote desktops using RDP, VNC, SSH, and other protocols while maintaining code compatibility with the Linux version.

## Glossary

- **Remmina**: The GTK-based remote desktop client application being ported
- **Build_System**: The CMake-based build configuration and compilation system
- **Platform_Abstraction_Layer**: Code layer that provides platform-specific implementations (e.g., Keychain vs libsecret)
- **Protocol_Plugin**: Dynamically loaded shared library implementing a remote desktop protocol (RDP, VNC, SSH, etc.)
- **Application_Bundle**: macOS .app package containing executable, resources, and dependencies
- **Keychain**: macOS secure password storage system (Security framework)
- **Homebrew**: macOS package manager used to install dependencies
- **GTK3**: Cross-platform GUI toolkit used by Remmina
- **Connection_Profile**: User-created configuration for connecting to a remote system
- **Secret_Storage**: Secure storage mechanism for passwords and credentials

## Requirements

### Requirement 1: Build System Configuration

**User Story:** As a developer, I want to build Remmina on macOS using standard tools, so that I can compile and test the application locally.

#### Acceptance Criteria

1. WHEN building on macOS, THE Build_System SHALL detect the Darwin platform and apply macOS-specific configuration
2. WHEN configuring the build, THE Build_System SHALL disable Linux-specific features (Avahi, AppIndicator, Telepathy) by default on macOS
3. WHEN linking libraries, THE Build_System SHALL use macOS linker flags (`-Wl,-undefined,error`) instead of Linux flags
4. WHEN installing the application, THE Build_System SHALL configure RPATH to `@executable_path/../Frameworks` and `@executable_path/../lib` for bundle compatibility
5. WHEN finding dependencies, THE Build_System SHALL search Homebrew installation paths (`/usr/local`, `/opt/homebrew`)
6. WHEN macOS Keychain support is enabled, THE Build_System SHALL link the Security framework
7. WHEN building plugins, THE Build_System SHALL locate FreeRDP, libvncserver, and libssh from Homebrew

### Requirement 2: Platform Abstraction for Secret Storage

**User Story:** As a user, I want my connection passwords stored securely in macOS Keychain, so that my credentials are protected using native macOS security.

#### Acceptance Criteria

1. WHEN storing a password, THE Platform_Abstraction_Layer SHALL save it to macOS Keychain using the Security framework
2. WHEN retrieving a password, THE Platform_Abstraction_Layer SHALL fetch it from macOS Keychain using service and account identifiers
3. WHEN deleting a password, THE Platform_Abstraction_Layer SHALL remove it from macOS Keychain
4. WHEN a password storage operation fails, THE Platform_Abstraction_Layer SHALL return a descriptive error with the OSStatus code
5. WHEN a password is not found, THE Platform_Abstraction_Layer SHALL return an error indicating the item does not exist
6. FOR ALL password operations, storing then retrieving a password SHALL return the same password value (round-trip property)

### Requirement 3: Protocol Plugin Support

**User Story:** As a user, I want to connect to remote systems using RDP, VNC, and SSH protocols, so that I can access different types of remote desktops and servers.

#### Acceptance Criteria

1. WHEN the application starts, THE Remmina SHALL load Protocol_Plugins from the bundle's plugin directory
2. WHEN the RDP plugin is loaded, THE Protocol_Plugin SHALL register RDP protocol support using FreeRDP library
3. WHEN the VNC plugin is loaded, THE Protocol_Plugin SHALL register VNC protocol support using libvncclient
4. WHEN the SSH plugin is loaded, THE Protocol_Plugin SHALL register SSH terminal and SFTP support using libssh
5. WHEN a plugin fails to load, THE Remmina SHALL log the error and continue loading other plugins
6. WHEN creating a connection, THE Remmina SHALL present available protocols from successfully loaded plugins

### Requirement 4: Application Bundle Creation

**User Story:** As a user, I want to install Remmina as a standard macOS application, so that I can launch it from Finder or Spotlight like other Mac apps.

#### Acceptance Criteria

1. WHEN packaging the application, THE Build_System SHALL create an Application_Bundle with standard macOS directory structure (Contents/MacOS, Contents/Resources, Contents/Frameworks)
2. WHEN the bundle is created, THE Application_Bundle SHALL include an Info.plist with bundle identifier, version, and display name
3. WHEN the bundle is created, THE Application_Bundle SHALL contain the Remmina executable in Contents/MacOS
4. WHEN the bundle is created, THE Application_Bundle SHALL contain Protocol_Plugins in Contents/Resources/lib/remmina/plugins
5. WHEN the bundle is created, THE Application_Bundle SHALL contain UI definition files and themes in Contents/Resources
6. WHEN the bundle includes an icon, THE Application_Bundle SHALL contain a .icns file in Contents/Resources
7. WHEN the bundle is launched, THE Remmina SHALL locate resources relative to the bundle path

### Requirement 5: Dependency Management

**User Story:** As a developer, I want clear documentation of required dependencies, so that I can install prerequisites and build the application successfully.

#### Acceptance Criteria

1. WHEN installing build dependencies, THE developer SHALL use Homebrew to install GTK3, GLib, libsodium, OpenSSL, libssh, FreeRDP, and libvncserver
2. WHEN GTK3 is installed via Homebrew, THE Build_System SHALL locate GTK3 headers and libraries
3. WHEN FreeRDP is installed via Homebrew, THE Build_System SHALL detect FreeRDP version and configure the RDP plugin accordingly
4. WHEN dependencies are missing, THE Build_System SHALL report which packages need to be installed
5. WHEN building with all dependencies installed, THE Build_System SHALL complete without dependency-related errors
6. THE Remmina SHALL use only open-source dependencies for all functionality

### Requirement 6: Linux Feature Compatibility

**User Story:** As a maintainer, I want to preserve Linux functionality while adding macOS support, so that the codebase remains maintainable across platforms.

#### Acceptance Criteria

1. WHEN compiling for Linux, THE Build_System SHALL enable Avahi, AppIndicator, and libsecret support
2. WHEN compiling for macOS, THE Build_System SHALL disable Avahi, AppIndicator, and libsecret support
3. WHEN using conditional compilation, THE source code SHALL use `#ifdef __APPLE__` to separate platform-specific implementations
4. WHEN implementing platform-specific features, THE Platform_Abstraction_Layer SHALL provide equivalent functionality on both platforms
5. WHEN modifying shared code, THE changes SHALL not break Linux builds or functionality

### Requirement 11: Service Discovery Alternative

**User Story:** As a user, I want to discover remote desktop services on my network, so that I can connect without manually entering hostnames.

#### Acceptance Criteria

1. WHEN compiling for macOS, THE Build_System SHALL disable Avahi dependency
2. WHEN service discovery is requested on macOS, THE Remmina SHALL use macOS Bonjour APIs (NSNetService or dns_sd.h) for mDNS discovery
3. WHEN Bonjour discovers a service, THE Remmina SHALL display it in the available services list
4. WHEN Bonjour is not available, THE Remmina SHALL allow manual hostname entry without service discovery
5. WHEN compiling for Linux, THE Remmina SHALL continue using Avahi for service discovery

### Requirement 12: System Tray Alternative

**User Story:** As a user, I want Remmina to provide a system tray or menu bar presence, so that I can quickly access the application.

#### Acceptance Criteria

1. WHEN compiling for macOS, THE Build_System SHALL disable AppIndicator dependency
2. WHEN running on macOS, THE Remmina SHALL use GTK StatusIcon for system tray integration
3. WHEN the StatusIcon is clicked, THE Remmina SHALL display the main window or a context menu
4. IF GTK StatusIcon is not available, THEN THE Remmina SHALL function without system tray integration
5. WHEN compiling for Linux, THE Remmina SHALL continue using AppIndicator where available

### Requirement 13: Communication Framework Handling

**User Story:** As a developer, I want to disable Telepathy support on macOS, so that the build does not depend on Linux-specific communication frameworks.

#### Acceptance Criteria

1. WHEN compiling for macOS, THE Build_System SHALL disable Telepathy dependency
2. WHEN Telepathy features are accessed, THE source code SHALL use conditional compilation to skip Telepathy-specific code on macOS
3. WHEN compiling for Linux, THE Remmina SHALL continue supporting Telepathy integration where enabled
4. WHEN Telepathy is disabled, THE Remmina SHALL provide all core remote desktop functionality without communication framework integration

### Requirement 7: Connection Profile Management

**User Story:** As a user, I want to create, save, and load connection profiles, so that I can quickly connect to frequently used remote systems.

#### Acceptance Criteria

1. WHEN creating a connection profile, THE Remmina SHALL allow specifying protocol, hostname, username, and other connection parameters
2. WHEN saving a connection profile, THE Remmina SHALL persist the configuration to disk in the user's config directory
3. WHEN a profile includes a password, THE Remmina SHALL store it in Secret_Storage (macOS Keychain on macOS)
4. WHEN loading a connection profile, THE Remmina SHALL read the configuration and retrieve the password from Secret_Storage
5. WHEN the config directory does not exist, THE Remmina SHALL create it with appropriate permissions
6. WHEN listing profiles, THE Remmina SHALL display all saved Connection_Profiles in the main window

### Requirement 8: User Interface Integration

**User Story:** As a user, I want Remmina to behave like a native macOS application, so that it feels familiar and integrates with my system.

#### Acceptance Criteria

1. WHEN the application launches, THE Remmina SHALL display the main window using GTK3 widgets
2. WHEN using keyboard shortcuts, THE Remmina SHALL respond to standard macOS key combinations where applicable
3. WHEN the application is in the background, THE Remmina SHALL not interfere with other applications
4. WHEN displaying icons, THE Remmina SHALL load icon resources from the Application_Bundle
5. WHEN the user quits the application, THE Remmina SHALL clean up resources and terminate gracefully

### Requirement 9: Error Handling and Logging

**User Story:** As a developer, I want comprehensive error logging, so that I can diagnose issues during development and after deployment.

#### Acceptance Criteria

1. WHEN a build configuration error occurs, THE Build_System SHALL output a clear error message indicating the problem
2. WHEN a plugin fails to load, THE Remmina SHALL log the plugin name and error reason
3. WHEN a Keychain operation fails, THE Platform_Abstraction_Layer SHALL log the OSStatus error code
4. WHEN a connection fails, THE Protocol_Plugin SHALL log the failure reason
5. WHEN debug logging is enabled, THE Remmina SHALL output detailed diagnostic information

### Requirement 10: Testing and Validation

**User Story:** As a developer, I want automated tests for platform-specific code, so that I can verify correctness and prevent regressions.

#### Acceptance Criteria

1. WHEN testing Keychain integration, THE test suite SHALL verify password storage, retrieval, and deletion operations
2. WHEN testing plugin loading, THE test suite SHALL verify that plugins can be discovered and loaded from the bundle
3. WHEN testing connection profiles, THE test suite SHALL verify that profiles can be saved and loaded correctly
4. WHEN testing resource loading, THE test suite SHALL verify that UI files and themes can be located in the bundle
5. WHEN running tests, THE test suite SHALL report pass/fail status for each test case
