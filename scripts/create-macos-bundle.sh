#!/bin/bash
# macOS Application Bundle Creation Script for Remmina
# This script creates a standard macOS .app bundle from the build output

set -e  # Exit on error

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Print colored messages
print_error() {
    echo -e "${RED}ERROR: $1${NC}" >&2
}

print_success() {
    echo -e "${GREEN}SUCCESS: $1${NC}"
}

print_info() {
    echo -e "${YELLOW}INFO: $1${NC}"
}

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Default values
BUILD_DIR="$PROJECT_ROOT/build"
BUNDLE_NAME="Remmina.app"
OUTPUT_DIR="$PROJECT_ROOT"
VERSION="1.4.41"
BUNDLE_ID="org.remmina.Remmina"

# Display usage information
usage() {
    cat << EOF
Usage: $0 [OPTIONS]

Create a macOS application bundle for Remmina.

OPTIONS:
    -h, --help              Show this help message
    -b, --build-dir DIR     Specify build directory (default: build)
    -o, --output-dir DIR    Specify output directory for bundle (default: project root)
    -n, --name NAME         Specify bundle name (default: Remmina.app)
    -v, --version VERSION   Specify version string (default: 1.4.41)

EXAMPLES:
    $0                      # Create bundle with default settings
    $0 -b build-release -o ~/Desktop  # Custom build dir and output location

EOF
}

# Parse command line arguments
parse_arguments() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                usage
                exit 0
                ;;
            -b|--build-dir)
                BUILD_DIR="$2"
                shift 2
                ;;
            -o|--output-dir)
                OUTPUT_DIR="$2"
                shift 2
                ;;
            -n|--name)
                BUNDLE_NAME="$2"
                shift 2
                ;;
            -v|--version)
                VERSION="$2"
                shift 2
                ;;
            *)
                print_error "Unknown option: $1"
                usage
                exit 1
                ;;
        esac
    done
}

# Verify build directory exists and contains executable
verify_build() {
    print_info "Verifying build directory: $BUILD_DIR"
    
    if [ ! -d "$BUILD_DIR" ]; then
        print_error "Build directory does not exist: $BUILD_DIR"
        echo "Please run the build script first: scripts/macos-build.sh"
        exit 1
    fi
    
    if [ ! -f "$BUILD_DIR/src/remmina" ]; then
        print_error "Remmina executable not found in build directory"
        echo "Expected: $BUILD_DIR/src/remmina"
        exit 1
    fi
    
    print_success "Build directory verified"
}

# Create bundle directory structure
create_bundle_structure() {
    local bundle_path="$OUTPUT_DIR/$BUNDLE_NAME"
    
    print_info "Creating bundle structure: $bundle_path"
    
    # Remove existing bundle if it exists
    if [ -d "$bundle_path" ]; then
        print_info "Removing existing bundle..."
        rm -rf "$bundle_path"
    fi
    
    # Create directory structure
    mkdir -p "$bundle_path/Contents/MacOS"
    mkdir -p "$bundle_path/Contents/Resources"
    mkdir -p "$bundle_path/Contents/Resources/lib/remmina/plugins"
    mkdir -p "$bundle_path/Contents/Resources/ui"
    mkdir -p "$bundle_path/Contents/Resources/theme"
    mkdir -p "$bundle_path/Contents/Frameworks"
    
    print_success "Bundle structure created"
}

# Copy executable to bundle
copy_executable() {
    local bundle_path="$OUTPUT_DIR/$BUNDLE_NAME"
    
    print_info "Copying executable to bundle..."
    
    cp "$BUILD_DIR/src/remmina" "$bundle_path/Contents/MacOS/"
    chmod +x "$bundle_path/Contents/MacOS/remmina"
    
    print_success "Executable copied"
}

# Copy plugins to bundle
copy_plugins() {
    local bundle_path="$OUTPUT_DIR/$BUNDLE_NAME"
    local plugin_dir="$BUILD_DIR/plugins"
    
    print_info "Copying plugins to bundle..."
    
    if [ ! -d "$plugin_dir" ]; then
        print_error "Plugin directory not found: $plugin_dir"
        exit 1
    fi
    
    # Find all plugin .so files
    local plugin_count=0
    while IFS= read -r -d '' plugin; do
        local plugin_name=$(basename "$plugin")
        cp "$plugin" "$bundle_path/Contents/Resources/lib/remmina/plugins/"
        print_info "  Copied plugin: $plugin_name"
        ((plugin_count++))
    done < <(find "$plugin_dir" -name "*.so" -type f -print0)
    
    if [ $plugin_count -eq 0 ]; then
        print_error "No plugins found in $plugin_dir"
        exit 1
    fi
    
    print_success "Copied $plugin_count plugin(s)"
}

