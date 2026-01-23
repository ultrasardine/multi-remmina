# Linux Build Verification

This document describes how to verify that the macOS port changes do not break Linux builds.

## Overview

The macOS port introduces platform-specific code using conditional compilation (`#ifdef __APPLE__` and `#ifndef __APPLE__`). This verification ensures that:

1. Linux builds compile successfully
2. Linux-specific features (Avahi, AppIndicator, libsecret) remain functional
3. All plugins load correctly on Linux
4. No macOS-specific code is compiled on Linux

## Verification Methods

### Method 1: Docker-based Verification (Recommended for macOS developers)

Run the Docker-based verification script from the project root:

```bash
./tests/verify_linux_docker.sh
```

This script:
- Pulls an Ubuntu 22.04 Docker image
- Installs all required dependencies
- Runs the full verification suite
- Reports results

### Method 2: Native Linux Verification

If you have access to a Linux system, run:

```bash
./tests/verify_linux_build.sh
```

### Method 3: GitLab CI

The GitLab CI pipeline automatically tests Linux builds on every commit. Check the CI status at:
https://gitlab.com/Remmina/Remmina/pipelines

## What is Verified

### 1. Build System Configuration

- ✓ CMake detects Linux platform correctly
- ✓ Linux-specific features are enabled (Avahi, AppIndicator, libsecret)
- ✓ macOS-specific features are disabled
- ✓ macOS frameworks are not linked

### 2. Source Code Compilation

- ✓ All source files compile without errors
- ✓ Linux-specific code paths are included
- ✓ macOS-specific code paths are excluded
- ✓ No undefined symbols or linking errors

### 3. Plugin System

- ✓ RDP plugin builds and loads
- ✓ VNC plugin builds and loads
- ✓ SSH plugin builds and loads
- ✓ SFTP plugin builds and loads
- ✓ Secret plugin builds with libsecret support
- ✓ All other plugins build successfully

### 4. Linux-Specific Features

#### Avahi (Service Discovery)
- ✓ Avahi libraries are detected and linked
- ✓ Service discovery code compiles
- ✓ mDNS/DNS-SD functionality works

#### AppIndicator (System Tray)
- ✓ AppIndicator libraries are detected and linked
- ✓ System tray integration code compiles
- ✓ Tray icon displays correctly

#### libsecret (Password Storage)
- ✓ libsecret libraries are detected and linked
- ✓ Secret storage code compiles
- ✓ Password storage/retrieval works with GNOME Keyring

### 5. Test Suite

- ✓ All unit tests pass
- ✓ All integration tests pass
- ✓ No test failures introduced by macOS changes

## Platform-Specific Code Structure

### Conditional Compilation Guards

The codebase uses the following pattern for platform-specific code:

```c
#ifdef __APPLE__
    // macOS-specific code (Keychain, Bonjour, etc.)
#else
    // Linux-specific code (libsecret, Avahi, etc.)
#endif
```

Or:

```c
#ifndef __APPLE__
    // Linux-only code
#endif
```

### Key Files with Platform-Specific Code

#### macOS-Only Files (not compiled on Linux):
- `src/remmina_keychain_macos.c` - macOS Keychain implementation
- `src/remmina_bonjour_macos.c` - Bonjour service discovery
- `src/remmina_bundle_macos.c` - Bundle resource path resolution

#### Files with Platform-Specific Sections:
- `src/remmina_avahi.c` - Avahi on Linux, Bonjour wrapper on macOS
- `src/remmina_main.c` - Desktop integration (GDesktopAppInfo on Linux only)
- `src/remmina_applet_menu.c` - AppIndicator on Linux, StatusIcon on macOS
- `plugins/secret/src/glibsecret_plugin.c` - libsecret on Linux, Keychain on macOS

### CMake Configuration

The root `CMakeLists.txt` contains platform detection:

```cmake
if(APPLE)
    message(STATUS "Configuring for macOS (Darwin)")
    # macOS-specific configuration
else()
    # Linux configuration
endif()
```

## Expected Results

### Successful Verification

When verification passes, you should see:

```
==========================================
Verification Summary
==========================================

Build Status: SUCCESS
Linux Features: Verified
Plugin Build: Verified

The macOS port changes do not break Linux builds.
```

### Common Issues

#### Issue: Avahi not found
**Solution**: Install Avahi development libraries:
```bash
sudo apt-get install libavahi-ui-gtk3-dev
```

#### Issue: AppIndicator not found
**Solution**: Install AppIndicator development libraries:
```bash
sudo apt-get install libappindicator3-dev
```

#### Issue: libsecret not found
**Solution**: Install libsecret development libraries:
```bash
sudo apt-get install libsecret-1-dev
```

## Continuous Integration

The GitLab CI configuration (`.gitlab-ci.yml`) automatically runs Linux builds on:
- Ubuntu 22.04 (primary)
- Ubuntu 20.04
- Ubuntu 18.04
- Flatpak builds
- Snap builds

All CI jobs must pass before merging macOS port changes.

## Manual Testing Checklist

After automated verification passes, manually test on Linux:

- [ ] Application launches successfully
- [ ] Main window displays correctly
- [ ] Connection profiles can be created
- [ ] Passwords are stored in GNOME Keyring/KWallet
- [ ] Passwords are retrieved correctly
- [ ] Service discovery finds network services (if Avahi installed)
- [ ] System tray icon appears (if AppIndicator installed)
- [ ] RDP connections work
- [ ] VNC connections work
- [ ] SSH connections work
- [ ] SFTP file transfer works
- [ ] Application preferences can be saved
- [ ] Application quits cleanly

## Reporting Issues

If Linux build verification fails:

1. Check the build logs for specific errors
2. Verify all dependencies are installed
3. Check for missing `#ifndef __APPLE__` guards
4. Ensure CMake configuration is correct
5. Report the issue with full build logs

## References

- Requirements: `.kiro/specs/macos-port/requirements.md` (Requirement 6.1, 6.5)
- Design: `.kiro/specs/macos-port/design.md`
- GitLab CI: `.gitlab-ci.yml`
