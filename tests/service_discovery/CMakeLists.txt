# Service Discovery Tests for macOS and Windows

if(APPLE OR WIN32)
	# Use GLib from parent scope (already found in main CMakeLists.txt)

	if(APPLE)
		# Property-based tests for macOS
		add_executable(test_service_discovery_properties
			test_service_discovery_properties.c
			../../src/remmina_bonjour_macos.c
		)

		target_include_directories(test_service_discovery_properties PRIVATE
			${GLib_INCLUDE_DIRS}
			${CMAKE_SOURCE_DIR}/src
		)

		target_link_libraries(test_service_discovery_properties
			${GLib_LIBRARY}
		)

		# Unit tests for macOS
		add_executable(test_service_discovery_unit
			test_service_discovery_unit.c
			../../src/remmina_bonjour_macos.c
		)

		target_include_directories(test_service_discovery_unit PRIVATE
			${GLib_INCLUDE_DIRS}
			${CMAKE_SOURCE_DIR}/src
		)

		target_link_libraries(test_service_discovery_unit
			${GLib_LIBRARY}
		)

		# Avahi wrapper integration tests for macOS
		add_executable(test_avahi_wrapper
			test_avahi_wrapper.c
			../../src/remmina_avahi.c
			../../src/remmina_bonjour_macos.c
		)

		target_include_directories(test_avahi_wrapper PRIVATE
			${GLib_INCLUDE_DIRS}
			${CMAKE_SOURCE_DIR}/src
			${CMAKE_SOURCE_DIR}/src/include
			${CMAKE_BINARY_DIR}/generated
		)

		target_link_libraries(test_avahi_wrapper
			${GLib_LIBRARY}
		)

		message(STATUS "Service discovery tests configured for macOS")
	elseif(WIN32)
		# Property-based tests for Windows
		add_executable(test_service_discovery_properties
			test_service_discovery_properties.c
			../../src/remmina_dnssd_windows.c
		)

		target_include_directories(test_service_discovery_properties PRIVATE
			${GLib_INCLUDE_DIRS}
			${CMAKE_SOURCE_DIR}/src
		)

		target_link_libraries(test_service_discovery_properties
			${GLib_LIBRARY}
			dnsapi
			ws2_32
		)

		# Unit tests for Windows
		add_executable(test_service_discovery_unit
			test_service_discovery_unit.c
			../../src/remmina_dnssd_windows.c
		)

		target_include_directories(test_service_discovery_unit PRIVATE
			${GLib_INCLUDE_DIRS}
			${CMAKE_SOURCE_DIR}/src
		)

		target_link_libraries(test_service_discovery_unit
			${GLib_LIBRARY}
			dnsapi
			ws2_32
		)

		# Avahi wrapper integration tests for Windows
		add_executable(test_avahi_wrapper
			test_avahi_wrapper.c
			../../src/remmina_avahi.c
			../../src/remmina_dnssd_windows.c
		)

		target_include_directories(test_avahi_wrapper PRIVATE
			${GLib_INCLUDE_DIRS}
			${CMAKE_SOURCE_DIR}/src
			${CMAKE_SOURCE_DIR}/src/include
			${CMAKE_BINARY_DIR}/generated
		)

		target_link_libraries(test_avahi_wrapper
			${GLib_LIBRARY}
			dnsapi
			ws2_32
		)

		message(STATUS "Service discovery tests configured for Windows")
	endif()

	# Add tests
	add_test(NAME ServiceDiscoveryProperties
		COMMAND test_service_discovery_properties
	)

	add_test(NAME ServiceDiscoveryUnit
		COMMAND test_service_discovery_unit
	)

	add_test(NAME AvahiWrapper
		COMMAND test_avahi_wrapper
	)
else()
	message(STATUS "Service discovery tests are only available on macOS and Windows")
endif()
