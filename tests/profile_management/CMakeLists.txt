# Profile Management Tests

# Use GLib/GTK from parent scope (already found in main CMakeLists.txt)

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
	${GLib_INCLUDE_DIRS}
	${GIO_INCLUDE_DIRS}
	${GTK3_INCLUDE_DIRS}
	${JSONGLIB_INCLUDE_DIRS}
)

target_link_libraries(test_profile_unit
	${GLib_LIBRARY}
	${GIO_LIBRARY}
	${GTK3_LIBRARIES}
	${JSONGLIB_LIBRARY}
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
	${GLib_INCLUDE_DIRS}
	${GIO_INCLUDE_DIRS}
	${GTK3_INCLUDE_DIRS}
	${JSONGLIB_INCLUDE_DIRS}
)

target_link_libraries(test_profile_properties
	${GLib_LIBRARY}
	${GIO_LIBRARY}
	${GTK3_LIBRARIES}
	${JSONGLIB_LIBRARY}
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

# Windows-specific tests
if(WIN32)
	# Windows profile creation test
	add_executable(test_windows_profile_creation
		test_windows_profile_creation.c
		${CMAKE_SOURCE_DIR}/src/remmina_file.c
		${CMAKE_SOURCE_DIR}/src/remmina_file_manager.c
		${CMAKE_SOURCE_DIR}/src/remmina_crypt.c
		${CMAKE_SOURCE_DIR}/src/remmina_sodium.c
		${CMAKE_SOURCE_DIR}/src/remmina_log.c
		${CMAKE_SOURCE_DIR}/src/remmina_pref.c
		${CMAKE_SOURCE_DIR}/src/remmina_utils.c
		${CMAKE_SOURCE_DIR}/src/remmina_string_array.c
		${CMAKE_SOURCE_DIR}/src/remmina_masterthread_exec.c
		${CMAKE_SOURCE_DIR}/src/remmina_paths_windows.c
		${CMAKE_SOURCE_DIR}/src/remmina_cred_windows.c
	)
	
	target_include_directories(test_windows_profile_creation PRIVATE
		${CMAKE_SOURCE_DIR}/src
		${CMAKE_SOURCE_DIR}/src/include
		${CMAKE_BINARY_DIR}
		${CMAKE_BINARY_DIR}/generated
		${GLib_INCLUDE_DIRS}
		${GIO_INCLUDE_DIRS}
		${GTK3_INCLUDE_DIRS}
		${JSONGLIB_INCLUDE_DIRS}
	)
	
	target_link_libraries(test_windows_profile_creation
		${GLib_LIBRARY}
		${GIO_LIBRARY}
		${GTK3_LIBRARIES}
		${JSONGLIB_LIBRARY}
		${sodium_LIBRARY_RELEASE}
		Advapi32
		Shell32
		Ole32
	)
	
	if(GCRYPT_FOUND)
		target_link_libraries(test_windows_profile_creation ${GCRYPT_LIBRARIES})
	endif()
	
	add_test(NAME WindowsProfileCreation COMMAND test_windows_profile_creation)
	
	# Windows profile persistence test
	add_executable(test_windows_profile_persistence
		test_windows_profile_persistence.c
		${CMAKE_SOURCE_DIR}/src/remmina_file.c
		${CMAKE_SOURCE_DIR}/src/remmina_file_manager.c
		${CMAKE_SOURCE_DIR}/src/remmina_crypt.c
		${CMAKE_SOURCE_DIR}/src/remmina_sodium.c
		${CMAKE_SOURCE_DIR}/src/remmina_log.c
		${CMAKE_SOURCE_DIR}/src/remmina_pref.c
		${CMAKE_SOURCE_DIR}/src/remmina_utils.c
		${CMAKE_SOURCE_DIR}/src/remmina_string_array.c
		${CMAKE_SOURCE_DIR}/src/remmina_masterthread_exec.c
		${CMAKE_SOURCE_DIR}/src/remmina_paths_windows.c
		${CMAKE_SOURCE_DIR}/src/remmina_cred_windows.c
	)
	
	target_include_directories(test_windows_profile_persistence PRIVATE
		${CMAKE_SOURCE_DIR}/src
		${CMAKE_SOURCE_DIR}/src/include
		${CMAKE_BINARY_DIR}
		${CMAKE_BINARY_DIR}/generated
		${GLib_INCLUDE_DIRS}
		${GIO_INCLUDE_DIRS}
		${GTK3_INCLUDE_DIRS}
		${JSONGLIB_INCLUDE_DIRS}
	)
	
	target_link_libraries(test_windows_profile_persistence
		${GLib_LIBRARY}
		${GIO_LIBRARY}
		${GTK3_LIBRARIES}
		${JSONGLIB_LIBRARY}
		${sodium_LIBRARY_RELEASE}
		Advapi32
		Shell32
		Ole32
	)
	
	if(GCRYPT_FOUND)
		target_link_libraries(test_windows_profile_persistence ${GCRYPT_LIBRARIES})
	endif()
	
	add_test(NAME WindowsProfilePersistence COMMAND test_windows_profile_persistence)
	
	# Windows profile property-based tests
	add_executable(test_windows_profile_properties
		test_windows_profile_properties.c
		${CMAKE_SOURCE_DIR}/src/remmina_file.c
		${CMAKE_SOURCE_DIR}/src/remmina_file_manager.c
		${CMAKE_SOURCE_DIR}/src/remmina_crypt.c
		${CMAKE_SOURCE_DIR}/src/remmina_sodium.c
		${CMAKE_SOURCE_DIR}/src/remmina_log.c
		${CMAKE_SOURCE_DIR}/src/remmina_pref.c
		${CMAKE_SOURCE_DIR}/src/remmina_utils.c
		${CMAKE_SOURCE_DIR}/src/remmina_string_array.c
		${CMAKE_SOURCE_DIR}/src/remmina_masterthread_exec.c
		${CMAKE_SOURCE_DIR}/src/remmina_paths_windows.c
		${CMAKE_SOURCE_DIR}/src/remmina_cred_windows.c
	)
	
	target_include_directories(test_windows_profile_properties PRIVATE
		${CMAKE_SOURCE_DIR}/src
		${CMAKE_SOURCE_DIR}/src/include
		${CMAKE_BINARY_DIR}
		${CMAKE_BINARY_DIR}/generated
		${GLib_INCLUDE_DIRS}
		${GIO_INCLUDE_DIRS}
		${GTK3_INCLUDE_DIRS}
		${JSONGLIB_INCLUDE_DIRS}
	)
	
	target_link_libraries(test_windows_profile_properties
		${GLib_LIBRARY}
		${GIO_LIBRARY}
		${GTK3_LIBRARIES}
		${JSONGLIB_LIBRARY}
		${sodium_LIBRARY_RELEASE}
		Advapi32
		Shell32
		Ole32
	)
	
	if(GCRYPT_FOUND)
		target_link_libraries(test_windows_profile_properties ${GCRYPT_LIBRARIES})
	endif()
	
	add_test(NAME WindowsProfileProperties COMMAND test_windows_profile_properties)
endif()
