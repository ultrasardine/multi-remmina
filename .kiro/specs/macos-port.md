---
title: Port Remmina to macOS
status: draft
created: 2026-01-21
---

# Port Remmina Remote Desktop Client to macOS

## Overview

Port the Remmina GTK+ Remote Desktop Client to run natively on macOS, enabling users to connect to remote desktops using RDP, VNC, SSH, and other protocols from their Mac.

## Goals

- Build and run Remmina on macOS with core functionality
- Support essential protocols: RDP, VNC, SSH/SFTP
- Integrate with macOS system features (Keychain, native menus)
- Maintain code compatibility with Linux version
- Create distributable macOS application bundle

## Non-Goals

- Complete feature parity with Linux version in first iteration
- Support for Linux-specific features (AppIndicator, Telepathy, Avahi)
- Wayland-specific functionality
- Snap/Flatpak packaging for macOS

## Requirements Analysis

### Current macOS Support
- Some `__APPLE__` conditionals already exist in codebase
- CMake has Darwin-specific configuration
- Core dependencies available via Homebrew

### Dependencies Status on macOS

#### Available via Homebrew
- ✅ GTK3 (`gtk+3`)
- ✅ GLib (`glib`)
- ✅ libsodium (`libsodium`)
- ✅ OpenSSL (`openssl`)
- ✅ libssh (`libssh`)
- ✅ FreeRDP (`freerdp`)
- ✅ libvncserver (`libvncserver`)
- ✅ webkit2gtk (`webkitgtk`)
- ✅ spice-gtk (`spice-gtk`)
- ✅ CMake (`cmake`)
- ✅ pkg-config (`pkg-config`)

#### Linux-Specific (Need Alternatives/Disable)
- ❌ Avahi (mDNS discovery) - Use macOS Bonjour APIs or disable
- ❌ AppIndicator (system tray) - Use GTK StatusIcon or native macOS menu bar
- ❌ GnomeKeyring - Use macOS Keychain via Security framework
- ❌ Telepathy - Disable (communication framework)
- ❌ libsecret - Replace with macOS Keychain

## Implementation Plan

### Phase 1: Build System Adaptation

#### Task 1.1: Update CMake Configuration
**Files:** `CMakeLists.txt`, `cmake/ConfigOptions.cmake`

**Changes:**
- Add macOS-specific build options
- Disable Linux-only features by default on macOS
- Configure proper RPATH for macOS bundles
- Handle framework linking for macOS

**Implementation:**
```cmake
# In CMakeLists.txt, add after platform detection:

if(APPLE)
    message(STATUS "Configuring for macOS")
    
    # Disable Linux-specific features
    option(WITH_AVAHI "Enable Avahi support" OFF)
    option(WITH_APPINDICATOR "Enable AppIndicator support" OFF)
    option(WITH_TELEPATHY "Enable Telepathy support" OFF)
    
    # macOS-specific options
    option(WITH_MACOS_KEYCHAIN "Use macOS Keychain for password storage" ON)
    set(CMAKE_MACOSX_RPATH ON)
    set(CMAKE_INSTALL_RPATH "@executable_path/../Frameworks;@executable_path/../lib")
    
    # Find macOS frameworks
    find_library(SECURITY_FRAMEWORK Security)
    find_library(COCOA_FRAMEWORK Cocoa)
    
else()
    # Linux defaults
    option(WITH_AVAHI "Enable Avahi support" ON)
    option(WITH_APPINDICATOR "Enable AppIndicator support" ON)
endif()
```

#### Task 1.2: Update Plugin Build Configuration
**Files:** `plugins/CMakeLists.txt`

**Changes:**
- Ensure FreeRDP detection works on macOS
- Configure VNC plugin for macOS
- Disable incompatible plugins

**Implementation:**
```cmake
# In plugins/CMakeLists.txt, update dependency checks:

if(APPLE)
    # macOS-specific plugin configuration
    message(STATUS "Configuring plugins for macOS")
    
    # Ensure Homebrew paths are checked
    list(APPEND CMAKE_PREFIX_PATH 
        "/usr/local" 
        "/opt/homebrew"
        "/usr/local/opt/openssl"
        "/opt/homebrew/opt/openssl")
endif()
```

### Phase 2: Platform Abstraction Layer

