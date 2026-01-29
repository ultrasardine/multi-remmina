# Test: Linux-specific features disabled on Windows
# Validates: Requirements 1.2, 6.2
#
# This test verifies that Linux-specific features (Avahi, AppIndicator, Telepathy)
# are disabled on Windows

message(STATUS "Running test: ${TEST_NAME}")

# Check that Avahi is disabled
if(WITH_AVAHI)
  message(FATAL_ERROR "WITH_AVAHI should be OFF on Windows, but it is: ${WITH_AVAHI}")
endif()
message(STATUS "✓ WITH_AVAHI is correctly set to OFF")

# Check that AppIndicator is disabled
if(WITH_APPINDICATOR)
  message(FATAL_ERROR "WITH_APPINDICATOR should be OFF on Windows, but it is: ${WITH_APPINDICATOR}")
endif()
message(STATUS "✓ WITH_APPINDICATOR is correctly set to OFF")

# Check that Telepathy is disabled
if(WITH_TELEPATHY)
  message(FATAL_ERROR "WITH_TELEPATHY should be OFF on Windows, but it is: ${WITH_TELEPATHY}")
endif()
message(STATUS "✓ WITH_TELEPATHY is correctly set to OFF")

message(STATUS "Test ${TEST_NAME} PASSED")
