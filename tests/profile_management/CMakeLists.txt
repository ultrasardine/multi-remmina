# Profile Management Tests

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(GIO REQUIRED gio-2.0)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(JSONGLIB REQUIRED json-glib-1.0)

# Unit tests for profile creation
add_executable(test_profile_unit
	test_profile_unit.c
	${CMAKE_SOURCE_DIR}/src/remmina_file.c
	${CMAKE_SOURCE_DIR}/src/remmina_file_manager.c
	${CMAKE_SOURCE_DIR}/src/remmina_crypt.c
	${CMAKE_SOURCE_DIR}/src/remmina_sodium.c
	${CMAKE_SOURCE_DIR}/src/remmina_log.c
	${CMAKE_SOURCE_DIR}/src/remmina_pref.c
	${CMAKE_SOURCE_DIR}/src/remmina_utils.c
	${CMAKE_SOURCE_DIR}/src/remmina_string_array.c
	${CMAKE_SOURCE_DIR}/src/remmina_masterthread_exec.c
)

target_include_directories(test_profile_unit PRIVATE
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/src/include
	${CMAKE_BINARY_DIR}
	${CMAKE_BINARY_DIR}/generated
	${GLIB_INCLUDE_DIRS}
	${GIO_INCLUDE_DIRS}
	${GTK3_INCLUDE_DIRS}
	${JSONGLIB_INCLUDE_DIRS}
)

target_link_libraries(test_profile_unit
	${GLIB_LIBRARIES}
	${GIO_LIBRARIES}
	${GTK3_LIBRARIES}
	${JSONGLIB_LIBRARIES}
	${sodium_LIBRARY_RELEASE}
)

if(GCRYPT_FOUND)
	target_link_libraries(test_profile_unit ${GCRYPT_LIBRARIES})
endif()

if(APPLE)
	target_link_libraries(test_profile_unit
		"-framework Security"
		"-framework CoreFoundation"
	)
endif()

add_test(NAME ProfileUnit COMMAND test_profile_unit)

# Property-based tests for profile persistence
add_executable(test_profile_properties
	test_profile_properties.c
	${CMAKE_SOURCE_DIR}/src/remmina_file.c
	${CMAKE_SOURCE_DIR}/src/remmina_file_manager.c
	${CMAKE_SOURCE_DIR}/src/remmina_crypt.c
	${CMAKE_SOURCE_DIR}/src/remmina_sodium.c
	${CMAKE_SOURCE_DIR}/src/remmina_log.c
	${CMAKE_SOURCE_DIR}/src/remmina_pref.c
	${CMAKE_SOURCE_DIR}/src/remmina_utils.c
	${CMAKE_SOURCE_DIR}/src/remmina_string_array.c
	${CMAKE_SOURCE_DIR}/src/remmina_masterthread_exec.c
)

target_include_directories(test_profile_properties PRIVATE
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/src/include
	${CMAKE_BINARY_DIR}
	${CMAKE_BINARY_DIR}/generated
	${GLIB_INCLUDE_DIRS}
	${GIO_INCLUDE_DIRS}
	${GTK3_INCLUDE_DIRS}
	${JSONGLIB_INCLUDE_DIRS}
)

target_link_libraries(test_profile_properties
	${GLIB_LIBRARIES}
	${GIO_LIBRARIES}
	${GTK3_LIBRARIES}
	${JSONGLIB_LIBRARIES}
	${sodium_LIBRARY_RELEASE}
)

if(GCRYPT_FOUND)
	target_link_libraries(test_profile_properties ${GCRYPT_LIBRARIES})
endif()

if(APPLE)
	target_link_libraries(test_profile_properties
		"-framework Security"
		"-framework CoreFoundation"
	)
	
	# Link Keychain implementation on macOS
	target_sources(test_profile_properties PRIVATE
		${CMAKE_SOURCE_DIR}/src/remmina_keychain_macos.c
	)
endif()

add_test(NAME ProfileProperties COMMAND test_profile_properties)
