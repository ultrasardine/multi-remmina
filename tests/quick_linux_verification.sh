#!/bin/bash
# Quick Linux build verification
# Performs static analysis of the codebase to verify Linux compatibility

set -e

echo "=========================================="
echo "Quick Linux Build Verification"
echo "=========================================="
echo ""

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

print_status() {
    if [ $1 -eq 0 ]; then
        echo -e "${GREEN}✓ $2${NC}"
    else
        echo -e "${RED}✗ $2${NC}"
        return 1
    fi
}

print_warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
}

ERRORS=0

echo "1. Verifying platform-specific code guards..."
echo "----------------------------------------------"

# Check that all macOS-specific code is properly guarded
echo "Checking macOS-specific files..."

MACOS_FILES=(
    "src/remmina_keychain_macos.c"
    "src/remmina_bonjour_macos.c"
    "src/remmina_bundle_macos.c"
)

for file in "${MACOS_FILES[@]}"; do
    if [ -f "$file" ]; then
        if head -50 "$file" | grep -q "#ifdef __APPLE__"; then
            print_status 0 "$file has proper guard"
        else
            print_status 1 "$file missing #ifdef __APPLE__ guard"
            ERRORS=$((ERRORS + 1))
        fi
    fi
done

echo ""
echo "2. Verifying Linux-specific code is not broken..."
echo "--------------------------------------------------"

