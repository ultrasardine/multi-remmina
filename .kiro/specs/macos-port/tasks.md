# Implementation Plan: macOS Port

## Overview

This implementation plan breaks down the macOS port into discrete coding tasks. The approach follows these phases:
1. CMake build system adaptation for macOS
2. Platform abstraction layer implementation (Keychain, Bonjour, StatusIcon)
3. Source code modifications for macOS compatibility
4. Application bundle creation and packaging
5. Testing and validation

Each task builds incrementally, with checkpoints to ensure stability before proceeding.

## Tasks

- [x] 1. Configure CMake build system for macOS
  - Modify root `CMakeLists.txt` to detect macOS platform (Darwin)
  - Set macOS-specific variables: `NO_UNDEFINED_FLAG`, `CMAKE_MACOSX_RPATH`, `CMAKE_INSTALL_RPATH`
  - Add platform-specific feature flags: disable Avahi, AppIndicator, Telepathy on macOS
  - Add Homebrew path search: `/usr/local`, `/opt/homebrew`, `/usr/local/opt/openssl`, `/opt/homebrew/opt/openssl`
  - Find and link macOS frameworks: Security, Cocoa
  - _Requirements: 1.1, 1.2, 1.3, 1.4, 1.5, 1.6_

- [x] 1.1 Write unit tests for CMake configuration
  - Test platform detection sets `APPLE=TRUE` on macOS
  - Test Linux-specific features are disabled on macOS
  - Test macOS frameworks are linked when `WITH_MACOS_KEYCHAIN=ON`
  - _Requirements: 1.1, 1.2, 1.6_

- [x] 2. Update plugin build configuration for macOS
  - Modify `plugins/CMakeLists.txt` to search Homebrew paths for FreeRDP, libvncserver, libssh
  - Ensure plugin RPATH is set correctly for bundle loading
  - Add macOS-specific plugin configuration
  - _Requirements: 1.7, 3.1_

- [x] 2.1 Write unit tests for plugin build configuration
  - Test plugins can be found in Homebrew locations
  - Test plugin RPATH is configured correctly
  - _Requirements: 1.7_

- [x] 3. Implement macOS Keychain abstraction layer
  - [x] 3.1 Create `src/remmina_keychain_macos.h` header file
    - Define function signatures for init, store, get, delete password
    - Use GLib types for consistency with existing code
    - _Requirements: 2.1, 2.2, 2.3_

  - [x] 3.2 Create `src/remmina_keychain_macos.c` implementation
    - Implement `remmina_keychain_macos_init()` (returns TRUE)
    - Implement `remmina_keychain_macos_store_password()` using `SecKeychainAddGenericPassword`
    - Implement `remmina_keychain_macos_get_password()` using `SecKeychainFindGenericPassword`
    - Implement `remmina_keychain_macos_delete_password()` using `SecKeychainItemDelete`
    - Handle OSStatus error codes and convert to GError
    - Wrap all code in `#ifdef __APPLE__` blocks
    - _Requirements: 2.1, 2.2, 2.3, 2.4, 2.5_

  - [x] 3.3 Write property test for Keychain round-trip
    - **Property 1: Keychain Password Round-Trip**
    - **Validates: Requirements 2.1, 2.2, 2.3, 2.6**
    - Generate random service, account, password
    - Store password, retrieve it, verify identical
    - Run 100 iterations

  - [x] 3.4 Write property test for Keychain deletion
    - **Property 2: Keychain Deletion Completeness**
    - **Validates: Requirements 2.3, 2.5**
    - Store password, delete it, verify retrieval fails
    - Run 100 iterations

  - [x] 3.5 Write unit tests for Keychain edge cases
    - Test empty password storage
    - Test special characters in passwords
    - Test non-existent password retrieval
    - Test error handling for Keychain failures
    - _Requirements: 2.4, 2.5_

- [x] 4. Integrate Keychain with secret plugin
  - Modify `plugins/secret/src/glibsecret_plugin.c` to use Keychain on macOS
  - Add `#ifdef __APPLE__` blocks around libsecret code
  - Call `remmina_keychain_macos_*` functions on macOS
  - Maintain libsecret functionality on Linux
  - Update `plugins/secret/CMakeLists.txt` to link Security framework on macOS
  - _Requirements: 2.1, 2.2, 2.3, 6.1, 6.2, 6.4_

- [x] 4.1 Write property test for platform abstraction equivalence
  - **Property 7: Platform Abstraction Functional Equivalence**
  - **Validates: Requirements 6.4**
  - Test password operations work on both macOS and Linux
  - Verify equivalent results across platforms

- [x] 5. Checkpoint - Ensure Keychain integration works
  - Build project on macOS
  - Run Keychain property tests
  - Verify passwords can be stored and retrieved
  - Ask user if questions arise

