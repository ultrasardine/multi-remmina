# Test: macOS frameworks linked when Keychain enabled
# Validates: Requirements 1.6
#
# This test verifies that Security and Cocoa frameworks are found and linked
# when WITH_MACOS_KEYCHAIN is enabled

message(STATUS "Running test: ${TEST_NAME}")

# Check that Security framework is found
if(NOT SECURITY_FRAMEWORK)
  message(FATAL_ERROR "SECURITY_FRAMEWORK should be set when WITH_MACOS_KEYCHAIN=ON, but it is: ${SECURITY_FRAMEWORK}")
endif()
message(STATUS "✓ Security framework found: ${SECURITY_FRAMEWORK}")

# Verify Security framework path is valid
if(NOT EXISTS "${SECURITY_FRAMEWORK}")
  message(FATAL_ERROR "Security framework path does not exist: ${SECURITY_FRAMEWORK}")
endif()
message(STATUS "✓ Security framework path is valid")

# Check that Cocoa framework is found
if(NOT COCOA_FRAMEWORK)
  message(FATAL_ERROR "COCOA_FRAMEWORK should be set on macOS, but it is: ${COCOA_FRAMEWORK}")
endif()
message(STATUS "✓ Cocoa framework found: ${COCOA_FRAMEWORK}")

# Verify Cocoa framework path is valid
if(NOT EXISTS "${COCOA_FRAMEWORK}")
  message(FATAL_ERROR "Cocoa framework path does not exist: ${COCOA_FRAMEWORK}")
endif()
message(STATUS "✓ Cocoa framework path is valid")

message(STATUS "Test ${TEST_NAME} PASSED")
