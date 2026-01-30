# System Tray Integration Tests
# Tests for StatusIcon (macOS) and AppIndicator (Linux) system tray functionality

cmake_minimum_required(VERSION 3.10)

# Use GTK from parent scope (already found in main CMakeLists.txt)

# Unit tests for system tray integration
add_executable(test_system_tray_unit
	test_system_tray_unit.c
)

target_include_directories(test_system_tray_unit PRIVATE
	${GTK3_INCLUDE_DIRS}
	${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(test_system_tray_unit
	${GTK3_LIBRARIES}
)

# Add test to CTest
add_test(NAME SystemTrayUnit COMMAND test_system_tray_unit)

# Set test properties
set_tests_properties(SystemTrayUnit PROPERTIES
	TIMEOUT 30
	LABELS "unit;system_tray;macos"
)

message(STATUS "System tray tests configured")
