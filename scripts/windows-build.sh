#!/bin/bash
# Windows Build Script for Multi-Remmina (MSYS2)
# This script checks for MSYS2 environment, installs dependencies, and builds Multi-Remmina on Windows

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
        echo "Download MSYS2 from: https://www.msys2.org/"
        exit 1
    fi
    
    print_success "Running in MSYS2 environment: $MSYSTEM"
    
    # Check if pacman is available
    if ! command -v pacman &> /dev/null; then
        print_error "pacman package manager not found"
        echo "Please ensure you are running from MSYS2 shell"
        exit 1
    fi
    
    print_success "pacman package manager is available"
}

# Check if a package is installed via pacman
is_package_installed() {
    pacman -Q "$1" &> /dev/null
}

# Install required dependencies
install_dependencies() {
    print_info "Installing required dependencies via pacman..."
    
    # Determine package prefix based on MSYS2 environment
    local pkg_prefix=""
    case "$MSYSTEM" in
        MINGW64)
            pkg_prefix="mingw-w64-x86_64"
            ;;
        UCRT64)
            pkg_prefix="mingw-w64-ucrt-x86_64"
            ;;
        MINGW32)
            pkg_prefix="mingw-w64-i686"
            ;;
        *)
            print_error "Unsupported MSYS2 environment: $MSYSTEM"
            echo "Please use MINGW64 or UCRT64 environment"
            exit 1
            ;;
    esac
    
    print_info "Using package prefix: $pkg_prefix"
    
    # List of required packages (without prefix)
    local base_packages=(
        "cmake"
        "make"
        "gcc"
        "pkg-config"
        "gtk3"
        "glib2"
        "libsodium"
        "openssl"
        "libssh"
        "freerdp"
        "libvncserver"
        "gettext"
        "json-glib"
        "libgcrypt"
    )
    
    # Optional packages
    local optional_packages=(
        "libsoup"
        "vte3"
    )
    
    local missing_packages=()
    local installed_count=0
    
    # Check which packages are missing
    for package in "${base_packages[@]}"; do
        local full_package="${pkg_prefix}-${package}"
        if is_package_installed "$full_package"; then
            print_success "$full_package is already installed"
            ((installed_count++))
        else
            print_info "$full_package is not installed"
            missing_packages+=("$full_package")
        fi
    done
    
    # Install missing packages
    if [ ${#missing_packages[@]} -gt 0 ]; then
        print_info "Installing ${#missing_packages[@]} missing package(s)..."
        print_info "Running: pacman -S --noconfirm ${missing_packages[*]}"
        
        if pacman -S --noconfirm "${missing_packages[@]}"; then
            print_success "All required packages installed successfully"
        else
            print_error "Failed to install required packages"
            echo "Please run manually: pacman -S ${missing_packages[*]}"
            exit 1
        fi
    else
        print_success "All required dependencies are already installed"
    fi
    
    # Try to install optional packages (don't fail if they're not available)
    print_info "Checking optional dependencies..."
    for package in "${optional_packages[@]}"; do
        local full_package="${pkg_prefix}-${package}"
        if is_package_installed "$full_package"; then
            print_success "$full_package is already installed"
        else
            print_info "Attempting to install optional package: $full_package"
            if pacman -S --noconfirm "$full_package" 2>/dev/null; then
                print_success "$full_package installed successfully"
            else
                print_info "$full_package is not available or failed to install (optional, continuing...)"
            fi
        fi
    done
}

# Create build directory
create_build_directory() {
    local build_dir="${1:-build}"
    
    print_info "Creating build directory: $build_dir"
    
    if [ -d "$build_dir" ]; then
        print_info "Build directory already exists, cleaning..."
        rm -rf "$build_dir"
    fi
    
    mkdir -p "$build_dir"
    print_success "Build directory created: $build_dir"
}

# Run CMake with Windows-specific options
run_cmake() {
    local build_dir="${1:-build}"
    local build_type="${2:-Release}"
    
    print_info "Running CMake with Windows-specific options..."
    
    cd "$build_dir"
    
    # Windows-specific CMake options
    cmake \
        -G "MSYS Makefiles" \
        -DCMAKE_BUILD_TYPE="$build_type" \
        -DCMAKE_INSTALL_PREFIX=/mingw64 \
        -DWITH_AVAHI=OFF \
        -DWITH_APPINDICATOR=OFF \
        -DWITH_TELEPATHY=OFF \
        -DWITH_LIBSECRET=OFF \
        -DWITH_WEBKIT2GTK=OFF \
        -DWITH_WINDOWS_CREDENTIAL_MANAGER=ON \
        -DWITH_FREERDP3=ON \
        ..
    
    if [ $? -eq 0 ]; then
        print_success "CMake configuration completed successfully"
    else
        print_error "CMake configuration failed"
        cd ..
        exit 1
    fi
    
    cd ..
}

# Build with make using all CPU cores
build_project() {
    local build_dir="${1:-build}"
    
    print_info "Building project with make..."
    
    cd "$build_dir"
    
    # Get number of CPU cores
    local cpu_cores=$(nproc 2>/dev/null || echo 4)
    print_info "Using $cpu_cores CPU cores for parallel build"
    
    if make -j"$cpu_cores"; then
        print_success "Build completed successfully"
    else
        print_error "Build failed"
        cd ..
        exit 1
    fi
    
    cd ..
}

# Display usage information
usage() {
    cat << EOF
Usage: $0 [OPTIONS]

Build Multi-Remmina on Windows with MSYS2 dependencies.

OPTIONS:
    -h, --help              Show this help message
    -d, --build-dir DIR     Specify build directory (default: build)
    -t, --build-type TYPE   Specify build type: Release or Debug (default: Release)
    --skip-deps             Skip dependency installation
    --cmake-only            Only run CMake configuration, don't build

EXAMPLES:
    $0                      # Build with default settings
    $0 -d build-debug -t Debug  # Debug build in custom directory
    $0 --skip-deps          # Build without checking/installing dependencies

REQUIREMENTS:
    - MSYS2 environment (MINGW64 or UCRT64)
    - Run from MSYS2 MinGW 64-bit shell
    - Internet connection for package installation

EOF
}

# Main script
main() {
    local build_dir="build"
    local build_type="Release"
    local skip_deps=false
    local cmake_only=false
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                usage
                exit 0
                ;;
            -d|--build-dir)
                build_dir="$2"
                shift 2
                ;;
            -t|--build-type)
                build_type="$2"
                if [[ "$build_type" != "Release" && "$build_type" != "Debug" ]]; then
                    print_error "Invalid build type: $build_type (must be Release or Debug)"
                    exit 1
                fi
                shift 2
                ;;
            --skip-deps)
                skip_deps=true
                shift
                ;;
            --cmake-only)
                cmake_only=true
                shift
                ;;
            *)
                print_error "Unknown option: $1"
                usage
                exit 1
                ;;
        esac
    done
    
    print_info "Starting Multi-Remmina Windows build process..."
    print_info "Build directory: $build_dir"
    print_info "Build type: $build_type"
    
    # Check for MSYS2 environment
    check_msys2
    
    # Install dependencies unless skipped
    if [ "$skip_deps" = false ]; then
        install_dependencies
    else
        print_info "Skipping dependency installation"
    fi
    
    # Create build directory
    create_build_directory "$build_dir"
    
    # Run CMake
    run_cmake "$build_dir" "$build_type"
    
    # Build project unless cmake-only
    if [ "$cmake_only" = false ]; then
        build_project "$build_dir"
        print_success "Multi-Remmina build completed successfully!"
        print_info "Executable location: $build_dir/src/multi-remmina.exe"
    else
        print_info "CMake configuration completed (build skipped)"
    fi
}

# Run main function
main "$@"
