# Test: Windows APIs linked when enabled
# Validates: Requirements 1.6
#
# This test verifies that Windows-specific APIs (Advapi32, dnsapi) are linked
# when Windows Credential Manager and DNS-SD support are enabled

message(STATUS "Running test: ${TEST_NAME}")

# Check that Advapi32 is linked when Credential Manager is enabled
if(WITH_WINDOWS_CREDENTIAL_MANAGER)
  if(NOT ADVAPI32_LIBRARY)
    message(FATAL_ERROR "ADVAPI32_LIBRARY should be set when WITH_WINDOWS_CREDENTIAL_MANAGER is ON")
  endif()
  message(STATUS "✓ Advapi32 library is correctly linked: ${ADVAPI32_LIBRARY}")
else()
  message(STATUS "Windows Credential Manager is disabled, skipping Advapi32 check")
endif()

# Check that dnsapi is linked when DNS-SD is enabled
if(WITH_WINDOWS_DNSSD)
  if(NOT DNSAPI_LIBRARY)
    message(FATAL_ERROR "DNSAPI_LIBRARY should be set when WITH_WINDOWS_DNSSD is ON")
  endif()
  message(STATUS "✓ dnsapi library is correctly linked: ${DNSAPI_LIBRARY}")
else()
  message(STATUS "Windows DNS-SD is disabled, skipping dnsapi check")
endif()

message(STATUS "Test ${TEST_NAME} PASSED")
