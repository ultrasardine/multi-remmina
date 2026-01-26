# Multi-Remmina

[![Build Status](https://github.com/ultrasardine/multi-remmina/actions/workflows/build.yml/badge.svg)](https://github.com/ultrasardine/multi-remmina/actions)
[![License: GPL v2+](https://img.shields.io/badge/License-GPL%20v2+-blue.svg)](COPYING)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20FreeBSD-lightgrey.svg)](https://github.com/ultrasardine/multi-remmina)

**Multi-Remmina** is a multiplatform remote desktop client that extends the popular Remmina project to macOS and FreeBSD while maintaining full Linux compatibility. Connect to remote desktops using RDP, VNC, SPICE, X2Go, SSH, and more—all from a unified GTK-based interface.

---

## Table of Contents

- [Features](#features)
- [Supported Protocols](#supported-protocols)
- [Supported Platforms](#supported-platforms)
- [Screenshots](#screenshots)
- [Installation](#installation)
  - [Linux](#linux)
  - [macOS](#macos)
  - [FreeBSD](#freebsd)
- [Building from Source](#building-from-source)
  - [Linux Build](#linux-build)
  - [macOS Build](#macos-build)
  - [FreeBSD Build](#freebsd-build)
- [Architecture](#architecture)
  - [Core Components](#core-components)
  - [Plugin System](#plugin-system)
  - [Platform Abstraction](#platform-abstraction)
- [Usage](#usage)
  - [Quick Start](#quick-start)
  - [Creating Connections](#creating-connections)
  - [SSH Tunneling](#ssh-tunneling)
  - [Kiosk Mode](#kiosk-mode)
  - [Command Line Options](#command-line-options)
- [Configuration](#configuration)
  - [Configuration Files](#configuration-files)
  - [Connection Profiles](#connection-profiles)
  - [Preferences](#preferences)
- [Development](#development)
  - [Project Structure](#project-structure)
  - [Building Plugins](#building-plugins)
  - [Coding Standards](#coding-standards)
- [Contributing](#contributing)
- [Compatibility](#compatibility)
- [License](#license)
- [Acknowledgments](#acknowledgments)

---

## Features

- **Multi-Protocol Support**: RDP, VNC, SPICE, X2Go, SSH, SFTP, and HTTP protocols
- **Cross-Platform**: Native support for Linux, macOS, and FreeBSD
- **Plugin Architecture**: Extensible design with dynamically loaded protocol plugins
- **SSH Tunneling**: Secure connections through SSH tunnels
- **Multi-Monitor Support**: Span remote desktops across multiple displays
- **Kiosk Mode**: Dedicated thin-client/kiosk deployment mode
- **Credential Management**: Secure password storage using platform-native keychains
  - Linux: GNOME Keyring, KWallet
  - macOS: Keychain Services
  - FreeBSD: Secret Service API
- **SFTP/FTP Client**: Built-in file transfer capabilities
- **Terminal Emulator**: Integrated SSH terminal with VTE
- **Quick Connect**: Rapid connection via protocol handlers (rdp://, vnc://, ssh://)
- **Connection Profiles**: Save and organize connection settings
- **Clipboard Sharing**: Seamless clipboard integration with remote systems
- **Color Schemes**: 300+ terminal color themes included

---

## Supported Protocols

| Protocol | Description | Plugin |
|----------|-------------|--------|
| **RDP** | Remote Desktop Protocol (Windows) | FreeRDP-based |
| **VNC** | Virtual Network Computing | libvncclient |
| **SPICE** | Simple Protocol for Independent Computing Environments | libspice |
| **X2Go** | Remote desktop for X11 applications | X2Go client |
| **SSH** | Secure Shell terminal | libssh |
| **SFTP** | SSH File Transfer Protocol | libssh |
| **WWW** | HTTP/HTTPS web browser | WebKit |
| **EXEC** | Execute local commands | Built-in |

---

## Supported Platforms

### Linux
- **Distributions**: Ubuntu, Debian, Fedora, Arch, openSUSE, and more
- **Packaging**: Native packages, Snap, Flatpak
- **Display Servers**: X11, Wayland
- **Desktop Environments**: GNOME, KDE, XFCE, MATE, Cinnamon

### macOS
- **Versions**: macOS 10.15 (Catalina) and later
- **Architectures**: Intel (x86_64) and Apple Silicon (arm64)
- **Distribution**: Application bundle (.app)
- **Integration**: Native Keychain, Bonjour service discovery

### FreeBSD
- **Versions**: FreeBSD 12.x and later
- **Packaging**: Ports, pkg
- **Desktop**: X11-based environments

---

## Screenshots

*Coming soon - screenshots of Multi-Remmina on different platforms*

---

## Installation

### Linux

#### Ubuntu/Debian
```bash
# Add PPA (if available)
sudo add-apt-repository ppa:multi-remmina/ppa
sudo apt update
sudo apt install multi-remmina
```

#### Fedora
```bash
sudo dnf install multi-remmina
```

#### Arch Linux
```bash
yay -S multi-remmina
```

#### Snap (Universal)
```bash
sudo snap install multi-remmina
```

#### Flatpak (Universal)
```bash
flatpak install flathub org.multi-remmina.MultiRemmina
```

### macOS

#### Homebrew (Recommended)
```bash
brew tap ultrasardine/multi-remmina
brew install --cask multi-remmina
```

#### Manual Installation
1. Download the latest `.dmg` from [Releases](https://github.com/ultrasardine/multi-remmina/releases)
2. Open the `.dmg` file
3. Drag `Multi-Remmina.app` to your Applications folder
4. Launch from Applications or Spotlight

### FreeBSD

#### Using pkg
```bash
sudo pkg install multi-remmina
```

#### Using Ports
```bash
cd /usr/ports/net/multi-remmina
sudo make install clean
```

---

## Building from Source

### Prerequisites

#### All Platforms
- CMake 3.10.0 or later
- C compiler (GCC or Clang)
- GTK3 3.14.0 or later
- GLib 2.0
- libsodium
- OpenSSL or libgcrypt

#### Protocol-Specific Dependencies
- **RDP**: FreeRDP 2.0+
- **VNC**: libvncclient
- **SPICE**: spice-gtk, spice-protocol
- **SSH/SFTP**: libssh 0.8.0+
- **WWW**: webkit2gtk

### Linux Build

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt install build-essential cmake git \
    libgtk-3-dev libglib2.0-dev libsodium-dev \
    libssl-dev libssh-dev freerdp2-dev \
    libvncserver-dev libspice-client-gtk-3.0-dev

# Clone repository
git clone https://github.com/ultrasardine/multi-remmina.git
cd multi-remmina

# Build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

# Install
sudo make install
```

### macOS Build

```bash
# Install Homebrew (if not already installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake pkg-config gtk+3 glib libsodium openssl@3 \
             libssh freerdp libvncserver gettext json-glib

# Clone repository
git clone https://github.com/ultrasardine/multi-remmina.git
cd multi-remmina

# Build using automated script
./scripts/macos-build.sh

# Create application bundle
./scripts/create-macos-bundle.sh

# Launch
open Multi-Remmina.app
```

#### macOS Build Options

```bash
# Debug build
./scripts/macos-build.sh -t Debug

# Custom build directory
./scripts/macos-build.sh -d build-custom

# Skip dependency installation
./scripts/macos-build.sh --skip-deps

# Show all options
./scripts/macos-build.sh --help
```

### FreeBSD Build

```bash
# Install dependencies
sudo pkg install cmake pkgconf gtk3 glib libsodium \
                 libssh freerdp libvncserver

# Clone repository
git clone https://github.com/ultrasardine/multi-remmina.git
cd multi-remmina

# Build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(sysctl -n hw.ncpu)

# Install
sudo make install
```

### Build Options

Common CMake options:

```bash
cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DWITH_TRANSLATIONS=ON \
    -DWITH_AVAHI=ON \           # Linux only
    -DWITH_MACOS_KEYCHAIN=ON \  # macOS only
    -DWITH_NEWS=OFF \
    -DWITH_STATS=OFF \
    ..
```

| Option | Default | Description |
|--------|---------|-------------|
| `CMAKE_BUILD_TYPE` | Release | Build type (Release/Debug) |
| `WITH_TRANSLATIONS` | ON | Enable translations |
| `WITH_AVAHI` | ON (Linux) | Enable Avahi/mDNS discovery |
| `WITH_MACOS_KEYCHAIN` | ON (macOS) | Enable macOS Keychain |
| `WITH_NEWS` | ON | Enable online news check |
| `WITH_STATS` | ON | Enable usage statistics |
| `WITH_MANPAGES` | ON | Build man pages |

---

## Architecture

Multi-Remmina follows a modular architecture with clear separation between core functionality and protocol implementations.

### Core Components

```
multi-remmina/
├── src/                          # Core application
│   ├── remmina.c                 # Main entry point
│   ├── remmina_main.c            # Main window
│   ├── remmina_connection_window.c  # Connection handling
│   ├── remmina_file.c            # Profile management
│   ├── remmina_plugin_manager.c  # Plugin system
│   ├── remmina_ssh.c             # SSH/tunnel support
│   ├── remmina_pref.c            # Preferences
│   └── remmina_*_macos.c         # Platform-specific code
├── plugins/                      # Protocol plugins
│   ├── rdp/                      # RDP plugin
│   ├── vnc/                      # VNC plugin
│   ├── ssh/                      # SSH plugin
│   └── secret/                   # Credential storage
└── data/                         # Resources
    ├── ui/                       # GTK UI definitions
    ├── theme/                    # Color schemes
    └── desktop/                  # Desktop integration
```

### Plugin System

Multi-Remmina uses a dynamic plugin architecture:

1. **Plugin Discovery**: Plugins are loaded from `${libdir}/multi-remmina/plugins/`
2. **Plugin Interface**: Each plugin implements the `RemminaPlugin` interface
3. **Protocol Registration**: Plugins register protocol handlers with the core
4. **Dynamic Loading**: Plugins are loaded on-demand using `dlopen()`

#### Plugin Types

- **Protocol Plugins**: Implement remote desktop protocols (RDP, VNC, etc.)
- **Entry Plugins**: Provide UI widgets for connection settings
- **File Plugins**: Handle file transfer protocols (SFTP)
- **Tool Plugins**: Add utility functions to the toolbar
- **Secret Plugins**: Manage credential storage backends
- **Preference Plugins**: Add custom preference pages

#### Creating a Plugin

```c
// Example plugin structure
static RemminaProtocolPlugin remmina_plugin = {
    REMMINA_PLUGIN_TYPE_PROTOCOL,
    "MYPROTOCOL",
    "My Protocol",
    "My Protocol Description",
    PLUGIN_VERSION,
    "myprotocol",
    "myprotocol",
    NULL,
    NULL,
    remmina_plugin_myprotocol_init,
    remmina_plugin_myprotocol_open_connection,
    remmina_plugin_myprotocol_close_connection,
    // ... more callbacks
};
```

### Platform Abstraction

Multi-Remmina abstracts platform-specific functionality:

#### Credential Storage
- **Linux**: GNOME Keyring (`libsecret`) or KWallet
- **macOS**: Keychain Services API
- **FreeBSD**: Secret Service API

#### Service Discovery
- **Linux**: Avahi (mDNS/DNS-SD)
- **macOS**: Bonjour (native mDNS)
- **FreeBSD**: Avahi

#### System Tray
- **Linux**: AppIndicator or StatusIcon
- **macOS**: NSStatusBar
- **FreeBSD**: StatusIcon

---

## Usage

### Quick Start

1. **Launch Multi-Remmina**
   ```bash
   multi-remmina
   ```

2. **Create a new connection**
   - Click the "+" button or press `Ctrl+N`
   - Select protocol (RDP, VNC, SSH, etc.)
   - Enter server address and credentials
   - Click "Save and Connect"

3. **Quick Connect**
   - Use the quick connect bar at the top
   - Enter: `protocol://server:port`
   - Examples:
     - `rdp://192.168.1.100`
     - `vnc://server.example.com:5900`
     - `ssh://user@host.com`

### Creating Connections

#### RDP Connection
```
Protocol: RDP
Server: windows-server.example.com
Username: administrator
Password: (stored in keychain)
Resolution: Use client resolution
Color depth: True color (32 bpp)
```

#### VNC Connection
```
Protocol: VNC
Server: 192.168.1.50:5900
Password: (stored in keychain)
Color depth: True color (24 bpp)
Quality: Good
```

#### SSH Connection
```
Protocol: SSH
Server: linux-server.example.com
Username: user
SSH Authentication: Public key
SSH Private key: ~/.ssh/id_rsa
```

### SSH Tunneling

Multi-Remmina supports SSH tunneling for secure connections:

1. **Enable SSH Tunnel** in connection settings
2. **Configure tunnel**:
   - SSH Server: jump-host.example.com
   - SSH Username: tunnel-user
   - SSH Authentication: Public key or password
3. **Target Server**: Internal server address (e.g., 10.0.0.50)

The connection flow: `Client → SSH Tunnel → Target Server`

### Kiosk Mode

Launch Multi-Remmina in kiosk mode for dedicated thin-client deployments:

```bash
multi-remmina --kiosk
```

Features:
- Minimal UI (no menu bar, no toolbar)
- Auto-connect to specified profile
- Prevent user from closing connection
- Ideal for public terminals or dedicated workstations

### Command Line Options

```bash
multi-remmina [OPTIONS] [FILE]

Options:
  -a, --about              Show about dialog
  -c, --connect=FILE       Connect to profile
  -e, --edit=FILE          Edit profile
  -h, --help               Show help options
  -k, --kiosk              Start in kiosk mode
  -n, --new                Create new connection
  -p, --pref               Show preferences
  -q, --quit               Quit application
  -v, --version            Show version
  -x, --execute=PROFILE    Execute profile
  --display=DISPLAY        X display to use
```

Examples:
```bash
# Connect to a saved profile
multi-remmina -c ~/.local/share/multi-remmina/myserver.remmina

# Edit a profile
multi-remmina -e ~/.local/share/multi-remmina/myserver.remmina

# Quick connect via protocol handler
multi-remmina rdp://192.168.1.100

# Start in kiosk mode with auto-connect
multi-remmina --kiosk -c kiosk-profile.remmina
```

---

## Configuration

### Configuration Files

Multi-Remmina stores configuration in platform-specific locations:

#### Linux/FreeBSD
- **Config**: `~/.config/multi-remmina/remmina.pref`
- **Profiles**: `~/.local/share/multi-remmina/*.remmina`
- **Logs**: `~/.cache/multi-remmina/remmina.log`

#### macOS
- **Config**: `~/Library/Application Support/multi-remmina/remmina.pref`
- **Profiles**: `~/Library/Application Support/multi-remmina/profiles/*.remmina`
- **Logs**: `~/Library/Logs/multi-remmina/remmina.log`

### Connection Profiles

Connection profiles are stored as `.remmina` files in INI format:

```ini
[remmina]
name=My Server
protocol=RDP
server=192.168.1.100
username=administrator
password=.
resolution_mode=0
colordepth=32
sound=off
shareprinter=0
disableclipboard=0
ssh_enabled=0
```

**Note**: Passwords are stored securely in the platform keychain, not in the profile file.

### Preferences

Access preferences via `Edit → Preferences` or `Ctrl+P`:

#### General
- Minimize to system tray
- Start minimized
- Confirm before closing connections
- Remember last view mode

#### Appearance
- View mode (icon, list, tree)
- Show toolbar
- Show quick connect bar
- Hide toolbar when maximized

#### Keyboard
- Keyboard grab mode
- Keyboard shortcuts
- Host key combination

#### Security
- Encryption plugin
- Audit mode
- Trust all certificates (not recommended)
- Screenshot security

---

## Development

### Project Structure

```
multi-remmina/
├── cmake/                  # CMake modules
│   ├── Find*.cmake         # Dependency finders
│   └── ConfigOptions.cmake # Build options
├── data/                   # Application data
│   ├── desktop/            # Desktop files, icons
│   ├── ui/                 # GTK UI definitions (.glade)
│   └── theme/              # Terminal color schemes
├── plugins/                # Protocol plugins
│   ├── common/             # Shared plugin utilities
│   ├── rdp/                # RDP plugin (FreeRDP)
│   ├── vnc/                # VNC plugin
│   ├── ssh/                # SSH plugin
│   ├── secret/             # Credential storage
│   └── */                  # Other plugins
├── po/                     # Translations (gettext)
├── scripts/                # Build scripts
├── src/                    # Core application
│   ├── include/            # Public headers
│   └── *.c                 # Source files
└── tests/                  # Unit tests
```

### Building Plugins

To build a custom plugin:

1. **Create plugin directory**: `plugins/myplugin/`
2. **Implement plugin interface**: See `plugins/tool_hello_world/` for example
3. **Add CMakeLists.txt**:
   ```cmake
   set(REMMINA_PLUGIN_MYPLUGIN_SRCS
       myplugin_plugin.c
   )
   add_library(remmina-plugin-myplugin MODULE ${REMMINA_PLUGIN_MYPLUGIN_SRCS})
   target_link_libraries(remmina-plugin-myplugin ${REMMINA_COMMON_LIBRARIES})
   install(TARGETS remmina-plugin-myplugin DESTINATION ${REMMINA_PLUGINDIR})
   ```
4. **Build**: `make remmina-plugin-myplugin`

### Coding Standards

- **Language**: C (C11 standard)
- **Style**: K&R style with tabs for indentation
- **Naming**:
  - Files: `remmina_component.c`
  - Functions: `remmina_component_action()`
  - Structs: `RemminaComponent`
  - Macros: `REMMINA_CONSTANT`
- **Types**: Use GLib types (`gchar*`, `gint`, `gboolean`)
- **Memory**: Use GLib memory functions (`g_malloc`, `g_free`)
- **Headers**: Include order: system → config.h → local headers

Example:
```c
#include <gtk/gtk.h>
#include "config.h"
#include "remmina_component.h"

gboolean
remmina_component_action(RemminaComponent *component, const gchar *param)
{
    g_return_val_if_fail(component != NULL, FALSE);
    
    // Implementation
    
    return TRUE;
}
```

---

## Contributing

We welcome contributions! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

### Quick Contribution Guide

1. **Fork** the repository
2. **Create** a feature branch: `git checkout -b feature/my-feature`
3. **Commit** your changes: `git commit -am 'Add my feature'`
4. **Push** to the branch: `git push origin feature/my-feature`
5. **Submit** a pull request

### Areas for Contribution

- Bug fixes and testing
- Platform-specific improvements (especially macOS and FreeBSD)
- New protocol plugins
- Documentation improvements
- Translations
- UI/UX enhancements

### Reporting Issues

- Use [GitHub Issues](https://github.com/ultrasardine/multi-remmina/issues)
- Include:
  - Platform and version
  - Steps to reproduce
  - Expected vs actual behavior
  - Relevant logs

---

## Compatibility

### Upstream Compatibility

Multi-Remmina maintains compatibility with the original Remmina project:

- **Protocol Compatibility**: All protocols work identically
- **Profile Format**: Can read and write Remmina `.remmina` files
- **Plugin API**: Compatible plugin interface
- **Configuration**: Can import existing Remmina configurations

### Migration from Remmina

To migrate from Remmina to Multi-Remmina:

1. **Profiles**: Copy `~/.local/share/remmina/` to `~/.local/share/multi-remmina/`
2. **Config**: Copy `~/.config/remmina/` to `~/.config/multi-remmina/`
3. **Credentials**: Re-enter passwords (keychain entries are separate)

Multi-Remmina will automatically detect and use existing Remmina configurations as fallback.

### Upstream Project

Multi-Remmina is based on [Remmina](https://gitlab.com/Remmina/Remmina) by the Remmina team. We maintain an upstream remote to sync improvements and security fixes.

---

## License

Multi-Remmina is licensed under the **GNU General Public License v2.0 or later** with the **OpenSSL linking exception**.

See [COPYING](COPYING) and [LICENSE.OpenSSL](LICENSE.OpenSSL) for full license text.

### Third-Party Licenses

Multi-Remmina uses several open-source libraries:
- **GTK3**: LGPL 2.1+
- **FreeRDP**: Apache License 2.0
- **libvncclient**: GPL 2.0+
- **libssh**: LGPL 2.1+
- **libsodium**: ISC License

---

## Acknowledgments

### Multi-Remmina Team

Multi-Remmina is a community-driven multiplatform port.

### Original Remmina Team

- **Antenore Gatta** - Core maintainer
- **Giovanni Panozzo** - Core maintainer
- **Hiroyuki Tanaka** - Maintainer
- **Allan Nordhøy** - Documentation and translations

### Original Authors

- **Vic Lee** - Original Remmina creator
- **Marc-André Moreau** - FreeRDP integration

### Contributors

See [AUTHORS](AUTHORS) and [THANKS.md](THANKS.md) for the full list of contributors.

### Projects

- [Remmina](https://gitlab.com/Remmina/Remmina) - Original project
- [FreeRDP](https://github.com/FreeRDP/FreeRDP) - RDP library
- [GTK](https://www.gtk.org/) - UI toolkit
- [libssh](https://www.libssh.org/) - SSH library

---

## Links

- **GitHub**: [https://github.com/ultrasardine/multi-remmina](https://github.com/ultrasardine/multi-remmina)
- **Issues**: [https://github.com/ultrasardine/multi-remmina/issues](https://github.com/ultrasardine/multi-remmina/issues)
- **Discussions**: [https://github.com/ultrasardine/multi-remmina/discussions](https://github.com/ultrasardine/multi-remmina/discussions)
- **Original Remmina**: [https://www.remmina.org/](https://www.remmina.org/)
- **Original GitLab**: [https://gitlab.com/Remmina/Remmina](https://gitlab.com/Remmina/Remmina)

---

**Multi-Remmina** - Remote desktop access, anywhere, on any platform.
