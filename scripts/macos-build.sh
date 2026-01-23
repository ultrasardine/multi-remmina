#!/bin/bash
# macOS Build Script for Remmina
# This script checks for Homebrew, installs dependencies, and builds Remmina on macOS

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

# Check if Homebrew is installed
check_homebrew() {
    print_info "Checking for Homebrew installation..."
    if ! command -v brew &> /dev/null; then
        print_error "Homebrew is not installed"
        echo "Please install Homebrew from https://brew.sh/"
        echo "Run: /bin/bash -c \"\$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\""
        exit 1
    fi
    print_success "Homebrew is installed"
}

# Check if a package is installed via Homebrew
is_package_installed() {
    brew list "$1" &> /dev/null
}

# Install required dependencies
install_dependencies() {
    print_info "Installing required dependencies via Homebrew..."
    
    # List of required packages
    local packages=(
        "cmake"
        "pkg-config"
        "gtk+3"
        "glib"
        "libsodium"
        "openssl@3"
        "libssh"
        "freerdp"
        "libvncserver"
        "gettext"
        "json-glib"
        "libgcrypt"
    )
    
    # Optional packages
    local optional_packages=(
        "libsoup@2"
        "vte3"
        "webkit2gtk"
    )
    
    local missing_packages=()
    local installed_count=0
    
    # Check which packages are missing
    for package in "${packages[@]}"; do
        if is_package_installed "$package"; then
            print_success "$package is already installed"
            ((installed_count++))
        else
            print_info "$package is not installed"
            missing_packages+=("$package")
        fi
    done
    
    # Install missing packages
    if [ ${#missing_packages[@]} -gt 0 ]; then
        print_info "Installing ${#missing_packages[@]} missing package(s)..."
        for package in "${missing_packages[@]}"; do
            print_info "Installing $package..."
            if brew install "$package"; then
                print_success "$package installed successfully"
            else
                print_error "Failed to install $package"
                exit 1
            fi
        done
    else
        print_success "All required dependencies are already installed"
    fi
    
    # Try to install optional packages (don't fail if they're not available)
    print_info "Checking optional dependencies..."
    for package in "${optional_packages[@]}"; do
        if is_package_installed "$package"; then
            print_success "$package is already installed"
        else
            print_info "Attempting to install optional package: $package"
            if brew install "$package" 2>/dev/null; then
                print_success "$package installed successfully"
            else
                print_info "$package is not available or failed to install (optional, continuing...)"
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

# Run CMake with macOS-specific options
run_cmake() {
    local build_dir="${1:-build}"
    local build_type="${2:-Release}"
    
    print_info "Running CMake with macOS-specific options..."
    
    cd "$build_dir"
    
    # macOS-specific CMake options
    cmake \
        -DCMAKE_BUILD_TYPE="$build_type" \
        -DCMAKE_INSTALL_PREFIX=/usr/local \
        -DWITH_AVAHI=OFF \
        -DWITH_APPINDICATOR=OFF \
        -DWITH_TELEPATHY=OFF \
        -DWITH_MACOS_KEYCHAIN=ON \
        -DCMAKE_PREFIX_PATH="$(brew --prefix)" \
        -DOPENSSL_ROOT_DIR="$(brew --prefix openssl@3)" \
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
    local cpu_cores=$(sysctl -n hw.ncpu)
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

Build Remmina on macOS with Homebrew dependencies.

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
    
    print_info "Starting Remmina macOS build process..."
    print_info "Build directory: $build_dir"
    print_info "Build type: $build_type"
    
    # Check for Homebrew
    check_homebrew
    
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
        print_success "Remmina build completed successfully!"
        print_info "Executable location: $build_dir/src/remmina"
    else
        print_info "CMake configuration completed (build skipped)"
    fi
}

# Run main function
main "$@"
