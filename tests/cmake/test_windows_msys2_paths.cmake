# Test: MSYS2/MinGW paths configured on Windows
# Validates: Requirements 1.5
#
# This test verifies that MSYS2/MinGW paths are added to CMAKE_PREFIX_PATH
# for dependency discovery on Windows

message(STATUS "Running test: ${TEST_NAME}")

# Check that CMAKE_PREFIX_PATH contains MSYS2 paths
if(WIN32 AND MINGW)
  # Convert CMAKE_PREFIX_PATH to string for checking
  string(FIND "${CMAKE_PREFIX_PATH}" "/mingw64" MINGW64_FOUND)
  string(FIND "${CMAKE_PREFIX_PATH}" "/ucrt64" UCRT64_FOUND)
  
  if(MINGW64_FOUND EQUAL -1 AND UCRT64_FOUND EQUAL -1)
    message(FATAL_ERROR "CMAKE_PREFIX_PATH should contain /mingw64 or /ucrt64 paths on Windows/MinGW")
  endif()
  
  if(NOT MINGW64_FOUND EQUAL -1)
    message(STATUS "✓ /mingw64 path found in CMAKE_PREFIX_PATH")
  endif()
  
  if(NOT UCRT64_FOUND EQUAL -1)
    message(STATUS "✓ /ucrt64 path found in CMAKE_PREFIX_PATH")
  endif()
else()
  message(STATUS "Not running on Windows/MinGW, skipping MSYS2 path checks")
endif()

message(STATUS "Test ${TEST_NAME} PASSED")
