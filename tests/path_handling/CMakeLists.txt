# Path handling property-based and unit tests (Windows only)

if(WIN32)
	# Use GLib from parent scope (already found in main CMakeLists.txt)

	# Property-based tests
	add_executable(test_path_properties
		test_path_properties.c
		../../src/remmina_paths_windows.c
	)

	target_include_directories(test_path_properties PRIVATE
		${CMAKE_SOURCE_DIR}/src
		${GLib_INCLUDE_DIRS}
		${GIO_INCLUDE_DIRS}
	)

	target_link_libraries(test_path_properties
		${GLib_LIBRARY}
		${GIO_LIBRARY}
		shell32
		ole32
	)

	# Unit tests
	add_executable(test_path_unit
		test_path_unit.c
		../../src/remmina_paths_windows.c
	)

	target_include_directories(test_path_unit PRIVATE
		${CMAKE_SOURCE_DIR}/src
		${GLib_INCLUDE_DIRS}
		${GIO_INCLUDE_DIRS}
	)

	target_link_libraries(test_path_unit
		${GLib_LIBRARY}
		${GIO_LIBRARY}
		shell32
		ole32
	)

	# Add tests to CTest
	add_test(NAME PathHandlingProperties COMMAND test_path_properties)
	add_test(NAME PathHandlingUnit COMMAND test_path_unit)
endif()
