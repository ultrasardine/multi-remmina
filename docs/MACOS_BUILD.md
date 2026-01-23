# Building Remmina on macOS

This guide provides step-by-step instructions for building and packaging Remmina on macOS.

## Table of Contents

- [Prerequisites](#prerequisites)
- [Quick Start](#quick-start)
- [Detailed Build Instructions](#detailed-build-instructions)
- [Creating an Application Bundle](#creating-an-application-bundle)
- [Manual Build Process](#manual-build-process)
- [Troubleshooting](#troubleshooting)
- [Known Issues and Workarounds](#known-issues-and-workarounds)
- [Testing](#testing)
- [Advanced Configuration](#advanced-configuration)

## Prerequisites

### System Requirements

- **macOS**: 10.15 (Catalina) or later
- **Xcode Command Line Tools**: Required for compilation
- **Homebrew**: Package manager for installing dependencies

### Installing Xcode Command Line Tools

If you don't have Xcode Command Line Tools installed:

```bash
xcode-select --install
```

### Installing Homebrew

If you don't have Homebrew installed, install it from [https://brew.sh/](https://brew.sh/):

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

After installation, follow the instructions to add Homebrew to your PATH.

## Quick Start

The fastest way to build Remmina on macOS is using the automated build script:

```bash
# Clone the repository (if you haven't already)
git clone https://gitlab.com/Remmina/Remmina.git
cd Remmina

# Run the build script
./scripts/macos-build.sh

# Create the application bundle
./scripts/create-macos-bundle.sh

# Launch Remmina
open Remmina.app
```

That's it! The scripts will handle dependency installation, building, and packaging.

## Detailed Build Instructions

### Step 1: Install Dependencies

The build script automatically installs all required dependencies via Homebrew. If you prefer to install them manually:

#### Required Dependencies

```bash
brew install cmake pkg-config gtk+3 glib libsodium openssl@3 \
             libssh freerdp libvncserver gettext json-glib libgcrypt
```

#### Optional Dependencies

```bash
brew install libsoup@2 vte3 webkit2gtk
```

### Step 2: Build Remmina

#### Using the Build Script (Recommended)

```bash
./scripts/macos-build.sh
```

The script supports several options:

```bash
# Build in debug mode
./scripts/macos-build.sh -t Debug

# Use a custom build directory
./scripts/macos-build.sh -d build-custom

# Skip dependency installation (if already installed)
./scripts/macos-build.sh --skip-deps

# Only run CMake configuration without building
./scripts/macos-build.sh --cmake-only

# Show help
./scripts/macos-build.sh --help
```

#### Build Output

After a successful build, you'll find:
- **Executable**: `build/src/remmina`
- **Plugins**: `build/plugins/*/remmina-plugin-*.so`

### Step 3: Create Application Bundle

To create a standard macOS .app bundle:

```bash
./scripts/create-macos-bundle.sh
```

This creates `Remmina.app` in the project root directory.

#### Bundle Creation Options

```bash
# Specify custom build directory
./scripts/create-macos-bundle.sh -b build-custom

# Specify output location
./scripts/create-macos-bundle.sh -o ~/Desktop

# Specify bundle name
./scripts/create-macos-bundle.sh -n MyRemmina.app

# Specify version
./scripts/create-macos-bundle.sh -v 1.5.0

# Show help
./scripts/create-macos-bundle.sh --help
```

### Step 4: Launch Remmina

```bash
# Launch from command line
open Remmina.app

# Or double-click Remmina.app in Finder
```

## Creating an Application Bundle

The application bundle contains all necessary files for Remmina to run as a standalone macOS application.

### Bundle Structure

```
Remmina.app/
├── Contents/
│   ├── Info.plist              # Bundle metadata
│   ├── MacOS/
│   │   └── remmina             # Executable
│   ├── Resources/
│   │   ├── Remmina.icns        # Application icon
│   │   ├── ui/                 # Glade UI definition files
│   │   ├── theme/              # Terminal color schemes
│   │   └── lib/
│   │       └── remmina/
│   │           └── plugins/    # Protocol plugins (.so files)
│   └── Frameworks/             # Bundled libraries (optional)
```

### What Gets Bundled

The bundle creation script automatically includes:

1. **Executable**: The main Remmina binary
2. **Plugins**: All protocol plugins (RDP, VNC, SSH, etc.)
3. **UI Files**: GTK interface definitions
4. **Themes**: Terminal color schemes
5. **Icon**: Application icon in ICNS format
6. **Metadata**: Info.plist with version and bundle information

### Bundle Verification

The script automatically verifies the bundle structure. You can manually verify:

```bash
# Check bundle structure
ls -la Remmina.app/Contents/

# Check executable
file Remmina.app/Contents/MacOS/remmina

# Check plugins
ls -la Remmina.app/Contents/Resources/lib/remmina/plugins/

# Check Info.plist
plutil -lint Remmina.app/Contents/Info.plist
```

## Manual Build Process

If you prefer to build manually without the scripts:

### 1. Configure with CMake

```bash
mkdir build && cd build

cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DWITH_AVAHI=OFF \
    -DWITH_APPINDICATOR=OFF \
    -DWITH_TELEPATHY=OFF \
    -DWITH_MACOS_KEYCHAIN=ON \
    -DCMAKE_PREFIX_PATH="$(brew --prefix)" \
    -DOPENSSL_ROOT_DIR="$(brew --prefix openssl@3)" \
    ..
```

### 2. Build

```bash
make -j$(sysctl -n hw.ncpu)
```

### 3. Install (Optional)

```bash
sudo make install
```

## Troubleshooting

### Common Issues

#### 1. Homebrew Not Found

**Error**: `brew: command not found`

**Solution**: Install Homebrew or add it to your PATH:

```bash
# For Apple Silicon Macs
export PATH="/opt/homebrew/bin:$PATH"

# For Intel Macs
export PATH="/usr/local/bin:$PATH"
```

Add this to your `~/.zshrc` or `~/.bash_profile` to make it permanent.

#### 2. CMake Cannot Find Dependencies

**Error**: `Could not find GTK3` or similar

**Solution**: Ensure Homebrew packages are installed and CMake can find them:

```bash
# Verify installations
brew list gtk+3 glib libsodium

# Set CMAKE_PREFIX_PATH
export CMAKE_PREFIX_PATH="$(brew --prefix)"
```

#### 3. OpenSSL Not Found

**Error**: `Could not find OpenSSL`

**Solution**: Explicitly set OpenSSL path:

```bash
export OPENSSL_ROOT_DIR="$(brew --prefix openssl@3)"
```

#### 4. FreeRDP Version Mismatch

**Error**: `FreeRDP version not supported`

**Solution**: Install the correct FreeRDP version:

```bash
brew uninstall freerdp
brew install freerdp
```

#### 5. Plugin Loading Fails

**Error**: Plugins not loading at runtime

**Solution**: Check RPATH configuration:

```bash
# Check executable RPATH
otool -l build/src/remmina | grep -A 3 LC_RPATH

# Should show: @executable_path/../Frameworks and @executable_path/../lib
```

#### 6. GTK Warnings at Runtime

**Warning**: `Gtk-WARNING **: Theme parsing error`

**Solution**: These are usually harmless. To suppress:

```bash
export GTK_THEME=Adwaita
```

#### 7. Icon Not Displaying

**Error**: Bundle icon doesn't show in Finder

**Solution**: Rebuild the icon cache:

```bash
# Clear icon cache
sudo rm -rf /Library/Caches/com.apple.iconservices.store
killall Finder
```

#### 8. Build Fails with "No Space Left"

**Error**: Build fails due to disk space

**Solution**: Clean build directory and rebuild:

```bash
rm -rf build
./scripts/macos-build.sh
```

### Debug Build

For debugging issues, build in Debug mode:

```bash
./scripts/macos-build.sh -t Debug -d build-debug
```

This enables:
- Debug symbols
- Verbose logging
- Address sanitizer (optional)

### Verbose Build Output

To see detailed compilation commands:

```bash
cd build
cmake -DCMAKE_VERBOSE_MAKEFILE=ON ..
make VERBOSE=1
```

## Known Issues and Workarounds

### 1. Service Discovery (Bonjour)

**Issue**: Network service discovery may not work immediately after launch.

**Workaround**: 
- Manually enter hostnames instead of relying on auto-discovery
- Restart the application if services don't appear

**Status**: Under investigation

### 2. System Tray Icon

**Issue**: System tray icon may not appear on some macOS versions.

**Workaround**:
- Use the Dock icon instead
- Access Remmina from Applications folder

**Status**: GTK StatusIcon has limited support on modern macOS

### 3. Keychain Access Prompts

**Issue**: macOS may repeatedly prompt for Keychain access.

**Workaround**:
- Click "Always Allow" when prompted
- Grant Keychain access in System Preferences > Security & Privacy

**Status**: Expected behavior for first-time access

### 4. High DPI Display Issues

**Issue**: UI elements may appear too small or too large on Retina displays.

**Workaround**:
```bash
# Set GTK scaling factor
export GDK_SCALE=2
export GDK_DPI_SCALE=0.5
```

**Status**: GTK3 scaling on macOS is limited

### 5. Clipboard Sharing

**Issue**: Clipboard sharing between local and remote systems may not work reliably.

**Workaround**:
- Use file transfer instead
- Copy/paste text in smaller chunks

**Status**: Protocol-dependent limitation

### 6. Wayland Support

**Issue**: Wayland-specific features are not available on macOS.

**Workaround**: Not applicable - macOS uses its own display server

**Status**: Expected - Wayland is Linux-specific

### 7. AppIndicator Features

**Issue**: Linux AppIndicator features are not available.

**Workaround**: Use GTK StatusIcon or Dock icon

**Status**: Expected - AppIndicator is Linux-specific

### 8. Avahi/mDNS Discovery

**Issue**: Avahi-based service discovery is not available.

**Workaround**: Bonjour (macOS native) is used instead

**Status**: Expected - platform-specific implementation

### 9. Telepathy Integration

**Issue**: Telepathy communication framework is not available.

**Workaround**: Not needed - Telepathy is Linux-specific

**Status**: Expected - disabled on macOS

### 10. Library Dependencies

**Issue**: Some Homebrew libraries may conflict with system libraries.

**Workaround**:
```bash
# Use Homebrew libraries explicitly
export DYLD_LIBRARY_PATH="$(brew --prefix)/lib:$DYLD_LIBRARY_PATH"
```

**Status**: Rare, usually auto-resolved by CMake

## Testing

### Running Tests

If tests are enabled during build:

```bash
cd build
make test
```

### Manual Testing Checklist

After building, verify these features work:

- [ ] Application launches without errors
- [ ] Main window displays correctly
- [ ] Can create a new connection profile
- [ ] Can save connection profile
- [ ] Password is stored in macOS Keychain
- [ ] Can load saved connection profile
- [ ] Password is retrieved from Keychain
- [ ] RDP plugin is available
- [ ] VNC plugin is available
- [ ] SSH plugin is available
- [ ] Can connect to a remote system (if available)
- [ ] Preferences dialog opens
- [ ] About dialog displays version information

### Keychain Testing

To verify Keychain integration:

```bash
# Create a test connection with password
# Then check Keychain Access.app for "Remmina" entries
open -a "Keychain Access"
```

Search for "Remmina" in Keychain Access to see stored passwords.

## Advanced Configuration

### Custom CMake Options

You can customize the build with additional CMake options:

```bash
cmake \
    -DCMAKE_BUILD_TYPE=Debug \
    -DWITH_TRANSLATIONS=ON \
    -DWITH_NEWS=OFF \
    -DWITH_STATS=OFF \
    -DWITH_MANPAGES=OFF \
    ..
```

### Available CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `CMAKE_BUILD_TYPE` | Release | Build type (Release/Debug) |
| `WITH_AVAHI` | OFF (macOS) | Enable Avahi support |
| `WITH_APPINDICATOR` | OFF (macOS) | Enable AppIndicator |
| `WITH_TELEPATHY` | OFF (macOS) | Enable Telepathy |
| `WITH_MACOS_KEYCHAIN` | ON (macOS) | Enable macOS Keychain |
| `WITH_TRANSLATIONS` | ON | Enable translations |
| `WITH_NEWS` | ON | Enable online news check |
| `WITH_STATS` | ON | Enable statistics |
| `WITH_MANPAGES` | ON | Build man pages |

### Environment Variables

Useful environment variables for building and running:

```bash
# CMake configuration
export CMAKE_PREFIX_PATH="$(brew --prefix)"
export OPENSSL_ROOT_DIR="$(brew --prefix openssl@3)"

# GTK configuration
export GTK_THEME=Adwaita
export GDK_SCALE=2
export GDK_DPI_SCALE=0.5

# Library paths
export DYLD_LIBRARY_PATH="$(brew --prefix)/lib"

# Debug logging
export G_MESSAGES_DEBUG=all
export REMMINA_DEBUG=1
```

### Building Specific Plugins

To build only specific plugins:

```bash
cmake \
    -DWITH_VNC=ON \
    -DWITH_RDP=ON \
    -DWITH_SSH=ON \
    -DWITH_SPICE=OFF \
    -DWITH_WWW=OFF \
    ..
```

### Cross-Compilation

Cross-compilation is not currently supported. Build on the target macOS version.

### Universal Binary (Apple Silicon + Intel)

To create a universal binary:

```bash
cmake \
    -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" \
    ..
```

Note: All dependencies must also be universal binaries.

## Additional Resources

- **Project Website**: [https://www.remmina.org/](https://www.remmina.org/)
- **GitLab Repository**: [https://gitlab.com/Remmina/Remmina](https://gitlab.com/Remmina/Remmina)
- **Issue Tracker**: [https://gitlab.com/Remmina/Remmina/-/issues](https://gitlab.com/Remmina/Remmina/-/issues)
- **Wiki**: [https://gitlab.com/Remmina/Remmina/-/wikis/home](https://gitlab.com/Remmina/Remmina/-/wikis/home)
- **IRC**: #remmina on libera.chat
- **Matrix**: [https://riot.im/app/#/group/+remmina:matrix.org](https://riot.im/app/#/group/+remmina:matrix.org)

## Contributing

If you encounter issues with the macOS build or have improvements to suggest:

1. Check existing issues: [https://gitlab.com/Remmina/Remmina/-/issues](https://gitlab.com/Remmina/Remmina/-/issues)
2. Create a new issue with:
   - macOS version
   - Build output/error messages
   - Steps to reproduce
3. Submit merge requests with fixes or improvements

See [CONTRIBUTING.md](../CONTRIBUTING.md) for more details.

## License

Remmina is licensed under GPLv2+ with OpenSSL linking exception.

See [COPYING](../COPYING) and [LICENSE.OpenSSL](../LICENSE.OpenSSL) for details.

---

**Last Updated**: January 2026  
**Remmina Version**: 1.4.41+  
**Minimum macOS**: 10.15 (Catalina)