#### Task 2.1: Create macOS Keychain Integration
**New Files:** 
- `src/remmina_keychain_macos.c`
- `src/remmina_keychain_macos.h`

**Purpose:** Replace libsecret/gnome-keyring with macOS Keychain

**Implementation:**
```c
// src/remmina_keychain_macos.h
#ifndef REMMINA_KEYCHAIN_MACOS_H
#define REMMINA_KEYCHAIN_MACOS_H

#ifdef __APPLE__

#include <glib.h>

gboolean remmina_keychain_macos_init(void);
gboolean remmina_keychain_macos_store_password(
    const gchar *service,
    const gchar *account,
    const gchar *password,
    GError **error);
gchar* remmina_keychain_macos_get_password(
    const gchar *service,
    const gchar *account,
    GError **error);
gboolean remmina_keychain_macos_delete_password(
    const gchar *service,
    const gchar *account,
    GError **error);

#endif /* __APPLE__ */
#endif /* REMMINA_KEYCHAIN_MACOS_H */
```

```c
// src/remmina_keychain_macos.c
#ifdef __APPLE__

#include "remmina_keychain_macos.h"
#include <Security/Security.h>
#include <CoreFoundation/CoreFoundation.h>

gboolean remmina_keychain_macos_init(void) {
    return TRUE;
}

gboolean remmina_keychain_macos_store_password(
    const gchar *service,
    const gchar *account,
    const gchar *password,
    GError **error) {
    
    OSStatus status;
    
    // First try to delete existing item
    remmina_keychain_macos_delete_password(service, account, NULL);
    
    // Add new password
    status = SecKeychainAddGenericPassword(
        NULL,  // default keychain
        strlen(service), service,
        strlen(account), account,
        strlen(password), password,
        NULL);
    
    if (status != errSecSuccess) {
        if (error) {
            *error = g_error_new(
                G_IO_ERROR,
                G_IO_ERROR_FAILED,
                "Failed to store password in Keychain: %d",
                (int)status);
        }
        return FALSE;
    }
    
    return TRUE;
}

gchar* remmina_keychain_macos_get_password(
    const gchar *service,
    const gchar *account,
    GError **error) {
    
    OSStatus status;
    void *passwordData = NULL;
    UInt32 passwordLength = 0;
    gchar *password = NULL;
    
    status = SecKeychainFindGenericPassword(
        NULL,  // default keychain
        strlen(service), service,
        strlen(account), account,
        &passwordLength, &passwordData,
        NULL);
    
    if (status == errSecSuccess) {
        password = g_strndup(passwordData, passwordLength);
        SecKeychainItemFreeContent(NULL, passwordData);
    } else if (status == errSecItemNotFound) {
        if (error) {
            *error = g_error_new(
                G_IO_ERROR,
                G_IO_ERROR_NOT_FOUND,
                "Password not found in Keychain");
        }
    } else {
        if (error) {
            *error = g_error_new(
                G_IO_ERROR,
                G_IO_ERROR_FAILED,
                "Failed to retrieve password from Keychain: %d",
                (int)status);
        }
    }
    
    return password;
}

gboolean remmina_keychain_macos_delete_password(
    const gchar *service,
    const gchar *account,
    GError **error) {
    
    OSStatus status;
    SecKeychainItemRef itemRef = NULL;
    
    status = SecKeychainFindGenericPassword(
        NULL,
        strlen(service), service,
        strlen(account), account,
        NULL, NULL,
        &itemRef);
    
    if (status == errSecSuccess && itemRef) {
        status = SecKeychainItemDelete(itemRef);
        CFRelease(itemRef);
        
        if (status != errSecSuccess) {
            if (error) {
                *error = g_error_new(
                    G_IO_ERROR,
                    G_IO_ERROR_FAILED,
                    "Failed to delete password from Keychain: %d",
                    (int)status);
            }
            return FALSE;
        }
    }
    
    return TRUE;
}

#endif /* __APPLE__ */
```

#### Task 2.2: Update Secret Plugin for macOS
**Files:** `plugins/secret/src/remmina_plugin_secret.c`

**Changes:**
- Add conditional compilation for macOS Keychain
- Maintain libsecret for Linux

