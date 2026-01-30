# Application launch unit tests

# Use GLib/GTK from parent scope (already found in main CMakeLists.txt)

# Unit tests for application launch
add_executable(test_application_launch_unit
	test_application_launch_unit.c
)

target_include_directories(test_application_launch_unit PRIVATE
	${CMAKE_SOURCE_DIR}/src
	${GTK3_INCLUDE_DIRS}
	${GLib_INCLUDE_DIRS}
	${GIO_INCLUDE_DIRS}
)

target_link_libraries(test_application_launch_unit
	${GTK3_LIBRARIES}
	${GLib_LIBRARY}
	${GIO_LIBRARY}
)

# Add platform-specific frameworks on macOS
if(APPLE)
	target_link_libraries(test_application_launch_unit
		"-framework Cocoa"
		"-framework CoreFoundation"
	)
endif()

# Add tests to CTest
add_test(NAME ApplicationLaunchUnit COMMAND test_application_launch_unit)