# Check that Linux-specific includes are properly guarded
# Look for the include and check if it's preceded by #ifndef __APPLE__
DESKTOP_APP_FILES=$(grep -l "gio/gdesktopappinfo.h" src/*.c 2>/dev/null || true)
if [ -n "$DESKTOP_APP_FILES" ]; then
    ALL_GUARDED=true
    for file in $DESKTOP_APP_FILES; do
        # Check if the line before the include has #ifndef __APPLE__
        if grep -B 1 "gio/gdesktopappinfo.h" "$file" | grep -q "#ifndef __APPLE__"; then
            : # Properly guarded
        else
            ALL_GUARDED=false
            break
        fi
    done
    
    if $ALL_GUARDED; then
        print_status 0 "GDesktopAppInfo properly guarded"
    else
        print_status 1 "GDesktopAppInfo included without __APPLE__ guard"
        ERRORS=$((ERRORS + 1))
    fi
else
    print_status 0 "No GDesktopAppInfo includes found (OK)"
fi

# Check Avahi integration
if grep -r "avahi" src/remmina_avahi.c | grep -q "#ifdef __APPLE__"; then
    print_status 0 "Avahi code has macOS alternative"
else
    print_warning "Avahi code structure unclear"
fi

echo ""
echo "3. Verifying CMake platform detection..."
echo "-----------------------------------------"

# Check CMakeLists.txt for proper platform detection
if grep -q "if(APPLE)" CMakeLists.txt; then
    print_status 0 "CMake has APPLE platform detection"
else
    print_status 1 "CMake missing APPLE platform detection"
    ERRORS=$((ERRORS + 1))
fi

# Check that Linux features are not disabled unconditionally
if grep -q "WITH_AVAHI=OFF" CMakeLists.txt && ! grep -q "if(APPLE)" CMakeLists.txt; then
    print_status 1 "Avahi disabled unconditionally"
    ERRORS=$((ERRORS + 1))
else
    print_status 0 "Avahi configuration looks correct"
fi

echo ""
echo "4. Verifying plugin system compatibility..."
echo "--------------------------------------------"

# Check secret plugin for platform abstraction
if [ -f "plugins/secret/src/glibsecret_plugin.c" ]; then
    if (grep -q "#ifdef __APPLE__\|#if defined(__APPLE__)\|#if defined(_WIN32)" plugins/secret/src/glibsecret_plugin.c) && \
       grep -q "#else" plugins/secret/src/glibsecret_plugin.c; then
        print_status 0 "Secret plugin has platform abstraction"
    else
        print_status 1 "Secret plugin missing platform abstraction"
        ERRORS=$((ERRORS + 1))
    fi
fi

# Check plugin CMakeLists for platform-specific configuration
if [ -f "plugins/secret/CMakeLists.txt" ]; then
    if grep -q "if(APPLE)\|if(WIN32)\|IF(APPLE)\|IF(WIN32)" plugins/secret/CMakeLists.txt; then
        print_status 0 "Secret plugin CMake has platform detection"
    else
        print_status 1 "Secret plugin CMake missing platform detection"
        ERRORS=$((ERRORS + 1))
    fi
fi

echo ""
echo "5. Checking for common Linux compatibility issues..."
echo "-----------------------------------------------------"

# Check for macOS-specific headers in non-guarded code
MACOS_HEADERS=(
    "Security/Security.h"
    "CoreFoundation/CoreFoundation.h"
    "Cocoa/Cocoa.h"
)

for header in "${MACOS_HEADERS[@]}"; do
    # Find includes of this header
    MATCHES=$(grep -r "#include <$header>" src/ 2>/dev/null | grep -v "Binary file" || true)
    
    if [ -n "$MATCHES" ]; then
        # Check if they're all properly guarded
        while IFS= read -r match; do
            FILE=$(echo "$match" | cut -d: -f1)
            LINE=$(echo "$match" | cut -d: -f2-)
            
            # Get context around the include
            CONTEXT=$(grep -B 5 "$LINE" "$FILE" 2>/dev/null || true)
            
            if echo "$CONTEXT" | grep -q "#ifdef __APPLE__"; then
                print_status 0 "$header in $FILE is guarded"
            else
                print_status 1 "$header in $FILE is NOT guarded"
                ERRORS=$((ERRORS + 1))
            fi
        done <<< "$MATCHES"
    fi
done

echo ""
echo "6. Verifying test suite structure..."
echo "-------------------------------------"

# Check that macOS-specific tests are properly guarded
MACOS_TEST_DIRS=(
    "tests/keychain"
    "tests/bundle_resources"
    "tests/service_discovery"
)

for dir in "${MACOS_TEST_DIRS[@]}"; do
    if [ -d "$dir" ]; then
        if grep -q "if(APPLE)" "$dir/CMakeLists.txt" 2>/dev/null; then
            print_status 0 "$dir tests are macOS-only"
        else
            print_warning "$dir tests may run on Linux"
        fi
    fi
done

echo ""
echo "7. Summary of platform-specific code..."
echo "----------------------------------------"

echo ""
echo "macOS-only source files:"
find src -name "*macos*" -type f | while read -r file; do
    echo "  - $file"
done

echo ""
echo "Files with platform-specific sections:"
grep -l "#ifdef __APPLE__\|#ifndef __APPLE__" src/*.c 2>/dev/null | while read -r file; do
    APPLE_COUNT=$(grep -c "#ifdef __APPLE__" "$file" 2>/dev/null || echo 0)
    NOT_APPLE_COUNT=$(grep -c "#ifndef __APPLE__" "$file" 2>/dev/null || echo 0)
    echo "  - $file (#ifdef: $APPLE_COUNT, #ifndef: $NOT_APPLE_COUNT)"
done

echo ""
echo "=========================================="
echo "Verification Results"
echo "=========================================="
echo ""

if [ $ERRORS -eq 0 ]; then
    echo -e "${GREEN}✓ All checks passed${NC}"
    echo ""
    echo "The codebase structure indicates that Linux builds"
    echo "should not be broken by macOS port changes."
    echo ""
    echo "Recommendation: Run full Docker verification or CI pipeline"
    echo "to confirm actual build success on Linux."
    exit 0
else
    echo -e "${RED}✗ Found $ERRORS issue(s)${NC}"
    echo ""
    echo "Please fix the issues above before proceeding."
    exit 1
fi
