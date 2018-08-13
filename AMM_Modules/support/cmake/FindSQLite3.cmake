# Copyright (C) 2007-2009 LuaDist.
# Created by Peter Kapec <kapecp@gmail.com>
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
#	Note:
#		Searching headers and libraries is very simple and is NOT as powerful as scripts
#		distributed with CMake, because LuaDist defines directories to search for.
#		Everyone is encouraged to contact the author with improvements. Maybe this file
#		becomes part of CMake distribution sometimes.

# - Find sqlite3
# Find the native SQLITE3 headers and libraries.
#
# SQLITE3_INCLUDE_DIRS	- where to find sqlite3.h, etc.
# SQLITE3_LIBRARIES	- List of libraries when using sqlite.
# SQLITE3_FOUND	- True if sqlite found.
if(NOT SQLITE3_FOUND)
# Look for the header file.
find_path(SQLITE3_INCLUDE_DIR NAMES sqlite3.h)

# Look for the library.
find_library(SQLITE3_LIBRARY_RELEASE NAMES sqlite sqlite3)
find_library(SQLITE3_LIBRARY_DEBUG   
    NAMES 
      sqlite_d sqlite3_d
      sqliteD sqlite3D
      sqlite sqlite3
 )

# Handle the QUIETLY and REQUIRED arguments and set SQLITE3_FOUND to TRUE if all listed variables are TRUE.
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SQLITE3 DEFAULT_MSG 
  SQLITE3_LIBRARY_RELEASE
  SQLITE3_LIBRARY_DEBUG
  SQLITE3_INCLUDE_DIR
)

# Copy the results to the output variables.
if(SQLITE3_FOUND)
    set(SQLITE3_LIBRARIES optimized ${SQLITE3_LIBRARY_RELEASE} debug ${SQLITE3_LIBRARY_DEBUG})
    set(SQLITE3_INCLUDE_DIRS ${SQLITE3_INCLUDE_DIR})
    add_library(SQLite3::SQLite3 SHARED  IMPORTED GLOBAL)
    set_target_properties(SQLite3::SQLite3
      PROPERTIES
      IMPORTED_LOCATION       ${SQLITE3_LIBRARY_RELEASE}   
      IMPORTED_LOCATION_DEBUG ${SQLITE3_LIBRARY_DEBUG}
      INTERFACE_INCLUDE_DIRECTORIES
      ${SQLITE3_INCLUDE_DIR}
    )
    
    mark_as_advanced(SQLITE3_INCLUDE_DIRS SQLITE3_LIBRARIES)
endif()


endif()
