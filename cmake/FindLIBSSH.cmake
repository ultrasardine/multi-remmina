# Remmina - The GTK+ Remote Desktop Client
#
# Copyright (C) 2011 Marc-Andre Moreau
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

include(LibFindMacros)

# Dependencies
find_package(PkgConfig)

# Add MSYS2 paths for Windows
if(WIN32)
  set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/mingw64/lib/pkgconfig:/ucrt64/lib/pkgconfig")
endif()

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(PC_LIBSSH libssh>=0.6)


set(LIBSSH_DEFINITIONS ${PC_LIBSSH_CFLAGS_OTHER})

# Additional search paths for Windows MSYS2
set(_LIBSSH_SEARCH_PATHS "")
if(WIN32)
  list(APPEND _LIBSSH_SEARCH_PATHS
    /mingw64/include
    /ucrt64/include
    /mingw64/lib
    /ucrt64/lib
  )
endif()

# Include dir
find_path(LIBSSH_INCLUDE_DIR
	NAMES libssh/libssh.h
	#HINTS ${PC_LIBSSH_INCLUDEDIR} ${PC_LIBSSH_INCLUDE_DIRS}
	PATHS ${PC_LIBSSH_PKGCONF_INCLUDE_DIRS} ${_LIBSSH_SEARCH_PATHS}
)

# The library itself
find_library(LIBSSH_LIBRARY
	NAMES ssh
	#HINTS ${PC_LIBSSH_LIBDIR} ${PC_LIBSSH_LIBRARY_DIRS}
	PATHS ${PC_LIBSSH_PKGCONF_LIBRARY_DIRS} ${_LIBSSH_SEARCH_PATHS}
)

find_library(LIBSSH_THREADS_LIBRARY
	NAMES ssh_threads
	PATHS ${PC_LIBSSH_LIBDIR} ${PC_LIBSSH_LIBRARY_DIRS} ${_LIBSSH_SEARCH_PATHS}
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(LIBSSH DEFAULT_MSG LIBSSH_LIBRARY LIBSSH_INCLUDE_DIR)

if (LIBSSH_THREADS_LIBRARY)
	set(LIBSSH_LIBRARIES ${LIBSSH_LIBRARY} ${LIBSSH_THREADS_LIBRARY})
else()
	set(LIBSSH_LIBRARIES ${LIBSSH_LIBRARY})
endif()
set(LIBSSH_INCLUDE_DIRS ${LIBSSH_INCLUDE_DIR})

mark_as_advanced(LIBSSH_INCLUDE_DIR LIBSSH_LIBRARY)