- [x] 6. Implement Bonjour service discovery for macOS
  - [x] 6.1 Create `src/remmina_bonjour_macos.h` header file
    - Define service discovery interface
    - Define callback type for discovered services
    - _Requirements: 11.2_

  - [x] 6.2 Create `src/remmina_bonjour_macos.c` implementation
    - Implement `remmina_service_discovery_init()` using `dns_sd.h`
    - Implement `remmina_service_discovery_browse()` using `DNSServiceBrowse`
    - Implement service resolution using `DNSServiceResolve`
    - Integrate with GLib main loop using `DNSServiceRefSockFD` and `g_io_add_watch`
    - Wrap all code in `#ifdef __APPLE__` blocks
    - _Requirements: 11.2, 11.3_

  - [x] 6.3 Write property test for service discovery display
    - **Property 8: Service Discovery Display**
    - **Validates: Requirements 11.2, 11.3**
    - Mock Bonjour service discovery
    - Verify discovered services appear in list
    - Run 100 iterations

  - [x] 6.4 Write unit tests for service discovery edge cases
    - Test fallback to manual entry when Bonjour unavailable
    - Test service resolution timeout
    - Test invalid service data handling
    - _Requirements: 11.4_

- [x] 7. Update Avahi code for macOS compatibility
  - Modify `src/remmina_avahi.c` to wrap Avahi code in `#ifndef __APPLE__` blocks
  - Provide stub functions for macOS that call Bonjour implementation
  - Update `src/CMakeLists.txt` to conditionally compile Avahi code
  - _Requirements: 11.1, 11.5, 6.2_

- [x] 8. Implement system tray using GTK StatusIcon on macOS
  - Modify `src/remmina_applet_menu.c` to use StatusIcon on macOS
  - Add `#ifdef __APPLE__` blocks to use `GtkStatusIcon` instead of AppIndicator
  - Implement fallback when StatusIcon is unavailable
  - Maintain AppIndicator functionality on Linux
  - _Requirements: 12.1, 12.2, 12.3, 12.4, 12.5_

- [x] 8.1 Write unit tests for system tray integration
  - Test StatusIcon creation on macOS
  - Test click handler triggers correct action
  - Test fallback when StatusIcon unavailable
  - _Requirements: 12.2, 12.3, 12.4_

- [x] 9. Disable Telepathy support on macOS
  - Wrap Telepathy code in `#ifndef __APPLE__` blocks
  - Update CMakeLists.txt to skip Telepathy on macOS
  - Verify core functionality works without Telepathy
  - _Requirements: 13.1, 13.2, 13.3, 13.4_

- [x] 10. Checkpoint - Ensure platform abstractions work
  - Build project on macOS
  - Test service discovery
  - Test system tray icon
  - Verify no Telepathy dependencies
  - Ask user if questions arise

- [x] 11. Update desktop integration for macOS
  - Modify `src/remmina_main.c` to handle macOS-specific application menu
  - Verify existing `#ifndef __APPLE__` around `gio/gdesktopappinfo.h` is sufficient
  - Disable MIME type registration on macOS (handle differently in bundle)
  - _Requirements: 8.1, 8.2_

- [x] 11.1 Write unit tests for application launch
  - Test main window displays on launch
  - Test keyboard shortcuts work
  - Test graceful quit
  - _Requirements: 8.1, 8.2, 8.5_

- [x] 12. Implement bundle resource path resolution
  - [x] 12.1 Create `src/remmina_bundle_macos.c` with bundle path functions
    - Implement `remmina_get_bundle_resource_path()` using CoreFoundation
    - Implement `remmina_get_resource_dir()` to return bundle Resources path
    - Implement `remmina_get_plugin_dir()` to return bundle plugins path
    - Implement `remmina_get_ui_dir()` to return bundle UI path
    - Wrap all code in `#ifdef __APPLE__` blocks
    - _Requirements: 4.7_

  - [x] 12.2 Update resource loading code to use bundle paths
    - Modify `src/remmina_file_manager.c` to use bundle paths on macOS
    - Modify `src/remmina_plugin_manager.c` to use bundle plugin path on macOS
    - Modify `src/remmina_icon.c` to use bundle resource path on macOS
    - _Requirements: 4.7, 8.4_

  - [x] 12.3 Write property test for bundle resource resolution
    - **Property 4: Bundle Resource Resolution**
    - **Validates: Requirements 4.1, 4.2, 4.3, 4.4, 4.5, 4.6, 4.7**
    - Test all resource types can be located in bundle
    - Run 100 iterations

  - [x] 12.4 Write property test for icon resource loading
    - **Property 9: Icon Resource Loading**
    - **Validates: Requirements 8.4**
    - Test all icons can be loaded from bundle
    - Run 100 iterations

  - [x] 12.5 Write unit tests for resource loading edge cases
    - Test missing UI file handling
    - Test missing theme file handling
    - Test missing icon handling
    - _Requirements: 4.7_

- [x] 13. Update icon and resource handling for macOS
  - Modify `data/CMakeLists.txt` to skip icon cache generation on macOS
  - Disable desktop database update on macOS
  - Ensure resources are installed to correct locations for bundle creation
  - _Requirements: 4.5_

