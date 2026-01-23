# Technology Stack

## Build System
- **CMake** (minimum version 3.10.0)
- Build type: Release (default) or Debug
- Shared libraries enabled by default

## Core Technologies
- **Language**: C (primary), C++ (some components)
- **UI Framework**: GTK3 (minimum 3.14.0)
- **GLib/GObject**: Core library dependencies

## Key Dependencies
- GTK3 (3.14.0+)
- GLib2
- OpenSSL or libgcrypt (encryption)
- libsodium (required for crypto operations)
- libssh (SSH protocol support)
- Wayland client (optional, for Wayland support)
- Avahi (optional, for mDNS/DNS-SD discovery on Linux)
- FreeRDP (for RDP plugin)
- libvncclient (for VNC plugin)
- SPICE libraries (for SPICE plugin)

## Common Build Commands

### Configure and Build
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

### Install
```bash
sudo make install
```

### Build with Debug Symbols
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

### Uninstall
```bash
make uninstall
```

## Build Options
CMake options are defined in `cmake/ConfigOptions.cmake`. Common options:
- `WITH_TRANSLATIONS=ON` - Enable translations
- `WITH_AVAHI=ON` - Enable Avahi support (Linux only)
- `WITH_NEWS=ON` - Enable online news check
- `WITH_STATS=ON` - Enable statistics
- `WITH_MANPAGES=ON` - Build man pages
- `WITH_ICON_CACHE=ON` - Generate icon cache during install
- `SNAP_BUILD=ON` - Build for Snap packaging
- `WITH_GCC_SANITIZE_ADDRESS=ON` - Enable address sanitizer (debug)

## Platform-Specific Notes

### FreeBSD
- Uses `/usr/local/include` and `/usr/local/lib`
- Requires explicit include/link directory configuration

### macOS
- Port in progress (see `.kiro/specs/macos-port.md`)
- Different linker flags: `-Wl,-undefined,error`

### Linux
- Standard GNU toolchain
- Linker flags: `-Wl,--no-undefined`

## Compiler Flags
- **GCC**: `-Wall`, `-Werror=incompatible-pointer-types`, `-fPIC` (x86_64)
- **Debug**: `-O0`, `-g`, warnings enabled
- **Release**: Optimizations enabled, some warnings suppressed
- **Clang**: Additional warning suppressions for release builds

## Testing & Debugging
- Address sanitizer: `WITH_GCC_SANITIZE_ADDRESS=ON`
- Thread sanitizer: `WITH_GCC_SANITIZE_THREAD=ON`
- LLVM coverage: `WITH_CLANG_COVERAGE=ON` (Clang only)
- GCC profiling: `WITH_GCC_PROFILING=ON`
