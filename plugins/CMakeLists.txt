# Remmina - The GTK+ Remote Desktop Client
#
# Copyright (C) 2011 Marc-Andre Moreau
# Copyright (C) 2014-2015 Antenore Gatta, Fabio Castelli, Giovanni Panozzo
# Copyright (C) 2016-2023 Antenore Gatta, Giovanni Panozzo
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA  02110-1301, USA.
#
# In addition, as a special exception, the copyright holders give
# permission to link the code of portions of this program with the
# OpenSSL library under certain conditions as described in each
# individual source file, and distribute linked combinations
# including the two.
# You must obey the GNU General Public License in all respects
# for all of the code used other than OpenSSL. If you modify
# file(s) with this exception, you may extend this exception to your
# version of the file(s), but you are not obligated to do so. If you
# do not wish to do so, delete this exception statement from your
# version. If you delete this exception statement from all source
# files in the program, then also delete it here.

# Needed FreeRDP version to build freerdp plugin

# macOS-specific plugin configuration
if(APPLE)
  message(STATUS "Configuring plugins for macOS")
  
  # Add Homebrew search paths for plugin dependencies
  # Support both Intel (/usr/local) and Apple Silicon (/opt/homebrew) installations
  list(APPEND CMAKE_PREFIX_PATH
    /usr/local
    /opt/homebrew
    /usr/local/opt/freerdp
    /opt/homebrew/opt/freerdp
    /usr/local/opt/libvncserver
    /opt/homebrew/opt/libvncserver
    /usr/local/opt/libssh
    /opt/homebrew/opt/libssh
  )
  
  # Add Homebrew include paths for plugin dependencies
  include_directories(SYSTEM
    /usr/local/include
    /opt/homebrew/include
    /usr/local/opt/freerdp/include
    /opt/homebrew/opt/freerdp/include
    /usr/local/opt/libvncserver/include
    /opt/homebrew/opt/libvncserver/include
    /usr/local/opt/libssh/include
    /opt/homebrew/opt/libssh/include
  )
  
  # Add Homebrew library paths for plugin dependencies
  link_directories(
    /usr/local/lib
    /opt/homebrew/lib
    /usr/local/opt/freerdp/lib
    /opt/homebrew/opt/freerdp/lib
    /usr/local/opt/libvncserver/lib
    /opt/homebrew/opt/libvncserver/lib
    /usr/local/opt/libssh/lib
    /opt/homebrew/opt/libssh/lib
  )
  
  # Set plugin RPATH for bundle loading
  # Plugins need to find libraries relative to the bundle
  set(CMAKE_INSTALL_RPATH "@executable_path/../Frameworks;@executable_path/../lib;@loader_path")
  set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
  
  message(STATUS "macOS plugin configuration complete:")
  message(STATUS "  - Plugin RPATH: ${CMAKE_INSTALL_RPATH}")
  message(STATUS "  - Homebrew paths added for FreeRDP, libvncserver, libssh")
endif()

