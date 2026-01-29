#!/bin/bash
# Windows Bundle Script for Multi-Remmina (MSYS2)
# This script creates a self-contained Windows package with all dependencies bundled

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

# Check if running in MSYS2 environment
check_msys2() {
    print_info "Checking for MSYS2 environment..."
    
    if [ -z "$MSYSTEM" ]; then
        print_error "Not running in MSYS2 environment"
        echo "Please run this script from MSYS2 MinGW 64-bit shell"
        exit 1
    fi
    
    print_success "Running in MSYS2 environment: $MSYSTEM"
}

# Check if required tools are available
check_tools() {
    print_info "Checking for required tools..."
    
    local missing_tools=()
    
    # Check for ldd
    if ! command -v ldd &> /dev/null; then
        missing_tools+=("ldd")
    fi
    
    # Check for gdk-pixbuf-query-loaders
    if ! command -v gdk-pixbuf-query-loaders &> /dev/null; then
        print_info "gdk-pixbuf-query-loaders not found, will skip loaders cache update"
    fi
    
    if [ ${#missing_tools[@]} -gt 0 ]; then
        print_error "Missing required tools: ${missing_tools[*]}"
        exit 1
    fi
    
    print_success "All required tools are available"
}

# Collect all required DLLs using ldd
collect_dlls() {
    local exe_path="$1"
    local output_dir="$2"
    
    print_info "Collecting required DLLs for $exe_path..."
    
    if [ ! -f "$exe_path" ]; then
        print_error "Executable not found: $exe_path"
        exit 1
    fi
    
    # Get list of DLLs from ldd
    local dll_list=$(ldd "$exe_path" | grep -i "mingw\|msys" | awk '{print $3}')
    
    local dll_count=0
    for dll in $dll_list; do
        if [ -f "$dll" ]; then
            cp -v "$dll" "$output_dir/"
            ((dll_count++))
        fi
    done
    
    print_success "Copied $dll_count DLLs to $output_dir"
}

# Recursively collect DLLs for all DLLs and plugins
collect_all_dlls() {
    local output_dir="$1"
    
    print_info "Recursively collecting DLLs for all binaries..."
    
    # Collect DLLs for main executable
    if [ -f "$output_dir/multi-remmina.exe" ]; then
        collect_dlls "$output_dir/multi-remmina.exe" "$output_dir"
    fi
    
    # Collect DLLs for all plugin DLLs
    if [ -d "$output_dir/plugins" ]; then
        for plugin in "$output_dir/plugins"/*.dll; do
            if [ -f "$plugin" ]; then
                collect_dlls "$plugin" "$output_dir"
            fi
        done
    fi
    
    # Collect DLLs for all DLLs in output directory (recursive)
    local iteration=0
    local max_iterations=5
    while [ $iteration -lt $max_iterations ]; do
        local new_dlls=false
        for dll in "$output_dir"/*.dll; do
            if [ -f "$dll" ]; then
                local dll_deps=$(ldd "$dll" 2>/dev/null | grep -i "mingw\|msys" | awk '{print $3}' || true)
                for dep_dll in $dll_deps; do
                    if [ -f "$dep_dll" ]; then
                        local dll_name=$(basename "$dep_dll")
                        if [ ! -f "$output_dir/$dll_name" ]; then
                            cp -v "$dep_dll" "$output_dir/"
                            new_dlls=true
                        fi
                    fi
                done
            fi
        done
        
        if [ "$new_dlls" = false ]; then
            break
        fi
        
        ((iteration++))
    done
    
    print_success "Completed DLL collection (iterations: $iteration)"
}

# Copy GTK3 runtime files
copy_gtk_runtime() {
    local output_dir="$1"
    local mingw_prefix="$2"
    
    print_info "Copying GTK3 runtime files..."
    
    # Create necessary directories
    mkdir -p "$output_dir/share/icons"
    mkdir -p "$output_dir/share/glib-2.0/schemas"
    mkdir -p "$output_dir/lib/gdk-pixbuf-2.0"
    mkdir -p "$output_dir/etc/gtk-3.0"
    
    # Copy icon themes (Adwaita is required for GTK3)
    if [ -d "$mingw_prefix/share/icons/Adwaita" ]; then
        print_info "Copying Adwaita icon theme..."
        cp -r "$mingw_prefix/share/icons/Adwaita" "$output_dir/share/icons/"
        print_success "Adwaita icon theme copied"
    else
        print_info "Adwaita icon theme not found, skipping..."
    fi
    
    # Copy hicolor icon theme (fallback)
    if [ -d "$mingw_prefix/share/icons/hicolor" ]; then
        print_info "Copying hicolor icon theme..."
        cp -r "$mingw_prefix/share/icons/hicolor" "$output_dir/share/icons/"
        print_success "hicolor icon theme copied"
    fi
    
    # Copy GSettings schemas
    if [ -d "$mingw_prefix/share/glib-2.0/schemas" ]; then
        print_info "Copying GSettings schemas..."
        cp -r "$mingw_prefix/share/glib-2.0/schemas"/* "$output_dir/share/glib-2.0/schemas/" 2>/dev/null || true
        
        # Compile schemas
        if command -v glib-compile-schemas &> /dev/null; then
            print_info "Compiling GSettings schemas..."
            glib-compile-schemas "$output_dir/share/glib-2.0/schemas/"
            print_success "GSettings schemas compiled"
        fi
    fi
    
    # Copy GDK pixbuf loaders
    if [ -d "$mingw_prefix/lib/gdk-pixbuf-2.0" ]; then
        print_info "Copying GDK pixbuf loaders..."
        cp -r "$mingw_prefix/lib/gdk-pixbuf-2.0"/* "$output_dir/lib/gdk-pixbuf-2.0/"
        print_success "GDK pixbuf loaders copied"
    fi
    
    print_success "GTK3 runtime files copied"
}

# Copy application resources
copy_resources() {
    local build_dir="$1"
    local output_dir="$2"
    
    print_info "Copying application resources..."
    
    # Create resource directories
    mkdir -p "$output_dir/share/multi-remmina"
    
    # Copy UI files
    if [ -d "$build_dir/data/ui" ] || [ -d "data/ui" ]; then
        print_info "Copying UI files..."
        local ui_source=""
        if [ -d "$build_dir/data/ui" ]; then
            ui_source="$build_dir/data/ui"
        else
            ui_source="data/ui"
        fi
        cp -r "$ui_source" "$output_dir/share/multi-remmina/"
        print_success "UI files copied"
    fi
    
    # Copy theme files
    if [ -d "data/theme" ]; then
        print_info "Copying theme files..."
        cp -r "data/theme" "$output_dir/share/multi-remmina/"
        print_success "Theme files copied"
    fi
    
    # Copy icon files
    if [ -d "data/icons" ]; then
        print_info "Copying application icons..."
        cp -r "data/icons" "$output_dir/share/multi-remmina/"
        print_success "Application icons copied"
    fi
    
    # Copy desktop files (for reference)
    if [ -d "data/desktop" ]; then
        print_info "Copying desktop files..."
        mkdir -p "$output_dir/share/applications"
        cp data/desktop/*.desktop "$output_dir/share/applications/" 2>/dev/null || true
    fi
    
    print_success "Application resources copied"
}

# Copy plugins
copy_plugins() {
    local build_dir="$1"
    local output_dir="$2"
    
    print_info "Copying protocol plugins..."
    
    mkdir -p "$output_dir/plugins"
    
    # Copy all plugin DLLs from build directory
    if [ -d "$build_dir/plugins" ]; then
        local plugin_count=0
        for plugin_dir in "$build_dir/plugins"/*; do
            if [ -d "$plugin_dir" ]; then
                for dll in "$plugin_dir"/*.dll; do
                    if [ -f "$dll" ]; then
                        cp -v "$dll" "$output_dir/plugins/"
                        ((plugin_count++))
                    fi
                done
            fi
        done
        print_success "Copied $plugin_count plugin(s)"
    else
        print_info "No plugins directory found in build"
    fi
}

# Create GTK settings file
create_gtk_settings() {
    local output_dir="$1"
    
    print_info "Creating GTK settings file..."
    
    mkdir -p "$output_dir/etc/gtk-3.0"
    
    cat > "$output_dir/etc/gtk-3.0/settings.ini" << 'EOF'
[Settings]
gtk-theme-name=Windows-10
gtk-icon-theme-name=Adwaita
gtk-font-name=Segoe UI 9
gtk-cursor-theme-name=Adwaita
gtk-cursor-theme-size=24
gtk-toolbar-style=GTK_TOOLBAR_BOTH_HORIZ
gtk-toolbar-icon-size=GTK_ICON_SIZE_LARGE_TOOLBAR
gtk-button-images=1
gtk-menu-images=1
gtk-enable-event-sounds=1
gtk-enable-input-feedback-sounds=0
gtk-xft-antialias=1
gtk-xft-hinting=1
gtk-xft-hintstyle=hintfull
gtk-xft-rgba=rgb
gtk-application-prefer-dark-theme=0
EOF
    
    print_success "GTK settings file created"
}

# Update GDK pixbuf loaders cache
update_loaders_cache() {
    local output_dir="$1"
    
    print_info "Updating GDK pixbuf loaders cache..."
    
    if command -v gdk-pixbuf-query-loaders &> /dev/null; then
        local loaders_dir="$output_dir/lib/gdk-pixbuf-2.0/2.10.0/loaders"
        if [ -d "$loaders_dir" ]; then
            # Generate loaders.cache file
            local cache_file="$output_dir/lib/gdk-pixbuf-2.0/2.10.0/loaders.cache"
            gdk-pixbuf-query-loaders "$loaders_dir"/*.dll > "$cache_file" 2>/dev/null || true
            
            # Update paths in cache file to be relative
            if [ -f "$cache_file" ]; then
                sed -i 's|'"$output_dir"'|.|g' "$cache_file"
                print_success "GDK pixbuf loaders cache updated"
            fi
        fi
    else
        print_info "gdk-pixbuf-query-loaders not available, skipping cache update"
    fi
}

# Copy main executable
copy_executable() {
    local build_dir="$1"
    local output_dir="$2"
    
    print_info "Copying main executable..."
    
    local exe_path="$build_dir/src/multi-remmina.exe"
    if [ ! -f "$exe_path" ]; then
        print_error "Executable not found: $exe_path"
        echo "Please build the project first using windows-build.sh"
        exit 1
    fi
    
    cp -v "$exe_path" "$output_dir/"
    print_success "Main executable copied"
}

# Display usage information
usage() {
    cat << EOF
Usage: $0 [OPTIONS] <build_dir> <output_dir>

Create a self-contained Windows package for Multi-Remmina.

ARGUMENTS:
    build_dir               Build directory containing compiled binaries (default: build)
    output_dir              Output directory for the package (default: multi-remmina-win64)

OPTIONS:
    -h, --help              Show this help message
    --skip-gtk-runtime      Skip copying GTK runtime files
    --skip-resources        Skip copying application resources
    --skip-loaders-cache    Skip updating GDK pixbuf loaders cache

EXAMPLES:
    $0                                      # Use default directories
    $0 build dist                           # Custom directories
    $0 --skip-gtk-runtime build dist        # Skip GTK runtime files

REQUIREMENTS:
    - MSYS2 environment (MINGW64 or UCRT64)
    - Built Multi-Remmina executable in build directory
    - ldd tool for DLL dependency analysis

EOF
}

# Main script
main() {
    local build_dir="build"
    local output_dir="multi-remmina-win64"
    local skip_gtk_runtime=false
    local skip_resources=false
    local skip_loaders_cache=false
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                usage
                exit 0
                ;;
            --skip-gtk-runtime)
                skip_gtk_runtime=true
                shift
                ;;
            --skip-resources)
                skip_resources=true
                shift
                ;;
            --skip-loaders-cache)
                skip_loaders_cache=true
                shift
                ;;
            -*)
                print_error "Unknown option: $1"
                usage
                exit 1
                ;;
            *)
                if [ -z "$build_dir" ] || [ "$build_dir" = "build" ]; then
                    build_dir="$1"
                elif [ -z "$output_dir" ] || [ "$output_dir" = "multi-remmina-win64" ]; then
                    output_dir="$1"
                else
                    print_error "Too many arguments"
                    usage
                    exit 1
                fi
                shift
                ;;
        esac
    done
    
    print_info "Starting Multi-Remmina Windows bundle creation..."
    print_info "Build directory: $build_dir"
    print_info "Output directory: $output_dir"
    
    # Check for MSYS2 environment
    check_msys2
    
    # Check for required tools
    check_tools
    
    # Determine MinGW prefix
    local mingw_prefix=""
    case "$MSYSTEM" in
        MINGW64)
            mingw_prefix="/mingw64"
            ;;
        UCRT64)
            mingw_prefix="/ucrt64"
            ;;
        MINGW32)
            mingw_prefix="/mingw32"
            ;;
        *)
            print_error "Unsupported MSYS2 environment: $MSYSTEM"
            exit 1
            ;;
    esac
    print_info "Using MinGW prefix: $mingw_prefix"
    
    # Create output directory
    if [ -d "$output_dir" ]; then
        print_info "Output directory exists, cleaning..."
        rm -rf "$output_dir"
    fi
    mkdir -p "$output_dir"
    print_success "Output directory created: $output_dir"
    
    # Copy main executable
    copy_executable "$build_dir" "$output_dir"
    
    # Copy plugins
    copy_plugins "$build_dir" "$output_dir"
    
    # Collect all required DLLs
    collect_all_dlls "$output_dir"
    
    # Copy GTK runtime files unless skipped
    if [ "$skip_gtk_runtime" = false ]; then
        copy_gtk_runtime "$output_dir" "$mingw_prefix"
    else
        print_info "Skipping GTK runtime files"
    fi
    
    # Copy application resources unless skipped
    if [ "$skip_resources" = false ]; then
        copy_resources "$build_dir" "$output_dir"
    else
        print_info "Skipping application resources"
    fi
    
    # Create GTK settings file
    create_gtk_settings "$output_dir"
    
    # Update GDK pixbuf loaders cache unless skipped
    if [ "$skip_loaders_cache" = false ]; then
        update_loaders_cache "$output_dir"
    else
        print_info "Skipping GDK pixbuf loaders cache update"
    fi
    
    print_success "Multi-Remmina Windows bundle created successfully!"
    print_info "Package location: $output_dir"
    print_info "Executable: $output_dir/multi-remmina.exe"
    
    # Display package size
    if command -v du &> /dev/null; then
        local package_size=$(du -sh "$output_dir" | awk '{print $1}')
        print_info "Package size: $package_size"
    fi
}

# Run main function
main "$@"
