# Test: Windows plugin DLL configuration
# Validates: Requirements 1.7, 3.1
#
# This test verifies that plugins are configured correctly for Windows DLL output

message(STATUS "Running test: ${TEST_NAME}")

if(WIN32)
  # Check that CMAKE_SHARED_MODULE_SUFFIX is set to .dll
  if(NOT CMAKE_SHARED_MODULE_SUFFIX STREQUAL ".dll")
    message(FATAL_ERROR "CMAKE_SHARED_MODULE_SUFFIX should be .dll on Windows, got: ${CMAKE_SHARED_MODULE_SUFFIX}")
  endif()
  message(STATUS "✓ CMAKE_SHARED_MODULE_SUFFIX is .dll")
  
  # Check that CMAKE_SHARED_LIBRARY_SUFFIX is set to .dll
  if(NOT CMAKE_SHARED_LIBRARY_SUFFIX STREQUAL ".dll")
    message(FATAL_ERROR "CMAKE_SHARED_LIBRARY_SUFFIX should be .dll on Windows, got: ${CMAKE_SHARED_LIBRARY_SUFFIX}")
  endif()
  message(STATUS "✓ CMAKE_SHARED_LIBRARY_SUFFIX is .dll")
  
  # Check that CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS is ON
  if(NOT CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS)
    message(FATAL_ERROR "CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS should be ON for plugin loading")
  endif()
  message(STATUS "✓ CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS is ON")
  
  # Check that RPATH is skipped on Windows
  if(NOT CMAKE_SKIP_RPATH)
    message(FATAL_ERROR "CMAKE_SKIP_RPATH should be TRUE on Windows")
  endif()
  message(STATUS "✓ CMAKE_SKIP_RPATH is TRUE")
  
else()
  message(STATUS "Not running on Windows, skipping plugin DLL checks")
endif()

message(STATUS "Test ${TEST_NAME} PASSED")
