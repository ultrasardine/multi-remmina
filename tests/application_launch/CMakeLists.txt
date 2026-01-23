# Application launch unit tests

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(GIO REQUIRED gio-2.0)

# Unit tests for application launch
add_executable(test_application_launch_unit
	test_application_launch_unit.c
)

target_include_directories(test_application_launch_unit PRIVATE
	${CMAKE_SOURCE_DIR}/src
	${GTK3_INCLUDE_DIRS}
	${GLIB_INCLUDE_DIRS}
	${GIO_INCLUDE_DIRS}
)

target_link_libraries(test_application_launch_unit
	${GTK3_LIBRARIES}
	${GLIB_LIBRARIES}
	${GIO_LIBRARIES}
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
