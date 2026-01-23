# Test: Plugin dependencies can be found in Homebrew locations
# This test verifies that CMake can locate FreeRDP, libvncserver, and libssh
# from Homebrew installation paths on macOS

if(NOT DEFINED TEST_NAME)
  message(FATAL_ERROR "TEST_NAME not defined")
endif()

message(STATUS "Running test: ${TEST_NAME}")

# Test should only run on macOS
if(NOT APPLE)
  message(STATUS "Test ${TEST_NAME}: SKIPPED (not macOS)")
  return()
endif()

# Check if CMAKE_PREFIX_PATH includes Homebrew paths
set(REQUIRED_PATHS
  "/usr/local"
  "/opt/homebrew"
)

set(PLUGIN_SPECIFIC_PATHS
  "/usr/local/opt/freerdp"
  "/opt/homebrew/opt/freerdp"
  "/usr/local/opt/libvncserver"
  "/opt/homebrew/opt/libvncserver"
  "/usr/local/opt/libssh"
  "/opt/homebrew/opt/libssh"
)

# Verify at least one Homebrew base path is in CMAKE_PREFIX_PATH
set(FOUND_BASE_PATH FALSE)
foreach(PATH ${REQUIRED_PATHS})
  list(FIND CMAKE_PREFIX_PATH ${PATH} PATH_INDEX)
  if(NOT PATH_INDEX EQUAL -1)
    set(FOUND_BASE_PATH TRUE)
    message(STATUS "  Found Homebrew base path: ${PATH}")
    break()
  endif()
endforeach()

if(NOT FOUND_BASE_PATH)
  message(FATAL_ERROR "Test ${TEST_NAME}: FAILED - No Homebrew base paths found in CMAKE_PREFIX_PATH")
endif()

# Verify plugin-specific paths are in CMAKE_PREFIX_PATH
set(FOUND_PLUGIN_PATHS 0)
foreach(PATH ${PLUGIN_SPECIFIC_PATHS})
  list(FIND CMAKE_PREFIX_PATH ${PATH} PATH_INDEX)
  if(NOT PATH_INDEX EQUAL -1)
    math(EXPR FOUND_PLUGIN_PATHS "${FOUND_PLUGIN_PATHS} + 1")
    message(STATUS "  Found plugin path: ${PATH}")
  endif()
endforeach()

if(FOUND_PLUGIN_PATHS EQUAL 0)
  message(FATAL_ERROR "Test ${TEST_NAME}: FAILED - No plugin-specific Homebrew paths found")
endif()

message(STATUS "Test ${TEST_NAME}: PASSED")
message(STATUS "  - Found ${FOUND_PLUGIN_PATHS} plugin-specific Homebrew paths")
