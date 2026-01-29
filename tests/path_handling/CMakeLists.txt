# Path handling property-based and unit tests (Windows only)

if(WIN32)
	# Find required packages
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(GLIB REQUIRED glib-2.0)
	pkg_check_modules(GIO REQUIRED gio-2.0)

	# Property-based tests
	add_executable(test_path_properties
		test_path_properties.c
		../../src/remmina_paths_windows.c
	)

	target_include_directories(test_path_properties PRIVATE
		${CMAKE_SOURCE_DIR}/src
		${GLIB_INCLUDE_DIRS}
		${GIO_INCLUDE_DIRS}
	)

	target_link_libraries(test_path_properties
		${GLIB_LIBRARIES}
		${GIO_LIBRARIES}
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
		${GLIB_INCLUDE_DIRS}
		${GIO_INCLUDE_DIRS}
	)

	target_link_libraries(test_path_unit
		${GLIB_LIBRARIES}
		${GIO_LIBRARIES}
		shell32
		ole32
	)

	# Add tests to CTest
	add_test(NAME PathHandlingProperties COMMAND test_path_properties)
	add_test(NAME PathHandlingUnit COMMAND test_path_unit)
endif()
