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
