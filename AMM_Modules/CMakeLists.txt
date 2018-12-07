cmake_minimum_required(VERSION 3.5.1)

project(AMM_Modules VERSION 0.2.2)
set(VERSION_TYPE "beta" CACHE STRING "version type" FORCE)
set(PROJECT_VERSION_MAJOR, 0)
set(PROJECT_VERSION_MINOR, 2)
set(PROJECT_VERSION_PATCH@, 2)
site_name(VERSION_HOST) # read hostname to VERSION_HOST
set(VERSION_HOST "${VERSION_HOST}" CACHE STRING "host of build" FORCE)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/support/cmake )

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/logs/)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/src/gen/)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/)

set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)

include(osmacros)

include(useGoldLinker)

# Helpful option enable build profiling to identify slowly compiling files
option(MEASURE_ALL "When enabled all commands will be passed through time command" OFF)
if(MEASURE_ALL)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "time")
endif()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/dist" CACHE PATH
            "Install path prefix, prepended onto install directories." FORCE )
endif()

# Enable helpfull warnings and C++14 for all files
if(MSVC)
    add_compile_options(/std:c++14 /W3 /WX )
else()
    # add_compile_options(-std=c++14 -Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wunused -pedantic)
    add_compile_options(-std=c++14)
    add_compile_options(-O0)
	add_compile_options(-g)
endif()

if(DEFINED ENV{BIOGEARS_HOME})
  list(APPEND CMAKE_PREFIX_PATH $ENV{BIOGEARS_HOME})
endif()
if(DEFINED ENV{FASTRTPS_HOME})
  list(APPEND CMAKE_PREFIX_PATH  $ENV{FASTRTPS_HOME})
endif()

find_package(fastcdr REQUIRED)
find_package(fastrtps REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost 1.58 COMPONENTS filesystem iostreams system signals thread log log_setup REQUIRED)
find_package(TinyXML2 REQUIRED)
find_package(SQLite3 REQUIRED)

find_package(Biogears)
message(STATUS "Biogears=${Biogears_FOUND}")
find_package(RapidJSON)
find_package(Pistache)

# External resources/repositories are downloaded here
# add_subdirectory(support/external)

# Documentation build
add_subdirectory(doc)

# Images, databases and other data which needs to be installed for project
add_subdirectory(support)

# Testing
# enable_testing()
# add_subdirectory(test)

# Source code
add_subdirectory(src)

message(STATUS "")
message(STATUS "    == Final overview for ${PROJECT_NAME} ==")
message(STATUS "Version:              ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH} ${VERSION_TYPE} @ ${VERSION_HOST}")
message(STATUS "Install prefix:       ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Compiler:             ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_BUILD_TYPE:     ${CMAKE_BUILD_TYPE}")
message(STATUS "  possible options: Debug Release RelWithDebInfo MinSizeRel")
message(STATUS "  set with ` cmake -DCMAKE_BUILD_TYPE=Debug .. `")
message(STATUS "")

