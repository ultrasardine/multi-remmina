# Remmina - CMake Configuration Tests
#
# This file contains tests for CMake build configuration
# Tests verify platform detection, feature flags, and framework linking

cmake_minimum_required(VERSION 3.10.0)

# Enable testing
enable_testing()

# Test 1: Platform detection on macOS
add_test(
  NAME test_macos_platform_detection
  COMMAND ${CMAKE_COMMAND}
    -DTEST_NAME=macos_platform_detection
    -DAPPLE=${APPLE}
    -DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/test_platform_detection.cmake
)

# Test 2: Linux-specific features disabled on macOS
if(APPLE)
  add_test(
    NAME test_macos_features_disabled
    COMMAND ${CMAKE_COMMAND}
      -DTEST_NAME=macos_features_disabled
      -DWITH_AVAHI=${WITH_AVAHI}
      -DWITH_APPINDICATOR=${WITH_APPINDICATOR}
      -DWITH_TELEPATHY=${WITH_TELEPATHY}
      -P ${CMAKE_CURRENT_SOURCE_DIR}/test_macos_features.cmake
  )
endif()

# Test 3: macOS frameworks linked when Keychain enabled
if(APPLE AND WITH_MACOS_KEYCHAIN)
  add_test(
    NAME test_macos_frameworks_linked
    COMMAND ${CMAKE_COMMAND}
      -DTEST_NAME=macos_frameworks_linked
      -DSECURITY_FRAMEWORK=${SECURITY_FRAMEWORK}
      -DCOCOA_FRAMEWORK=${COCOA_FRAMEWORK}
      -P ${CMAKE_CURRENT_SOURCE_DIR}/test_macos_frameworks.cmake
  )
endif()

# Test 4: RPATH configuration on macOS
if(APPLE)
  add_test(
    NAME test_macos_rpath_configuration
    COMMAND ${CMAKE_COMMAND}
      -DTEST_NAME=macos_rpath_configuration
      -DCMAKE_MACOSX_RPATH=${CMAKE_MACOSX_RPATH}
      -DCMAKE_INSTALL_RPATH=${CMAKE_INSTALL_RPATH}
      -P ${CMAKE_CURRENT_SOURCE_DIR}/test_macos_rpath.cmake
  )
endif()

# Test 5: Plugin dependencies can be found in Homebrew locations
if(APPLE)
  add_test(
    NAME test_plugin_homebrew_paths
    COMMAND ${CMAKE_COMMAND}
      -DTEST_NAME=plugin_homebrew_paths
      -DAPPLE=${APPLE}
      -DCMAKE_PREFIX_PATH="${CMAKE_PREFIX_PATH}"
      -P ${CMAKE_CURRENT_SOURCE_DIR}/test_plugin_homebrew_paths.cmake
  )
endif()

# Test 6: Plugin RPATH configured correctly for bundle loading
if(APPLE)
  add_test(
    NAME test_plugin_rpath_configuration
    COMMAND ${CMAKE_COMMAND}
      -DTEST_NAME=plugin_rpath_configuration
      -DAPPLE=${APPLE}
      -DCMAKE_INSTALL_RPATH="${CMAKE_INSTALL_RPATH}"
      -DCMAKE_BUILD_WITH_INSTALL_RPATH=${CMAKE_BUILD_WITH_INSTALL_RPATH}
      -P ${CMAKE_CURRENT_SOURCE_DIR}/test_plugin_rpath.cmake
  )
endif()

# ============================================================================
# Windows-specific tests
# ============================================================================

# Test 7: Platform detection on Windows
if(WIN32)
  add_test(
    NAME test_windows_platform_detection
    COMMAND ${CMAKE_COMMAND}
      -DTEST_NAME=windows_platform_detection
      -DWIN32=${WIN32}
      -DMINGW=${MINGW}
      -DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}
      -P ${CMAKE_CURRENT_SOURCE_DIR}/test_windows_platform_detection.cmake
  )
endif()

# Test 8: Linux-specific features disabled on Windows
if(WIN32)
  add_test(
    NAME test_windows_features_disabled
    COMMAND ${CMAKE_COMMAND}
      -DTEST_NAME=windows_features_disabled
      -DWITH_AVAHI=${WITH_AVAHI}
      -DWITH_APPINDICATOR=${WITH_APPINDICATOR}
      -DWITH_TELEPATHY=${WITH_TELEPATHY}
      -P ${CMAKE_CURRENT_SOURCE_DIR}/test_windows_features.cmake
  )
endif()

# Test 9: Windows APIs linked when enabled
if(WIN32)
  add_test(
    NAME test_windows_apis_linked
    COMMAND ${CMAKE_COMMAND}
      -DTEST_NAME=windows_apis_linked
      -DWITH_WINDOWS_CREDENTIAL_MANAGER=${WITH_WINDOWS_CREDENTIAL_MANAGER}
      -DADVAPI32_LIBRARY=${ADVAPI32_LIBRARY}
      -DWITH_WINDOWS_DNSSD=${WITH_WINDOWS_DNSSD}
      -DDNSAPI_LIBRARY=${DNSAPI_LIBRARY}
      -P ${CMAKE_CURRENT_SOURCE_DIR}/test_windows_apis.cmake
  )
endif()

# Test 10: MSYS2/MinGW paths configured on Windows
if(WIN32 AND MINGW)
  add_test(
    NAME test_windows_msys2_paths
    COMMAND ${CMAKE_COMMAND}
      -DTEST_NAME=windows_msys2_paths
      -DWIN32=${WIN32}
      -DMINGW=${MINGW}
      -DCMAKE_PREFIX_PATH="${CMAKE_PREFIX_PATH}"
      -P ${CMAKE_CURRENT_SOURCE_DIR}/test_windows_msys2_paths.cmake
  )
endif()

# Test 11: Windows plugin DLL configuration
if(WIN32)
  add_test(
    NAME test_windows_plugin_dll
    COMMAND ${CMAKE_COMMAND}
      -DTEST_NAME=windows_plugin_dll
      -DWIN32=${WIN32}
      -DCMAKE_SHARED_MODULE_SUFFIX=${CMAKE_SHARED_MODULE_SUFFIX}
      -DCMAKE_SHARED_LIBRARY_SUFFIX=${CMAKE_SHARED_LIBRARY_SUFFIX}
      -DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=${CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS}
      -DCMAKE_SKIP_RPATH=${CMAKE_SKIP_RPATH}
      -P ${CMAKE_CURRENT_SOURCE_DIR}/test_windows_plugin_dll.cmake
  )
endif()
