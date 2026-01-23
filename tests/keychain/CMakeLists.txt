# Keychain property-based tests (macOS only)

if(APPLE)
	# Find required packages
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(GLIB REQUIRED glib-2.0)
	pkg_check_modules(GIO REQUIRED gio-2.0)

	# Property-based tests
	add_executable(test_keychain_properties
		test_keychain_properties.c
		../../src/remmina_keychain_macos.c
	)

	target_include_directories(test_keychain_properties PRIVATE
		${CMAKE_SOURCE_DIR}/src
		${GLIB_INCLUDE_DIRS}
		${GIO_INCLUDE_DIRS}
	)

	target_link_libraries(test_keychain_properties
		${GLIB_LIBRARIES}
		${GIO_LIBRARIES}
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
		${GLIB_INCLUDE_DIRS}
		${GIO_INCLUDE_DIRS}
	)

	target_link_libraries(test_keychain_unit
		${GLIB_LIBRARIES}
		${GIO_LIBRARIES}
		"-framework Security"
		"-framework CoreFoundation"
	)

	# Platform abstraction tests
	add_executable(test_platform_abstraction
		test_platform_abstraction.c
		../../src/remmina_keychain_macos.c
	)

	target_include_directories(test_platform_abstraction PRIVATE
		${CMAKE_SOURCE_DIR}/src
		${GLIB_INCLUDE_DIRS}
		${GIO_INCLUDE_DIRS}
	)

	target_link_libraries(test_platform_abstraction
		${GLIB_LIBRARIES}
		${GIO_LIBRARIES}
		"-framework Security"
		"-framework CoreFoundation"
	)

	# Add tests to CTest
	add_test(NAME KeychainProperties COMMAND test_keychain_properties)
	add_test(NAME KeychainUnit COMMAND test_keychain_unit)
	add_test(NAME PlatformAbstraction COMMAND test_platform_abstraction)
endif()
