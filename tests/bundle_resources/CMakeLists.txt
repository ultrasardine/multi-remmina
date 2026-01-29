# Bundle Resources Tests

if(APPLE)
	# Property-based tests for bundle resource resolution
	add_executable(test_bundle_properties
		test_bundle_properties.c
		${CMAKE_SOURCE_DIR}/src/remmina_bundle_macos.c
	)

	target_include_directories(test_bundle_properties PRIVATE
		${CMAKE_SOURCE_DIR}/src
		${CMAKE_SOURCE_DIR}/src/include
	)

	target_link_libraries(test_bundle_properties
		${GLIB_LIBRARIES}
		"-framework CoreFoundation"
	)

	# Icon resource loading property tests
	add_executable(test_icon_properties
		test_icon_properties.c
		${CMAKE_SOURCE_DIR}/src/remmina_bundle_macos.c
	)

	target_include_directories(test_icon_properties PRIVATE
		${CMAKE_SOURCE_DIR}/src
		${CMAKE_SOURCE_DIR}/src/include
		${GTK_INCLUDE_DIRS}
	)

	target_link_libraries(test_icon_properties
		${GLIB_LIBRARIES}
		${GTK_LIBRARIES}
		"-framework CoreFoundation"
	)

	# Unit tests for resource loading edge cases
	add_executable(test_bundle_unit
		test_bundle_unit.c
		${CMAKE_SOURCE_DIR}/src/remmina_bundle_macos.c
	)

	target_include_directories(test_bundle_unit PRIVATE
		${CMAKE_SOURCE_DIR}/src
		${CMAKE_SOURCE_DIR}/src/include
	)

	target_link_libraries(test_bundle_unit
		${GLIB_LIBRARIES}
		"-framework CoreFoundation"
	)

	# Add tests to CTest
	add_test(NAME BundleResourceProperties COMMAND test_bundle_properties)
	add_test(NAME IconResourceProperties COMMAND test_icon_properties)
	add_test(NAME BundleResourceUnit COMMAND test_bundle_unit)
endif()

if(WIN32)
	# Property-based tests for Windows self-contained resource resolution
	add_executable(test_windows_resource_properties
		test_windows_resource_properties.c
		${CMAKE_SOURCE_DIR}/src/remmina_paths_windows.c
	)

	target_include_directories(test_windows_resource_properties PRIVATE
		${CMAKE_SOURCE_DIR}/src
		${CMAKE_SOURCE_DIR}/src/include
	)

	target_link_libraries(test_windows_resource_properties
		${GLIB_LIBRARIES}
		${GIO_LIBRARIES}
		Advapi32
		Shell32
		Ole32
	)

	# Property-based tests for Windows icon resource loading
	add_executable(test_windows_icon_properties
		test_windows_icon_properties.c
		${CMAKE_SOURCE_DIR}/src/remmina_paths_windows.c
	)

	target_include_directories(test_windows_icon_properties PRIVATE
		${CMAKE_SOURCE_DIR}/src
		${CMAKE_SOURCE_DIR}/src/include
	)

	target_link_libraries(test_windows_icon_properties
		${GLIB_LIBRARIES}
		${GIO_LIBRARIES}
		Advapi32
		Shell32
		Ole32
	)

	# Add tests to CTest
	add_test(NAME WindowsResourceProperties COMMAND test_windows_resource_properties)
	add_test(NAME WindowsIconProperties COMMAND test_windows_icon_properties)
endif()