# Copy UI files to bundle
copy_ui_files() {
    local bundle_path="$OUTPUT_DIR/$BUNDLE_NAME"
    local ui_source="$PROJECT_ROOT/data/ui"
    
    print_info "Copying UI files to bundle..."
    
    if [ ! -d "$ui_source" ]; then
        print_error "UI directory not found: $ui_source"
        exit 1
    fi
    
    # Copy all UI files
    cp -r "$ui_source"/* "$bundle_path/Contents/Resources/ui/" 2>/dev/null || true
    
    # Count copied files
    local ui_count=$(find "$bundle_path/Contents/Resources/ui" -type f | wc -l | tr -d ' ')
    
    if [ $ui_count -eq 0 ]; then
        print_error "No UI files copied"
        exit 1
    fi
    
    print_success "Copied $ui_count UI file(s)"
}

# Copy theme files to bundle
copy_theme_files() {
    local bundle_path="$OUTPUT_DIR/$BUNDLE_NAME"
    local theme_source="$PROJECT_ROOT/data/theme"
    
    print_info "Copying theme files to bundle..."
    
    if [ ! -d "$theme_source" ]; then
        print_error "Theme directory not found: $theme_source"
        exit 1
    fi
    
    # Copy all theme files
    cp -r "$theme_source"/* "$bundle_path/Contents/Resources/theme/" 2>/dev/null || true
    
    # Count copied files
    local theme_count=$(find "$bundle_path/Contents/Resources/theme" -type f -name "*.colors" | wc -l | tr -d ' ')
    
    if [ $theme_count -eq 0 ]; then
        print_error "No theme files copied"
        exit 1
    fi
    
    print_success "Copied $theme_count theme file(s)"
}

# Generate Info.plist
generate_info_plist() {
    local bundle_path="$OUTPUT_DIR/$BUNDLE_NAME"
    local plist_path="$bundle_path/Contents/Info.plist"
    
    print_info "Generating Info.plist..."
    
    cat > "$plist_path" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>CFBundleExecutable</key>
	<string>remmina</string>
	<key>CFBundleIdentifier</key>
	<string>$BUNDLE_ID</string>
	<key>CFBundleName</key>
	<string>Remmina</string>
	<key>CFBundleDisplayName</key>
	<string>Remmina</string>
	<key>CFBundleVersion</key>
	<string>$VERSION</string>
	<key>CFBundleShortVersionString</key>
	<string>$VERSION</string>
	<key>CFBundlePackageType</key>
	<string>APPL</string>
	<key>CFBundleSignature</key>
	<string>????</string>
	<key>CFBundleIconFile</key>
	<string>Remmina</string>
	<key>NSHighResolutionCapable</key>
	<true/>
	<key>LSMinimumSystemVersion</key>
	<string>10.15</string>
	<key>NSHumanReadableCopyright</key>
	<string>Copyright © 2009-2024 Remmina Developers. Licensed under GPLv2+.</string>
	<key>LSApplicationCategoryType</key>
	<string>public.app-category.utilities</string>
</dict>
</plist>
EOF
    
    print_success "Info.plist generated"
}

# Convert PNG icon to ICNS format
convert_icon() {
    local bundle_path="$OUTPUT_DIR/$BUNDLE_NAME"
    local icon_source="$PROJECT_ROOT/data/desktop/512x512/apps/org.remmina.Remmina.png"
    
    print_info "Converting icon to ICNS format..."
    
    if [ ! -f "$icon_source" ]; then
        print_error "Icon source not found: $icon_source"
        exit 1
    fi
    
    # Create temporary iconset directory
    local iconset_dir=$(mktemp -d)
    local iconset_path="$iconset_dir/Remmina.iconset"
    mkdir -p "$iconset_path"
    
    # Copy and resize icon to various sizes required for ICNS
    # macOS requires specific sizes: 16, 32, 64, 128, 256, 512, 1024
    # and @2x versions for Retina displays
    
    # Check if sips is available (macOS image processing tool)
    if ! command -v sips &> /dev/null; then
        print_error "sips command not found (required for icon conversion)"
        rm -rf "$iconset_dir"
        exit 1
    fi
    
    # Generate all required icon sizes
    sips -z 16 16     "$icon_source" --out "$iconset_path/icon_16x16.png" &>/dev/null
    sips -z 32 32     "$icon_source" --out "$iconset_path/icon_16x16@2x.png" &>/dev/null
    sips -z 32 32     "$icon_source" --out "$iconset_path/icon_32x32.png" &>/dev/null
    sips -z 64 64     "$icon_source" --out "$iconset_path/icon_32x32@2x.png" &>/dev/null
    sips -z 128 128   "$icon_source" --out "$iconset_path/icon_128x128.png" &>/dev/null
    sips -z 256 256   "$icon_source" --out "$iconset_path/icon_128x128@2x.png" &>/dev/null
    sips -z 256 256   "$icon_source" --out "$iconset_path/icon_256x256.png" &>/dev/null
    sips -z 512 512   "$icon_source" --out "$iconset_path/icon_256x256@2x.png" &>/dev/null
    sips -z 512 512   "$icon_source" --out "$iconset_path/icon_512x512.png" &>/dev/null
    cp "$icon_source" "$iconset_path/icon_512x512@2x.png"
    
    # Convert iconset to ICNS
    if ! iconutil -c icns "$iconset_path" -o "$bundle_path/Contents/Resources/Remmina.icns"; then
        print_error "Failed to convert icon to ICNS format"
        rm -rf "$iconset_dir"
        exit 1
    fi
    
    # Clean up temporary directory
    rm -rf "$iconset_dir"
    
    print_success "Icon converted to ICNS format"
}

# Verify bundle structure
verify_bundle() {
    local bundle_path="$OUTPUT_DIR/$BUNDLE_NAME"
    
    print_info "Verifying bundle structure..."
    
    local errors=0
    
    # Check required directories
    if [ ! -d "$bundle_path/Contents/MacOS" ]; then
        print_error "Missing Contents/MacOS directory"
        ((errors++))
    fi
    
    if [ ! -d "$bundle_path/Contents/Resources" ]; then
        print_error "Missing Contents/Resources directory"
        ((errors++))
    fi
    
    # Check executable
    if [ ! -f "$bundle_path/Contents/MacOS/remmina" ]; then
        print_error "Missing executable"
        ((errors++))
    elif [ ! -x "$bundle_path/Contents/MacOS/remmina" ]; then
        print_error "Executable is not executable"
        ((errors++))
    fi
    
    # Check Info.plist
    if [ ! -f "$bundle_path/Contents/Info.plist" ]; then
        print_error "Missing Info.plist"
        ((errors++))
    fi
    
    # Check icon
    if [ ! -f "$bundle_path/Contents/Resources/Remmina.icns" ]; then
        print_error "Missing icon file"
        ((errors++))
    fi
    
    # Check plugins
    local plugin_count=$(find "$bundle_path/Contents/Resources/lib/remmina/plugins" -name "*.so" | wc -l | tr -d ' ')
    if [ $plugin_count -eq 0 ]; then
        print_error "No plugins found in bundle"
        ((errors++))
    fi
    
    # Check UI files
    local ui_count=$(find "$bundle_path/Contents/Resources/ui" -type f | wc -l | tr -d ' ')
    if [ $ui_count -eq 0 ]; then
        print_error "No UI files found in bundle"
        ((errors++))
    fi
    
    # Check theme files
    local theme_count=$(find "$bundle_path/Contents/Resources/theme" -type f | wc -l | tr -d ' ')
    if [ $theme_count -eq 0 ]; then
        print_error "No theme files found in bundle"
        ((errors++))
    fi
    
    if [ $errors -gt 0 ]; then
        print_error "Bundle verification failed with $errors error(s)"
        exit 1
    fi
    
    print_success "Bundle verification passed"
}

# Main function
main() {
    parse_arguments "$@"
    
    print_info "Starting macOS bundle creation for Remmina..."
    print_info "Build directory: $BUILD_DIR"
    print_info "Output directory: $OUTPUT_DIR"
    print_info "Bundle name: $BUNDLE_NAME"
    print_info "Version: $VERSION"
    
    verify_build
    create_bundle_structure
    copy_executable
    copy_plugins
    copy_ui_files
    copy_theme_files
    generate_info_plist
    convert_icon
    verify_bundle
    
    print_success "Bundle creation completed successfully!"
    print_info "Bundle location: $OUTPUT_DIR/$BUNDLE_NAME"
    print_info ""
    print_info "You can now launch Remmina by double-clicking the bundle or running:"
    print_info "  open $OUTPUT_DIR/$BUNDLE_NAME"
}

# Run main function
main "$@"