**Implementation:**
```c
// Add to top of file:
#ifdef __APPLE__
#include "remmina_keychain_macos.h"
#endif

// Wrap existing libsecret code:
#ifndef __APPLE__
// ... existing libsecret code ...
#else
// Use macOS Keychain implementation
#endif
```

#### Task 2.3: Disable Avahi on macOS
**Files:** `src/remmina_avahi.c`, `src/CMakeLists.txt`

**Changes:**
- Wrap Avahi code in `#ifndef __APPLE__`
- Provide stub functions for macOS
- Update CMakeLists to skip Avahi on macOS

### Phase 3: Source Code Modifications

#### Task 3.1: Fix Desktop Integration
**Files:** `src/remmina_main.c`

**Changes:**
- Already has `#ifndef __APPLE__` around `gio/gdesktopappinfo.h`
- Add macOS-specific application menu handling
- Disable MIME type registration on macOS (handle differently)

#### Task 3.2: Update Icon and Resource Handling
**Files:** `src/remmina_icon.c`, `data/CMakeLists.txt`

**Changes:**
- Ensure icon loading works with macOS bundle structure
- Update resource paths for macOS app bundle
- Disable icon cache generation on macOS

#### Task 3.3: Fix System Tray Integration
**Files:** `src/remmina_applet_menu.c`

**Changes:**
- Use GTK StatusIcon instead of AppIndicator on macOS
- Or implement native macOS menu bar integration

### Phase 4: Build and Packaging

#### Task 4.1: Create Build Script for macOS
**New File:** `scripts/macos-build.sh`

**Purpose:** Automated build script using Homebrew dependencies

**Implementation:**
```bash
#!/bin/bash
set -e

echo "Building Remmina for macOS"

# Check for Homebrew
if ! command -v brew &> /dev/null; then
    echo "Error: Homebrew is required. Install from https://brew.sh"
    exit 1
fi

# Install dependencies
echo "Installing dependencies..."
brew install gtk+3 glib libsodium openssl libssh freerdp libvncserver \
    cmake pkg-config gettext json-glib libsoup webkitgtk spice-gtk \
    harfbuzz

# Set up build directory
BUILD_DIR="build-macos"
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake
echo "Configuring build..."
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DWITH_APPINDICATOR=OFF \
    -DWITH_AVAHI=OFF \
    -DWITH_TELEPATHY=OFF \
    -DWITH_GNOMEKEYRING=OFF \
    -DWITH_MACOS_KEYCHAIN=ON \
    -DWITH_FREERDP3=OFF \
    -DWITH_TRANSLATIONS=ON \
    -DWITH_MANPAGES=OFF \
    -DWITH_ICON_CACHE=OFF \
    -DWITH_UPDATE_DESKTOP_DB=OFF

# Build
echo "Building..."
make -j$(sysctl -n hw.ncpu)

echo "Build complete! Binary is in $BUILD_DIR/src/remmina"
```

#### Task 4.2: Create macOS Application Bundle
**New File:** `scripts/create-macos-bundle.sh`

**Purpose:** Package Remmina as a macOS .app bundle

**Implementation:**
```bash
#!/bin/bash
set -e

APP_NAME="Remmina"
BUNDLE_DIR="$APP_NAME.app"
CONTENTS_DIR="$BUNDLE_DIR/Contents"
MACOS_DIR="$CONTENTS_DIR/MacOS"
RESOURCES_DIR="$CONTENTS_DIR/Resources"
FRAMEWORKS_DIR="$CONTENTS_DIR/Frameworks"

echo "Creating macOS application bundle..."

# Create bundle structure
rm -rf "$BUNDLE_DIR"
mkdir -p "$MACOS_DIR" "$RESOURCES_DIR" "$FRAMEWORKS_DIR"

# Copy binary
cp build-macos/src/remmina "$MACOS_DIR/"

# Copy plugins
mkdir -p "$RESOURCES_DIR/lib/remmina/plugins"
cp build-macos/plugins/*/*.so "$RESOURCES_DIR/lib/remmina/plugins/" || true

# Copy resources
cp -r data/ui "$RESOURCES_DIR/"
cp -r data/theme "$RESOURCES_DIR/"

# Create Info.plist
cat > "$CONTENTS_DIR/Info.plist" << 'EOF'
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
    <key>CFBundleDisplayName</key>
    <string>Remmina</string>
    <key>CFBundleVersion</key>
    <string>1.4.41</string>
    <key>CFBundleShortVersionString</key>
    <string>1.4.41</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleSignature</key>
    <string>????</string>
    <key>NSHighResolutionCapable</key>
    <true/>
    <key>LSMinimumSystemVersion</key>
    <string>10.15</string>
</dict>
</plist>
EOF

# Copy icon (if available)
if [ -f "data/desktop/512x512/org.remmina.Remmina.png" ]; then
    # Convert PNG to ICNS (requires iconutil)
    mkdir -p Remmina.iconset
    cp data/desktop/512x512/org.remmina.Remmina.png Remmina.iconset/icon_512x512.png
    iconutil -c icns Remmina.iconset -o "$RESOURCES_DIR/Remmina.icns"
    rm -rf Remmina.iconset
fi

# Bundle GTK and dependencies using dylibbundler or manual copying
echo "Bundling dependencies..."
# This would need dylibbundler or similar tool

echo "Bundle created: $BUNDLE_DIR"
```