# Windows-specific plugin configuration (MSYS2/MinGW)
if(WIN32)
  message(STATUS "Configuring plugins for Windows")
  
  # Add MSYS2/MinGW search paths for plugin dependencies
  # Support both mingw64 and ucrt64 environments
  list(APPEND CMAKE_PREFIX_PATH
    /mingw64
    /ucrt64
    /mingw64/lib/cmake
    /ucrt64/lib/cmake
    # FreeRDP CMake config paths
    /mingw64/lib/cmake/FreeRDP2
    /ucrt64/lib/cmake/FreeRDP2
    /mingw64/lib/cmake/FreeRDP3
    /ucrt64/lib/cmake/FreeRDP3
    /mingw64/lib/cmake/FreeRDP-Client2
    /ucrt64/lib/cmake/FreeRDP-Client2
    /mingw64/lib/cmake/FreeRDP-Client3
    /ucrt64/lib/cmake/FreeRDP-Client3
    # WinPR CMake config paths
    /mingw64/lib/cmake/WinPR2
    /ucrt64/lib/cmake/WinPR2
    /mingw64/lib/cmake/WinPR3
    /ucrt64/lib/cmake/WinPR3
    # libssh CMake config paths
    /mingw64/lib/cmake/libssh
    /ucrt64/lib/cmake/libssh
  )
  
  # Add MSYS2 include paths for plugin dependencies
  include_directories(SYSTEM
    /mingw64/include
    /ucrt64/include
    # FreeRDP include paths
    /mingw64/include/freerdp2
    /ucrt64/include/freerdp2
    /mingw64/include/freerdp3
    /ucrt64/include/freerdp3
    # WinPR include paths
    /mingw64/include/winpr2
    /ucrt64/include/winpr2
    /mingw64/include/winpr3
    /ucrt64/include/winpr3
    # libvncserver include paths
    /mingw64/include/rfb
    /ucrt64/include/rfb
    # libssh include paths
    /mingw64/include/libssh
    /ucrt64/include/libssh
  )
  
  # Add MSYS2 library paths for plugin dependencies
  link_directories(
    /mingw64/lib
    /ucrt64/lib
    /mingw64/bin
    /ucrt64/bin
  )
  
  # Set PKG_CONFIG_PATH for MSYS2 packages
  set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/mingw64/lib/pkgconfig:/ucrt64/lib/pkgconfig")
  
  # Windows DLL naming convention
  # Plugins will be built as .dll files instead of .so
  set(CMAKE_SHARED_LIBRARY_SUFFIX ".dll")
  set(CMAKE_SHARED_MODULE_SUFFIX ".dll")
  
  # Windows plugins don't use RPATH - DLLs are found in executable directory
  set(CMAKE_SKIP_RPATH TRUE)
  
  # Export all symbols for Windows DLLs (needed for plugin loading)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
  
  # Define a helper function to configure Windows plugin targets
  # This ensures consistent DLL output configuration across all plugins
  function(configure_windows_plugin TARGET_NAME)
    if(WIN32)
      # Set output name without lib prefix (Windows convention)
      set_target_properties(${TARGET_NAME} PROPERTIES PREFIX "")
      # Ensure .dll suffix
      set_target_properties(${TARGET_NAME} PROPERTIES SUFFIX ".dll")
      # No SONAME on Windows
      set_target_properties(${TARGET_NAME} PROPERTIES NO_SONAME 1)
      # Export all symbols
      set_target_properties(${TARGET_NAME} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
      # Set runtime output directory to match install location
      set_target_properties(${TARGET_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
      )
    endif()
  endfunction()
  
  message(STATUS "Windows plugin configuration complete:")
  message(STATUS "  - MSYS2 paths: /mingw64, /ucrt64")
  message(STATUS "  - Plugin extension: .dll")
  message(STATUS "  - FreeRDP paths configured")
  message(STATUS "  - libvncserver paths configured")
  message(STATUS "  - libssh paths configured")
endif()

include_directories(${CMAKE_SOURCE_DIR}/plugins)

set(APPICONSCALE_EMBLEMS_DIR "${REMMINA_DATADIR}/icons/hicolor/scalable/emblems")

set(REMMINA_COMMON_INCLUDE_DIRS ${GTK3_INCLUDE_DIRS})
set(REMMINA_COMMON_LIBRARIES ${GTK_LIBRARIES} ${Intl_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

find_suggested_package(LIBSSH)
if(LIBSSH_FOUND)
    add_definitions(-DHAVE_LIBSSH)
    include_directories(SYSTEM ${SSH_INCLUDE_DIRS})
    target_link_libraries(remmina ${SSH_LIBRARIES})
endif()

option(WITH_FREERDP3 "Compile the RDP plugin using symbols from FreeRDP3" OFF)
if (WITH_FREERDP3)
    message(STATUS "Using FreeRDP3")
    set(FREERDP_REQUIRED_VERSION 3)
    set(FREERDP_REQUIRED_MAJOR 3)
    set(FREERDP_REQUIRED_MINOR 0)
    set(FREERDP_REQUIRED_REVISION 0)
else()
    message(STATUS "Using FreeRDP2")
    set(FREERDP_REQUIRED_VERSION 2)
    set(FREERDP_REQUIRED_MAJOR 2)
    set(FREERDP_REQUIRED_MINOR 0)
    set(FREERDP_REQUIRED_REVISION 0)
endif()

find_package(WinPR ${FREERDP_REQUIRED_VERSION} )
find_package(FreeRDP ${FREERDP_REQUIRED_VERSION} )
find_package(FreeRDP-Client ${FREERDP_REQUIRED_VERSION} )


if (WinPR_FOUND AND FreeRDP_FOUND AND FreeRDP-Client_FOUND)
    message(STATUS "Enabling RDP plugin")
    add_subdirectory(rdp)
else()
    message(STATUS "Disabling RDP plugin.")
endif()

# X2Go
option(WITH_X2GO "Build X2Go plugin" OFF)
if(WITH_X2GO)
    message(STATUS "Enabling X2Go plugin.")
    message(AUTHOR_WARNING "pyhoca-cli is needed at runtime, be sure to install it")
    add_subdirectory(x2go)
else()
    message(STATUS "Disabling X2Go plugin.")
endif()

find_suggested_package(LIBVNCSERVER)
if(LIBVNCSERVER_FOUND)
    add_subdirectory(vnc)
endif()

option(WITH_GVNC "Build GVNC plugin" OFF)
if(WITH_GVNC)
    message(STATUS "Enabling GVNC plugin.")
    add_definitions(-DWITH_GVNC)
    find_suggested_package(GTK-VNC)
    if(GTK-VNC_FOUND)
        add_subdirectory(gvnc)
    endif()
else()
    message(STATUS "Disabling GVNC plugin.")
endif()

find_suggested_package(Spice)
if(SPICE_FOUND)
    add_subdirectory(spice)
endif()

option(WITH_WWW "Build WWW plugin" ON)
if(WITH_WWW)
    message(STATUS "Enabling WWW plugin.")
    add_definitions(-DWITH_WWW)
    find_suggested_package(WEBKIT2GTK)
    if(WEBKIT2GTK_FOUND)
        add_subdirectory(www)
    endif()
else()
    message(STATUS "Disabling WWW plugin.")
endif()

if(WITH_EXAMPLES)
    message(STATUS "Enabling examples and test plugins.")
    add_subdirectory(tool_hello_world)
endif()

add_subdirectory(exec)

option(WITH_KF5WALLET "Building KF5WALLET plugin" OFF)
if(WITH_KF5WALLET)
    CheckHasModule(KF5Wallet)
    if(HAS_MODULE_KF5Wallet)
        message(STATUS "Enabling KDE Wallet plugin.")
        add_definitions(-DWITH_KF5WALLET)
        add_subdirectory(kwallet)
    else()
        message(FATAL_ERROR "libKF5wallet not found but requested")
    endif()
endif()

option(WITH_ST "Build Simple Terminal plugin" OFF)
if(WITH_ST)
    message(STATUS "Enabling ST plugin.")
    add_definitions(-DWITH_ST)
    get_filename_component(_st_fullpath "st" REALPATH)
    if(EXISTS "${_st_fullpath}" AND EXISTS "${_st_fullpath}/CMakeLists.txt")
        add_subdirectory(st)
    else()
        message(WARNING "Can't find ${_st_fullpath}/CMakeLists.txt")
    endif()
endif()

option(WITH_XDMCP "Build XDMCP plugin" OFF)
if(WITH_XDMCP)
    add_definitions(-DWITH_XDMCP)
    message(STATUS "Enabling XDMCP plugin.")
    get_filename_component(_xdmcp_fullpath "xdmcp" REALPATH)
    if(EXISTS "${_xdmcp_fullpath}" AND EXISTS "${_xdmcp_fullpath}/CMakeLists.txt")
        add_subdirectory(xdmcp)
    else()
        message(WARNING "Can't find ${_xdmcp_fullpath}/CMakeLists.txt")
    endif()
endif()

option(WITH_NX "Build NX plugin" OFF)
if(WITH_NX)
    add_definitions(-DWITH_NX)
    # After the plugin split only the NX plugin needs XKBfile.h
    find_required_package(XKBFILE)
    if(LIBSSH_FOUND AND XKBFILE_FOUND)
        message(STATUS "Enabling NX plugin.")
        get_filename_component(_nx_fullpath "nx" REALPATH)
        if(EXISTS "${_nx_fullpath}" AND EXISTS "${_nx_fullpath}/CMakeLists.txt")
            add_subdirectory(nx)
        else()
            message(WARNING "Can't find ${_nx_fullpath}/CMakeLists.txt")
        endif()
    endif()
endif()

option(WITH_PYTHONLIBS "Build Python Wrapper Plugin" ON)
if(WITH_PYTHONLIBS)
    message(STATUS "Enabling Python Wrapper Plugin.")
    add_definitions(-DWITH_PYTHONLIBS)
    find_suggested_package(PythonLibs)
    if(PythonLibs_FOUND)
        add_subdirectory(python_wrapper)
    endif()
else()
    message(STATUS "Disabling Python Wrapper plugin.")
endif()

#find_suggested_package(TELEPATHY)
#if(TELEPATHY_FOUND)
#add_subdirectory(telepathy)
#endif()
