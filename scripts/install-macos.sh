#!/bin/bash
# Multi-Remmina macOS Installer
# This script removes the quarantine attribute and installs the app

set -e

echo "Multi-Remmina macOS Installer"
echo "=============================="
echo ""

# Check if running on macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    echo "Error: This script is for macOS only"
    exit 1
fi

# Find the app
APP_PATH=""
if [ -f "Multi-Remmina.app/Contents/Info.plist" ]; then
    APP_PATH="Multi-Remmina.app"
elif [ -f "/Volumes/Multi-Remmina/Multi-Remmina.app/Contents/Info.plist" ]; then
    APP_PATH="/Volumes/Multi-Remmina/Multi-Remmina.app"
else
    echo "Error: Multi-Remmina.app not found"
    echo "Please mount the DMG first or run this script from the DMG folder"
    exit 1
fi

echo "Found Multi-Remmina.app at: $APP_PATH"
echo ""

# Remove quarantine
echo "Removing quarantine attributes..."
xattr -cr "$APP_PATH"

# Copy to Applications
echo "Installing to /Applications..."
if [ -d "/Applications/Multi-Remmina.app" ]; then
    echo "Removing existing installation..."
    rm -rf "/Applications/Multi-Remmina.app"
fi

cp -R "$APP_PATH" /Applications/

# Remove quarantine from installed app too
echo "Finalizing installation..."
xattr -cr /Applications/Multi-Remmina.app

echo ""
echo "✓ Installation complete!"
echo ""
echo "You can now open Multi-Remmina from your Applications folder."
echo "Or run: open /Applications/Multi-Remmina.app"
