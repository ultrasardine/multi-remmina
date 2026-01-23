# Plugin Loading Tests

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(GMODULE REQUIRED gmodule-2.0)

# Property-based test for plugin protocol registration
add_executable(test_plugin_properties
	test_plugin_properties.c
)

target_include_directories(test_plugin_properties PRIVATE
	${GLIB_INCLUDE_DIRS}
	${GMODULE_INCLUDE_DIRS}
)

target_link_libraries(test_plugin_properties
	${GLIB_LIBRARIES}
	${GMODULE_LIBRARIES}
)

# Unit tests for plugin loading edge cases
add_executable(test_plugin_unit
	test_plugin_unit.c
)

target_include_directories(test_plugin_unit PRIVATE
	${GLIB_INCLUDE_DIRS}
	${GMODULE_INCLUDE_DIRS}
)

target_link_libraries(test_plugin_unit
	${GLIB_LIBRARIES}
	${GMODULE_LIBRARIES}
)

# Add tests to CTest
add_test(NAME plugin_properties COMMAND test_plugin_properties)
add_test(NAME plugin_unit COMMAND test_plugin_unit)

# Set test properties
set_tests_properties(plugin_properties PROPERTIES
	TIMEOUT 300
	LABELS "property;plugin"
)

set_tests_properties(plugin_unit PROPERTIES
	TIMEOUT 60
	LABELS "unit;plugin"
)

message(STATUS "Plugin loading tests configured")

