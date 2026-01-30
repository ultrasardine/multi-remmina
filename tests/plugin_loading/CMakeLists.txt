# Plugin Loading Tests

# Use GLib from parent scope (already found in main CMakeLists.txt)
# GLib_INCLUDE_DIRS and GLib_LIBRARIES should be available

# Property-based test for plugin protocol registration
add_executable(test_plugin_properties
	test_plugin_properties.c
)

target_include_directories(test_plugin_properties PRIVATE
	${GLib_INCLUDE_DIRS}
	${GMODULE_INCLUDE_DIR}
)

target_link_libraries(test_plugin_properties
	${GLib_LIBRARY}
	${GMODULE_LIBRARY}
)

# Unit tests for plugin loading edge cases
add_executable(test_plugin_unit
	test_plugin_unit.c
)

target_include_directories(test_plugin_unit PRIVATE
	${GLib_INCLUDE_DIRS}
	${GMODULE_INCLUDE_DIR}
)

target_link_libraries(test_plugin_unit
	${GLib_LIBRARY}
	${GMODULE_LIBRARY}
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

