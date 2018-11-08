

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



macro (DEFINE_FastRTPS_SOURCES idlfilename)
  set(outsources)
  get_filename_component(it ${idlfilename} ABSOLUTE)
  get_filename_component(nfile ${idlfilename} NAME_WE)
  set(outsources ${outsources} ${CMAKE_CURRENT_BINARY_DIR}/gen/${nfile}.cxx ${CMAKE_CURRENT_BINARY_DIR}/gen/${nfile}.h)
  set(outsources ${outsources} ${CMAKE_CURRENT_BINARY_DIR}/gen/${nfile}PubSubTypes.cxx ${CMAKE_CURRENT_BINARY_DIR}/gen/${nfile}PubSubTypes.h)
endmacro (DEFINE_FastRTPS_SOURCES)

if (NOT FASTRTP_GEN_EXECUTABLE)
  find_program(FASTRTP_GEN_EXECUTABLE NAME fastrtpsgen
    DOC "FastRTPS code generator"
    PATH_SUFFIXES bin
  )
endif()

if (FASTRTP_GEN_EXECUTABLE)
macro (FastRTPS_IDLGEN idlfilename)
  get_filename_component(it ${idlfilename} ABSOLUTE)
  get_filename_component(idlfilename ${idlfilename} NAME)
  DEFINE_FastRTPS_SOURCES(${ARGV})
  add_custom_command (
          OUTPUT ${outsources}
          COMMAND ${FASTRTP_GEN_EXECUTABLE} 
          ARGS
          #-example x64Linux2.6gcc
          -replace -d ${CMAKE_CURRENT_SOURCE_DIR}/src/AMM/DDS ${idlfilename}
          DEPENDS ${it}
  )
endmacro (FastRTPS_IDLGEN)
endif()

# If fastrtpsgen is installed, we can do code generation.
if (COMMAND FastRTPS_IDLGEN)
    #FastRTPS_IDLGEN(${PROJECT_SOURCE_DIR}/../IDL/AMM.idl)
    add_custom_target(generate_from_idl COMMAND ${FASTRTP_GEN_EXECUTABLE} -replace -d ${CMAKE_CURRENT_SOURCE_DIR}/src/AMM/DDS ${PROJECT_SOURCE_DIR}/../IDL/AMM.idl)
endif()