- [x] 14. Implement connection profile management
  - [x] 14.1 Verify profile creation works on macOS
    - Test that profiles can be created with all parameters
    - Ensure config directory is created if needed
    - _Requirements: 7.1, 7.5_

  - [x] 14.2 Verify profile persistence works on macOS
    - Test that profiles are saved to disk
    - Test that passwords are stored in Keychain
    - Test that profiles can be loaded
    - _Requirements: 7.2, 7.3, 7.4_

  - [x] 14.3 Write property test for profile persistence round-trip
    - **Property 5: Connection Profile Persistence Round-Trip**
    - **Validates: Requirements 7.1, 7.2, 7.3, 7.4**
    - Generate random profile data
    - Save profile, load it, verify identical
    - Run 100 iterations

  - [x] 14.4 Write property test for profile listing
    - **Property 6: Profile Listing Completeness**
    - **Validates: Requirements 7.6**
    - Create random set of profiles
    - Verify all appear in list
    - Run 100 iterations

  - [x] 14.5 Write unit tests for profile edge cases
    - Test config directory creation
    - Test profile save failure handling
    - Test profile load failure handling
    - _Requirements: 7.5_

- [x] 15. Verify plugin loading works in bundle
  - Test that plugins can be discovered in bundle plugin directory
  - Test that RDP, VNC, SSH plugins load successfully
  - Test that protocols are registered correctly
  - _Requirements: 3.1, 3.2, 3.3, 3.4_

- [x] 15.1 Write property test for plugin protocol registration
  - **Property 3: Plugin Protocol Registration**
  - **Validates: Requirements 3.1, 3.2, 3.3, 3.4, 3.6**
  - Test loaded plugins register protocols
  - Verify protocols appear in available list
  - Run 100 iterations

- [x] 15.2 Write unit tests for plugin loading edge cases
  - Test plugin loading with missing dependency
  - Test plugin loading with corrupted file
  - Test error logging for failed plugins
  - _Requirements: 3.5_

- [x] 16. Checkpoint - Ensure core functionality works
  - Build project on macOS
  - Launch application
  - Create connection profile
  - Save profile with password
  - Load profile
  - Verify all protocols available
  - Ask user if questions arise

- [x] 17. Create macOS build script
  - [x] 17.1 Create `scripts/macos-build.sh`
    - Check for Homebrew installation
    - Install required dependencies via Homebrew
    - Create build directory
    - Run CMake with macOS-specific options
    - Build with make using all CPU cores
    - _Requirements: 5.1, 5.2, 5.3, 5.5_

  - [x] 17.2 Write unit tests for build script
    - Test dependency checking
    - Test error reporting for missing dependencies
    - _Requirements: 5.4_

- [x] 18. Create application bundle packaging script
  - [x] 18.1 Create `scripts/create-macos-bundle.sh`
    - Create bundle directory structure (Contents/MacOS, Contents/Resources, Contents/Frameworks)
    - Copy executable to Contents/MacOS
    - Copy plugins to Contents/Resources/lib/remmina/plugins
    - Copy UI files to Contents/Resources/ui
    - Copy themes to Contents/Resources/theme
    - Generate Info.plist with version and bundle identifier
    - Convert PNG icon to ICNS format
    - _Requirements: 4.1, 4.2, 4.3, 4.4, 4.5, 4.6_

  - [x] 18.2 Write unit tests for bundle creation
    - Test bundle structure is correct
    - Test Info.plist contains required keys
    - Test executable is in correct location
    - Test plugins are in correct location
    - Test resources are in correct location
    - _Requirements: 4.1, 4.2, 4.3, 4.4, 4.5, 4.6_

- [x] 19. Create build and installation documentation
  - Create `docs/MACOS_BUILD.md` with step-by-step instructions
  - Document Homebrew dependency installation
  - Document build process
  - Document bundle creation process
  - Document known issues and workarounds
  - _Requirements: 5.1_

- [x] 20. Verify Linux builds are not broken
  - Build project on Linux
  - Run Linux test suite
  - Verify Avahi, AppIndicator, libsecret still work
  - Verify all plugins load correctly
  - _Requirements: 6.1, 6.5_

- [x] 21. Final checkpoint - End-to-end testing
  - Build application on macOS using build script
  - Create application bundle using packaging script
  - Launch bundle
  - Create connection profiles for RDP, VNC, SSH
  - Save profiles with passwords
  - Verify passwords stored in Keychain
  - Load profiles
  - Verify passwords retrieved from Keychain
  - Test actual connections to remote systems
  - Test service discovery
  - Test system tray icon
  - Verify all property tests pass
  - Verify all unit tests pass
  - Ask user if questions arise

## Notes

- Each task references specific requirements for traceability
- Checkpoints ensure incremental validation
- Property tests validate universal correctness properties (minimum 100 iterations each)
- Unit tests validate specific examples and edge cases
- All platform-specific code uses `#ifdef __APPLE__` for conditional compilation
- Linux functionality must remain unchanged and working
- All tests are required for comprehensive validation
