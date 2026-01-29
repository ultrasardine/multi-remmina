# Credential Manager property-based tests (Windows only)

if(WIN32)
	# Find required packages
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(GLIB REQUIRED glib-2.0)
	pkg_check_modules(GIO REQUIRED gio-2.0)

	# Property-based tests
	add_executable(test_credential_properties
		test_credential_properties.c
		../../src/remmina_cred_windows.c
	)

	target_include_directories(test_credential_properties PRIVATE
		${CMAKE_SOURCE_DIR}/src
		${GLIB_INCLUDE_DIRS}
		${GIO_INCLUDE_DIRS}
	)

	target_link_libraries(test_credential_properties
		${GLIB_LIBRARIES}
		${GIO_LIBRARIES}
		advapi32
	)

	# Unit tests
	add_executable(test_credential_unit
		test_credential_unit.c
		../../src/remmina_cred_windows.c
	)

	target_include_directories(test_credential_unit PRIVATE
		${CMAKE_SOURCE_DIR}/src
		${GLIB_INCLUDE_DIRS}
		${GIO_INCLUDE_DIRS}
	)

	target_link_libraries(test_credential_unit
		${GLIB_LIBRARIES}
		${GIO_LIBRARIES}
		advapi32
	)

	# Add tests to CTest
	add_test(NAME CredentialManagerProperties COMMAND test_credential_properties)
	add_test(NAME CredentialManagerUnit COMMAND test_credential_unit)
endif()
