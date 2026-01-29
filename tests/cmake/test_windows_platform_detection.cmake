# Test: Platform Detection on Windows
# Validates: Requirements 1.1
#
# This test verifies that CMake correctly detects Windows platform

message(STATUS "Running test: ${TEST_NAME}")

# Check if WIN32 is set to TRUE on Windows
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  if(NOT WIN32)
    message(FATAL_ERROR "WIN32 should be TRUE on Windows platform, but it is: ${WIN32}")
  endif()
  message(STATUS "✓ WIN32 is correctly set to TRUE on Windows")
else()
  message(STATUS "Not running on Windows, skipping Windows-specific checks")
endif()

# Verify CMAKE_SYSTEM_NAME is set correctly
if(WIN32)
  if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(FATAL_ERROR "CMAKE_SYSTEM_NAME should be 'Windows' on Windows, but it is: ${CMAKE_SYSTEM_NAME}")
  endif()
  message(STATUS "✓ CMAKE_SYSTEM_NAME is correctly set to Windows")
endif()

# Check MinGW detection if applicable
if(WIN32 AND MINGW)
  message(STATUS "✓ MINGW is correctly detected")
endif()

message(STATUS "Test ${TEST_NAME} PASSED")
