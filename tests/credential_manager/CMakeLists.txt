# Credential Manager property-based tests (Windows only)

if(WIN32)
	# Use GLib from parent scope (already found in main CMakeLists.txt)

	# Property-based tests
	add_executable(test_credential_properties
		test_credential_properties.c
		../../src/remmina_cred_windows.c
	)

	target_include_directories(test_credential_properties PRIVATE
		${CMAKE_SOURCE_DIR}/src
		${GLib_INCLUDE_DIRS}
		${GIO_INCLUDE_DIRS}
	)

	target_link_libraries(test_credential_properties
		${GLib_LIBRARY}
		${GIO_LIBRARY}
		advapi32
	)

	# Unit tests
	add_executable(test_credential_unit
		test_credential_unit.c
		../../src/remmina_cred_windows.c
	)

	target_include_directories(test_credential_unit PRIVATE
		${CMAKE_SOURCE_DIR}/src
		${GLib_INCLUDE_DIRS}
		${GIO_INCLUDE_DIRS}
	)

	target_link_libraries(test_credential_unit
		${GLib_LIBRARY}
		${GIO_LIBRARY}
		advapi32
	)

	# Add tests to CTest
	add_test(NAME CredentialManagerProperties COMMAND test_credential_properties)
	add_test(NAME CredentialManagerUnit COMMAND test_credential_unit)
endif()
