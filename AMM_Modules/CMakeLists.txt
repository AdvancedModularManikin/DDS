cmake_minimum_required(VERSION 3.1)
project(AMM_Modules)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/support/cmake)
set(CMAKE_CXX_STANDARD 11)
set(THREADS_PREFER_PTHREAD_FLAG ON)


file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/)

SET(ENV{OSPL_HOME} "/usr/local/opensplice")
SET(ENV{BIOGEARS_HOME} "/usr/local/physiology")



include (osmacros)
include (MacroOpenSplice)

find_package (OpenSplice REQUIRED)
find_package (BioGears REQUIRED)
find_package (Threads)
find_package (Boost)
find_package (RapidJSON)
find_package (Pistache)

include_directories(
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/src/AMM
        ${CMAKE_BINARY_DIR}/gen
        ${BioGears_INCLUDE_DIRS}
        ${OpenSplice_INCLUDE_DIRS}
)

set (idls AMM.idl)

foreach(idl ${idls})
    OpenSplice_IDLGEN (${CMAKE_SOURCE_DIR}/../IDL/${idl})
    string (REGEX REPLACE "\(.*\).idl" "./gen/\\1.cpp" VARS_1 ${idl})
    string (REGEX REPLACE "\(.*\).idl" "./gen/\\1.h" VARS_2 ${idl})
    string (REGEX REPLACE "\(.*\).idl" "./gen/\\1Dcps.cpp" VARS_3 ${idl})
    string (REGEX REPLACE "\(.*\).idl" "./gen/\\1Dcps.h" VARS_4 ${idl})
    string (REGEX REPLACE "\(.*\).idl" "./gen/\\1Dcps_impl.cpp" VARS_5 ${idl})
    string (REGEX REPLACE "\(.*\).idl" "./gen/\\1Dcps_impl.h" VARS_6 ${idl})
    string (REGEX REPLACE "\(.*\).idl" "./gen/\\1SplDcps.cpp" VARS_7 ${idl})
    string (REGEX REPLACE "\(.*\).idl" "./gen/\\1SplDcps.h" VARS_8 ${idl})
    string (REGEX REPLACE "\(.*\).idl" "./gen/ccpp_\\1.h" VARS_9 ${idl})
    set(OpenSplice_DATAMODEL ${OpenSplice_DATAMODEL} ${VARS_1} ${VARS_2} ${VARS_3} ${VARS_4} ${VARS_5} ${VARS_6} ${VARS_7} ${VARS_8} ${VARS_9})
endforeach(idl)

set (DATAMODEL amm_types)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

set (SIM_MANAGER_SOURCES src/SimManager.cpp src/AMM/SimulationManager.cpp src/AMM/DDSEntityManager.cpp src/AMM/CheckStatus.cpp)
set (SIM_MANAGER_EXE amm_sim_manager)
add_executable (${SIM_MANAGER_EXE} ${SIM_MANAGER_SOURCES})

set (PHYSIOLOGY_MANAGER_SOURCES src/PhysiologyManager.cpp src/AMM/PhysiologyEngineManager.cpp src/AMM/BioGearsThread.cpp src/AMM/DDSEntityManager.cpp src/AMM/CheckStatus.cpp)
set (PHYSIOLOGY_MANAGER_EXE amm_physiology_manager)
add_executable (${PHYSIOLOGY_MANAGER_EXE} ${PHYSIOLOGY_MANAGER_SOURCES})

set (VIRTUAL_EQUIPMENT_SOURCES src/VirtualEquipment.cpp src/AMM/DDSEntityManager.cpp src/AMM/CheckStatus.cpp)
set (VIRTUAL_EQUIPMENT_EXE amm_virtual_equipment)
add_executable (${VIRTUAL_EQUIPMENT_EXE} ${VIRTUAL_EQUIPMENT_SOURCES})