#### Task 4.3: Create Installation Documentation
**New File:** `docs/MACOS_BUILD.md`

**Content:** Step-by-step instructions for building on macOS

### Phase 5: Testing and Validation

#### Task 5.1: Test Core Functionality
- Launch application
- Create connection profiles
- Test password storage/retrieval
- Verify preferences save/load

#### Task 5.2: Test Protocol Plugins
- RDP connection to Windows machine
- VNC connection to Linux/macOS machine
- SSH connection and terminal
- SFTP file transfer

#### Task 5.3: Test macOS Integration
- Application menu behavior
- Keyboard shortcuts
- Window management
- System tray/menu bar icon

## Dependencies

### Build Dependencies
```bash
brew install \
    gtk+3 \
    glib \
    libsodium \
    openssl \
    libssh \
    freerdp \
    libvncserver \
    cmake \
    pkg-config \
    gettext \
    json-glib \
    libsoup \
    webkitgtk \
    spice-gtk \
    harfbuzz
```

### Runtime Dependencies
- Same as build dependencies (dynamically linked)
- macOS 10.15 (Catalina) or later

## Risks and Mitigations

### Risk 1: GTK3 Performance on macOS
**Impact:** Medium
**Mitigation:** GTK3 is mature on macOS via Homebrew. Performance should be acceptable.

### Risk 2: FreeRDP Compatibility
**Impact:** Medium  
**Mitigation:** FreeRDP is actively maintained for macOS. Test thoroughly with various RDP servers.

### Risk 3: Bundle Size
**Impact:** Low
**Mitigation:** GTK dependencies will make bundle large (~200MB). Consider using system libraries where possible.

### Risk 4: Code Signing and Notarization
**Impact:** Medium
**Mitigation:** Will need Apple Developer account for distribution. Document process for users building locally.

## Success Criteria

- [ ] Remmina builds successfully on macOS without errors
- [ ] Application launches and displays main window
- [ ] Can create and save connection profiles
- [ ] RDP plugin connects to Windows machines
- [ ] VNC plugin connects to remote desktops
- [ ] SSH plugin provides terminal access
- [ ] Passwords stored securely in macOS Keychain
- [ ] Application bundle can be distributed to other Macs
- [ ] Documentation complete for building and installing

## Timeline Estimate

- Phase 1 (Build System): 2-3 days
- Phase 2 (Platform Abstraction): 3-4 days
- Phase 3 (Source Modifications): 2-3 days
- Phase 4 (Packaging): 2-3 days
- Phase 5 (Testing): 2-3 days

**Total: 11-16 days** (assuming full-time work)

## Future Enhancements

- Native macOS UI using Swift/AppKit (long-term)
- Bonjour integration for service discovery
- Touch Bar support
- Handoff support between Apple devices
- Mac App Store distribution
- Universal binary (Intel + Apple Silicon)

## References

- [Remmina GitLab](https://gitlab.com/Remmina/Remmina)
- [GTK on macOS](https://www.gtk.org/docs/installations/macos/)
- [FreeRDP](https://www.freerdp.com/)
- [macOS Keychain Services](https://developer.apple.com/documentation/security/keychain_services)
