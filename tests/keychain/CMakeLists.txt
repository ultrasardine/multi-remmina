# Keychain property-based tests (macOS) and Credential Manager tests (Windows)

# Use GLib from parent scope (already found in main CMakeLists.txt)

if(APPLE)
	# Property-based tests
	add_executable(test_keychain_properties
		test_keychain_properties.c
		../../src/remmina_keychain_macos.c
	)

	target_include_directories(test_keychain_properties PRIVATE
		${CMAKE_SOURCE_DIR}/src
		${GLib_INCLUDE_DIRS}
		${GIO_INCLUDE_DIRS}
	)

	target_link_libraries(test_keychain_properties
		${GLib_LIBRARY}
		${GIO_LIBRARY}
		"-framework Security"
		"-framework CoreFoundation"
	)

	# Unit tests
	add_executable(test_keychain_unit
		test_keychain_unit.c
		../../src/remmina_keychain_macos.c
	)

	target_include_directories(test_keychain_unit PRIVATE
		${CMAKE_SOURCE_DIR}/src
		${GLib_INCLUDE_DIRS}
		${GIO_INCLUDE_DIRS}
	)

	target_link_libraries(test_keychain_unit
		${GLib_LIBRARY}
		${GIO_LIBRARY}
		"-framework Security"
		"-framework CoreFoundation"
	)

	# Platform abstraction tests (macOS)
	add_executable(test_platform_abstraction
		test_platform_abstraction.c
		../../src/remmina_keychain_macos.c
	)

	target_include_directories(test_platform_abstraction PRIVATE
		${CMAKE_SOURCE_DIR}/src
		${GLib_INCLUDE_DIRS}
		${GIO_INCLUDE_DIRS}
	)

	target_link_libraries(test_platform_abstraction
		${GLib_LIBRARY}
		${GIO_LIBRARY}
		"-framework Security"
		"-framework CoreFoundation"
	)

	# Add tests to CTest
	add_test(NAME KeychainProperties COMMAND test_keychain_properties)
	add_test(NAME KeychainUnit COMMAND test_keychain_unit)
	add_test(NAME PlatformAbstraction COMMAND test_platform_abstraction)
elseif(WIN32)
	# Platform abstraction tests (Windows)
	add_executable(test_platform_abstraction
		test_platform_abstraction.c
		../../src/remmina_cred_windows.c
	)

	target_include_directories(test_platform_abstraction PRIVATE
		${CMAKE_SOURCE_DIR}/src
		${GLib_INCLUDE_DIRS}
		${GIO_INCLUDE_DIRS}
	)

	target_link_libraries(test_platform_abstraction
		${GLib_LIBRARY}
		${GIO_LIBRARY}
		advapi32
	)

	# Add tests to CTest
	add_test(NAME PlatformAbstraction COMMAND test_platform_abstraction)
endif()
