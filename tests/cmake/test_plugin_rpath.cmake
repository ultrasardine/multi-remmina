# Test: Plugin RPATH is configured correctly for bundle loading
# This test verifies that plugins have the correct RPATH settings
# to find libraries within the macOS application bundle

if(NOT DEFINED TEST_NAME)
  message(FATAL_ERROR "TEST_NAME not defined")
endif()

message(STATUS "Running test: ${TEST_NAME}")

# Test should only run on macOS
if(NOT APPLE)
  message(STATUS "Test ${TEST_NAME}: SKIPPED (not macOS)")
  return()
endif()

# Verify CMAKE_INSTALL_RPATH is set
if(NOT DEFINED CMAKE_INSTALL_RPATH)
  message(FATAL_ERROR "Test ${TEST_NAME}: FAILED - CMAKE_INSTALL_RPATH not defined")
endif()

message(STATUS "  CMAKE_INSTALL_RPATH: ${CMAKE_INSTALL_RPATH}")

# Required RPATH components for bundle loading
set(REQUIRED_RPATH_COMPONENTS
  "@executable_path/../Frameworks"
  "@executable_path/../lib"
  "@loader_path"
)

# Convert RPATH string to list (split by semicolon)
string(REPLACE ";" ";" RPATH_LIST "${CMAKE_INSTALL_RPATH}")

# Check each required component
set(MISSING_COMPONENTS "")
foreach(COMPONENT ${REQUIRED_RPATH_COMPONENTS})
  list(FIND RPATH_LIST ${COMPONENT} COMPONENT_INDEX)
  if(COMPONENT_INDEX EQUAL -1)
    list(APPEND MISSING_COMPONENTS ${COMPONENT})
  else()
    message(STATUS "  Found required RPATH component: ${COMPONENT}")
  endif()
endforeach()

# Report results
if(MISSING_COMPONENTS)
  message(FATAL_ERROR "Test ${TEST_NAME}: FAILED - Missing RPATH components: ${MISSING_COMPONENTS}")
endif()

# Verify CMAKE_BUILD_WITH_INSTALL_RPATH is TRUE
if(DEFINED CMAKE_BUILD_WITH_INSTALL_RPATH)
  if(CMAKE_BUILD_WITH_INSTALL_RPATH)
    message(STATUS "  CMAKE_BUILD_WITH_INSTALL_RPATH: TRUE (correct)")
  else()
    message(WARNING "  CMAKE_BUILD_WITH_INSTALL_RPATH: FALSE (should be TRUE for plugins)")
  endif()
endif()

message(STATUS "Test ${TEST_NAME}: PASSED")
message(STATUS "  - All required RPATH components present")
message(STATUS "  - Plugins will be able to find libraries in bundle")
