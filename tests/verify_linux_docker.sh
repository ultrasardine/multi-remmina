#!/bin/bash
# Docker-based Linux build verification
# This script can be run from macOS to verify Linux builds in a container

set -e

echo "=========================================="
echo "Remmina Linux Build Verification (Docker)"
echo "=========================================="
echo ""

# Check if Docker/Podman is available
if command -v podman >/dev/null 2>&1; then
    DOCKER_CMD="podman"
    echo "Using Podman"
elif command -v docker >/dev/null 2>&1; then
    DOCKER_CMD="docker"
    echo "Using Docker"
else
    echo "Error: Neither Docker nor Podman is installed"
    exit 1
fi

# Use Ubuntu 22.04 as base image (matches CI)
IMAGE="ubuntu:22.04"

echo "Pulling Ubuntu 22.04 image..."
$DOCKER_CMD pull $IMAGE

echo ""
echo "Running Linux build verification in container..."
echo ""

# Run the verification in a container
$DOCKER_CMD run --rm \
    -v "$(pwd):/workspace" \
    -w /workspace \
    $IMAGE \
    bash -c '
        export DEBIAN_FRONTEND=noninteractive
        
        echo "Installing build dependencies..."
        apt-get update -qq
        apt-get install -y -qq \
            build-essential \
            cmake \
            git \
            pkg-config \
            libgtk-3-dev \
            libglib2.0-dev \
            libssh-dev \
            libssl-dev \
            libsodium-dev \
            libavahi-ui-gtk3-dev \
            libappindicator3-dev \
            libsecret-1-dev \
            libvncserver-dev \
            libpcre2-dev \
            libjson-glib-dev \
            libsoup2.4-dev \
            libvte-2.91-dev \
            libxkbfile-dev \
            freerdp2-dev \
            2>&1 | grep -v "^Selecting\|^Preparing\|^Unpacking\|^Setting up\|^Processing"
        
        echo ""
        echo "Running verification script..."
        bash tests/verify_linux_build.sh
    '

VERIFICATION_STATUS=$?

if [ $VERIFICATION_STATUS -eq 0 ]; then
    echo ""
    echo "✓ Linux build verification PASSED"
    echo ""
else
    echo ""
    echo "✗ Linux build verification FAILED"
    echo ""
    exit 1
fi
