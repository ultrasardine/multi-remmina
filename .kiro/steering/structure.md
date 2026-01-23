# Project Structure

## Top-Level Organization

```
remmina/
├── src/                    # Main application source code
├── plugins/                # Protocol plugins (RDP, VNC, SSH, etc.)
├── data/                   # Application data (UI, icons, themes, desktop files)
├── cmake/                  # CMake modules and build configuration
├── po/                     # Translation files (gettext)
├── ci/                     # CI/CD configuration and Docker files
├── flatpak/                # Flatpak packaging configuration
├── snap/                   # Snap packaging configuration
├── scripts/                # Build and maintenance scripts
└── .kiro/                  # Kiro AI assistant configuration
```

## Source Code (`src/`)
Main application code organized by functionality:
- `remmina.c` - Main entry point
- `remmina_main.c` - Main window implementation
- `remmina_file*.c` - Connection file management
- `remmina_connection_window.c` - Connection window (needs refactoring)
- `remmina_plugin_*.c` - Plugin management
- `remmina_ssh*.c` - SSH/SFTP functionality
- `remmina_pref*.c` - Preferences/settings
- `remmina_*_window.c` - Various UI windows
- `include/` - Public header files

## Plugins (`plugins/`)
Each protocol is implemented as a plugin:
- `rdp/` - RDP protocol (FreeRDP-based)
- `vnc/` - VNC protocol
- `spice/` - SPICE protocol
- `ssh/` - SSH terminal
- `x2go/` - X2Go protocol
- `www/` - HTTP/WWW protocol
- `exec/` - Execute commands
- `secret/` - Secret storage (GNOME Keyring, KWallet)
- `kwallet/` - KDE Wallet integration
- `python_wrapper/` - Python plugin support
- `tool_hello_world/` - Example plugin
- `common/` - Shared plugin utilities

## Data (`data/`)
- `desktop/` - Desktop files, MIME types, icons (various sizes)
- `ui/` - Glade UI definition files
- `theme/` - Terminal color schemes (.colors files)
- `icons/` - Application icons (SVG)
- `reports/` - HTML report templates

## CMake (`cmake/`)
- `Find*.cmake` - CMake find modules for dependencies
- `ConfigOptions.cmake` - Build configuration options
- `macros/` - CMake utility macros
- `cmake_uninstall.cmake.in` - Uninstall target template

## Translations (`po/`)
- `*.po` - Translation files for various languages
- `POTFILES.in` - List of translatable source files
- `LINGUAS` - List of supported languages

## Code Conventions

### File Headers
All source files include:
- GPL v2+ license header
- Copyright notices
- OpenSSL linking exception statement

### Include Order
1. System headers (`<gtk/gtk.h>`, `<glib.h>`)
2. Config header (`"config.h"`)
3. Local headers (`"remmina_*.h"`)
4. Trace calls header (`"remmina/remmina_trace_calls.h"`)

### Naming Conventions
- **Files**: `remmina_<component>.c/h` (lowercase, underscores)
- **Functions**: `remmina_<component>_<action>()` (lowercase, underscores)
- **Structs**: `Remmina<Component>` (PascalCase)
- **Private structs**: `_Remmina<Component>Priv`
- **Macros**: `REMMINA_<NAME>` (uppercase, underscores)

### Code Style
- Use tabs for indentation (not spaces)
- K&R-style bracing
- GTK/GLib coding conventions
- Use GLib types (`gchar*`, `gint`, `gboolean`, etc.)
- Use GLib memory management (`g_malloc`, `g_free`)

## Plugin Architecture
Plugins are dynamically loaded shared libraries:
- Located in `${libdir}/remmina/plugins/`
- Export plugin initialization functions
- Register protocol handlers with main application
- Can be protocol plugins or tool plugins

## Configuration & Runtime
- User config: `$HOME/.config/remmina/`
- Connection profiles: `$HOME/.local/share/remmina/`
- System data: `/usr/share/remmina/` or `/usr/local/share/remmina/`
- Plugins: `/usr/lib/remmina/plugins/` or `/usr/local/lib/remmina/plugins/`

## Important Files
- `CMakeLists.txt` - Root build configuration
- `config.h.in` - Configuration template
- `buildflags.h.in` - Build flags template
- `remmina.doap` - Project metadata (DOAP format)
- `AUTHORS` - Contributors list
- `CONTRIBUTING.md` - Contribution guidelines
- `CHANGELOG.md` - Version history
