---
inclusion: always
---
<!------------------------------------------------------------------------------------
   Multi-Remmina Project Guidelines
   
   This is a multiplatform port of Remmina, extending support to macOS and FreeBSD
   while maintaining Linux compatibility.
   
   Learn about inclusion modes: https://kiro.dev/docs/steering/#inclusion-modes
-------------------------------------------------------------------------------------> 

## Project Identity
- Project name: Multi-Remmina
- Binary name: multi-remmina (with remmina symlink for compatibility)
- Config directory: ~/.config/multi-remmina (with ~/.config/remmina fallback)
- Application ID: org.multi-remmina.MultiRemmina

## Platform Support
- Linux (primary platform, full feature set)
- macOS (active development, see .kiro/specs/macos-port.md)
- FreeBSD (supported)

## Compatibility
- Maintains protocol compatibility with original Remmina
- Can read existing Remmina configuration files
- Plugin API remains compatible