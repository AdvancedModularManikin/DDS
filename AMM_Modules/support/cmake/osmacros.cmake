

# Set C++11
include(CheckCXXCompilerFlag)
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG OR
        CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    check_cxx_compiler_flag(--std=c++11 SUPPORTS_CXX11)
    if(SUPPORTS_CXX11)
        add_compile_options(--std=c++11)
    else()
        message(FATAL_ERROR "Compiler doesn't support C++11")
    endif()
endif()



MACRO (DEFINE_FastRTPS_SOURCES idlfilename)
  SET(outsources)
  GET_FILENAME_COMPONENT(it ${idlfilename} ABSOLUTE)
  GET_FILENAME_COMPONENT(nfile ${idlfilename} NAME_WE)
  SET(outsources ${outsources} gen/${nfile}.cxx gen/${nfile}.h)
  SET(outsources ${outsources} gen/${nfile}PubSubTypes.cxx gen/${nfile}PubSubTypes.h)
ENDMACRO (DEFINE_FastRTPS_SOURCES)

MACRO (FastRTPS_IDLGEN idlfilename)
  GET_FILENAME_COMPONENT(it ${idlfilename} ABSOLUTE)
  GET_FILENAME_COMPONENT(idlfilename ${idlfilename} NAME)
  DEFINE_FastRTPS_SOURCES(${ARGV})
  ADD_CUSTOM_COMMAND (
          OUTPUT ${outsources}
          COMMAND fastrtpsgen
          ARGS
          #-example x64Linux2.6gcc
          -replace -d gen ${idlfilename}
          DEPENDS ${it}
  )
ENDMACRO (FastRTPS_IDLGEN)
