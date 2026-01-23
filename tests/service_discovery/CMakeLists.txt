# Service Discovery Tests for macOS

if(APPLE)
	# Find required packages
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(GLIB REQUIRED glib-2.0)

	# Property-based tests
	add_executable(test_service_discovery_properties
		test_service_discovery_properties.c
		../../src/remmina_bonjour_macos.c
	)

	target_include_directories(test_service_discovery_properties PRIVATE
		${GLIB_INCLUDE_DIRS}
		${CMAKE_SOURCE_DIR}/src
	)

	target_link_libraries(test_service_discovery_properties
		${GLIB_LIBRARIES}
	)

	# Add test
	add_test(NAME ServiceDiscoveryProperties
		COMMAND test_service_discovery_properties
	)

	# Unit tests
	add_executable(test_service_discovery_unit
		test_service_discovery_unit.c
		../../src/remmina_bonjour_macos.c
	)

	target_include_directories(test_service_discovery_unit PRIVATE
		${GLIB_INCLUDE_DIRS}
		${CMAKE_SOURCE_DIR}/src
	)

	target_link_libraries(test_service_discovery_unit
		${GLIB_LIBRARIES}
	)

	# Add test
	add_test(NAME ServiceDiscoveryUnit
		COMMAND test_service_discovery_unit
	)

	# Avahi wrapper integration tests
	add_executable(test_avahi_wrapper
		test_avahi_wrapper.c
		../../src/remmina_avahi.c
		../../src/remmina_bonjour_macos.c
	)

	target_include_directories(test_avahi_wrapper PRIVATE
		${GLIB_INCLUDE_DIRS}
		${CMAKE_SOURCE_DIR}/src
		${CMAKE_SOURCE_DIR}/src/include
		${CMAKE_BINARY_DIR}/generated
	)

	target_link_libraries(test_avahi_wrapper
		${GLIB_LIBRARIES}
	)

	# Add test
	add_test(NAME AvahiWrapper
		COMMAND test_avahi_wrapper
	)

	message(STATUS "Service discovery tests configured for macOS")
else()
	message(STATUS "Service discovery tests are only available on macOS")
endif()
