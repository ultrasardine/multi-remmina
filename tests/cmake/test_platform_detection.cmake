# Test: Platform Detection on macOS
# Validates: Requirements 1.1
#
# This test verifies that CMake correctly detects macOS platform

message(STATUS "Running test: ${TEST_NAME}")

# Check if APPLE is set to TRUE on macOS
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  if(NOT APPLE)
    message(FATAL_ERROR "APPLE should be TRUE on Darwin platform, but it is: ${APPLE}")
  endif()
  message(STATUS "✓ APPLE is correctly set to TRUE on Darwin")
else()
  message(STATUS "Not running on Darwin, skipping macOS-specific checks")
endif()

# Verify CMAKE_SYSTEM_NAME is set correctly
if(APPLE)
  if(NOT CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(FATAL_ERROR "CMAKE_SYSTEM_NAME should be 'Darwin' on macOS, but it is: ${CMAKE_SYSTEM_NAME}")
  endif()
  message(STATUS "✓ CMAKE_SYSTEM_NAME is correctly set to Darwin")
endif()

message(STATUS "Test ${TEST_NAME} PASSED")
