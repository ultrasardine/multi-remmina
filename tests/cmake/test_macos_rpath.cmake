# Test: RPATH configuration on macOS
# Validates: Requirements 1.4
#
# This test verifies that RPATH is correctly configured for macOS application bundles

message(STATUS "Running test: ${TEST_NAME}")

# Check that CMAKE_MACOSX_RPATH is enabled
if(NOT CMAKE_MACOSX_RPATH)
  message(FATAL_ERROR "CMAKE_MACOSX_RPATH should be ON on macOS, but it is: ${CMAKE_MACOSX_RPATH}")
endif()
message(STATUS "✓ CMAKE_MACOSX_RPATH is correctly set to ON")

# Check that CMAKE_INSTALL_RPATH contains bundle-relative paths
set(EXPECTED_RPATH "@executable_path/../Frameworks;@executable_path/../lib")
if(NOT CMAKE_INSTALL_RPATH STREQUAL EXPECTED_RPATH)
  message(FATAL_ERROR "CMAKE_INSTALL_RPATH should be '${EXPECTED_RPATH}', but it is: '${CMAKE_INSTALL_RPATH}'")
endif()
message(STATUS "✓ CMAKE_INSTALL_RPATH is correctly set to: ${CMAKE_INSTALL_RPATH}")

# Verify RPATH contains @executable_path for bundle compatibility
string(FIND "${CMAKE_INSTALL_RPATH}" "@executable_path" RPATH_HAS_EXECUTABLE_PATH)
if(RPATH_HAS_EXECUTABLE_PATH EQUAL -1)
  message(FATAL_ERROR "CMAKE_INSTALL_RPATH should contain '@executable_path' for bundle compatibility")
endif()
message(STATUS "✓ RPATH contains @executable_path for bundle compatibility")

message(STATUS "Test ${TEST_NAME} PASSED")