set (COMMAND_EXECUTOR_SOURCES src/CommandExecutor.cpp src/AMM/DDSEntityManager.cpp src/AMM/CheckStatus.cpp)
set (COMMAND_EXECUTOR_EXE amm_command_executor)
add_executable (${COMMAND_EXECUTOR_EXE} ${COMMAND_EXECUTOR_SOURCES})

set (HEART_RATE_LED_SOURCES src/HeartRateLED.cpp src/AMM/DDSEntityManager.cpp src/AMM/CheckStatus.cpp)
set (HEART_RATE_LED_EXE amm_heartrate_led)
add_executable (${HEART_RATE_LED_EXE} ${HEART_RATE_LED_SOURCES})

if (RAPIDJSON_FOUND AND Pistache_FOUND)
    set (REST_ADAPTER_SOURCES src/REST_Adapter.cpp src/AMM/DDSEntityManager.cpp src/AMM/CheckStatus.cpp)
    set (REST_ADAPTER_EXE amm_rest_adapter)
    add_executable (${REST_ADAPTER_EXE} ${REST_ADAPTER_SOURCES})
endif()

add_library (${DATAMODEL} SHARED ${OpenSplice_DATAMODEL})

add_definitions (
        ${OpenSplice_DEFINITIONS}
        ${DEFINITIONS}
)

target_link_libraries (
        ${DATAMODEL}
        ${OpenSplice_LIBRARIES}
)

target_link_libraries (${SIM_MANAGER_EXE}
        ${OpenSplice_LIBRARIES}
        Threads::Threads
        ${DATAMODEL}
        )

target_link_libraries (${PHYSIOLOGY_MANAGER_EXE}
        ${OpenSplice_LIBRARIES} ${BioGears_LIBRARIES}
        Threads::Threads
        ${DATAMODEL}
        )

target_link_libraries (${VIRTUAL_EQUIPMENT_EXE}
        ${OpenSplice_LIBRARIES}
        ${DATAMODEL}
        )

target_link_libraries (${COMMAND_EXECUTOR_EXE}
        ${OpenSplice_LIBRARIES}
        ${DATAMODEL}
        )

target_link_libraries (${HEART_RATE_LED_EXE}
        ${OpenSplice_LIBRARIES}
        ${DATAMODEL}
        )

if (RAPIDJSON_FOUND AND Pistache_FOUND)
target_link_libraries (${REST_ADAPTER_EXE}
        ${OpenSplice_LIBRARIES} ${RAPIDJSON_LIBRARIES} ${Pistache_LIBRARY}
        Threads::Threads
        ${DATAMODEL}
        )
endif()

file(COPY ${PROJECT_SOURCE_DIR}/support/Actions DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/)
file(COPY ${PROJECT_SOURCE_DIR}/support/config DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin)
file(COPY ${PROJECT_SOURCE_DIR}/support/ecg DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/)
file(COPY ${PROJECT_SOURCE_DIR}/support/environments DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/)
file(COPY ${PROJECT_SOURCE_DIR}/support/nutrition DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/)
file(COPY ${PROJECT_SOURCE_DIR}/support/patients DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/)
file(COPY ${PROJECT_SOURCE_DIR}/support/states DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/)
file(COPY ${PROJECT_SOURCE_DIR}/support/substances DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/)
file(COPY $ENV{BIOGEARS_HOME}/library/bin/xsd DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/)
file(COPY $ENV{BIOGEARS_HOME}/library/bin/UCEDefs.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/)

configure_file(${PROJECT_SOURCE_DIR}/support/BioGearsConfiguration.xml ${CMAKE_CURRENT_BINARY_DIR}/bin COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/support/log4j.properties ${CMAKE_CURRENT_BINARY_DIR}/bin COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/support/ospl.xml ${CMAKE_CURRENT_BINARY_DIR}/bin COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/support/release.sh ${CMAKE_CURRENT_BINARY_DIR}/bin COPYONLY)
