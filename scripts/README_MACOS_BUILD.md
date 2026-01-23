# macOS Build Script

The `macos-build.sh` script automates the process of building Remmina on macOS with all required dependencies.

## Quick Start

```bash
# Build with default settings
./scripts/macos-build.sh

# Build in debug mode
./scripts/macos-build.sh -t Debug

# Build in custom directory
./scripts/macos-build.sh -d build-custom

# Skip dependency installation (if already installed)
./scripts/macos-build.sh --skip-deps
```

## Features

- **Automatic Dependency Installation**: Installs all required Homebrew packages
- **Homebrew Detection**: Checks for Homebrew and provides installation instructions if missing
- **Parallel Build**: Uses all available CPU cores for faster compilation
- **Error Handling**: Exits immediately on any error with clear messages
- **Flexible Options**: Supports custom build directories and build types
- **macOS-Specific Configuration**: Automatically applies correct CMake flags for macOS

## Dependencies Installed

### Required Packages
- cmake
- pkg-config
- gtk+3
- glib
- libsodium
- openssl@3
- libssh
- freerdp
- libvncserver
- gettext
- json-glib
- libgcrypt

### Optional Packages
- libsoup@2
- vte3
- webkit2gtk

## Usage

```
Usage: ./scripts/macos-build.sh [OPTIONS]

Build Remmina on macOS with Homebrew dependencies.

OPTIONS:
    -h, --help              Show this help message
    -d, --build-dir DIR     Specify build directory (default: build)
    -t, --build-type TYPE   Specify build type: Release or Debug (default: Release)
    --skip-deps             Skip dependency installation
    --cmake-only            Only run CMake configuration, don't build

EXAMPLES:
    ./scripts/macos-build.sh                      # Build with default settings
    ./scripts/macos-build.sh -d build-debug -t Debug  # Debug build in custom directory
    ./scripts/macos-build.sh --skip-deps          # Build without checking/installing dependencies
```

## CMake Options Applied

The script automatically applies these macOS-specific CMake options:

- `CMAKE_BUILD_TYPE`: Release or Debug
- `CMAKE_INSTALL_PREFIX`: /usr/local
- `WITH_AVAHI`: OFF (Linux-specific)
- `WITH_APPINDICATOR`: OFF (Linux-specific)
- `WITH_TELEPATHY`: OFF (Linux-specific)
- `WITH_MACOS_KEYCHAIN`: ON (macOS Keychain support)
- `CMAKE_PREFIX_PATH`: Homebrew prefix
- `OPENSSL_ROOT_DIR`: Homebrew OpenSSL location

## Output

After successful build, the executable will be located at:
```
build/src/remmina
```

## Troubleshooting

### Homebrew Not Found
If you see "Homebrew is not installed", install it from https://brew.sh/:
```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

### Dependency Installation Fails
If a specific package fails to install, try installing it manually:
```bash
brew install <package-name>
```

### Build Fails
1. Check that all dependencies are installed: `brew list`
2. Try cleaning the build directory: `rm -rf build`
3. Run with verbose output: Add `-DCMAKE_VERBOSE_MAKEFILE=ON` to CMake options

## Testing

Unit tests for the build script are available:
```bash
./tests/build_script/test_macos_build_script.sh
```

## Requirements

Validates requirements 5.1, 5.2, 5.3, 5.4, and 5.5 from the macOS port specification.
