cmake_minimum_required(VERSION 3.1)

project(AMM_Modules)




IF (WIN32)
    SET(ENV{BIOGEARS_HOME} "c:/tools/physiology/install")
    SET(ENV{FASTRTPS_HOME} "C:/tools/fast-rtps")
ELSE()
    SET(ENV{BIOGEARS_HOME} "/usr/local/physiology")
    SET(ENV{FASTRTPS_HOME} "/usr/local")
ENDIF()



set(fastcdr_DIR "$ENV{FASTRTPS_HOME}/lib/fastcdr/cmake")
set(fastrtps_DIR "$ENV{FASTRTPS_HOME}/lib/fastrtps/cmake")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/support/cmake ${fastcdr_DIR} ${fastrtps_DIR})

include (osmacros)

set(THREADS_PREFER_PTHREAD_FLAG ON)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/gen/)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/)

find_package (BioGears REQUIRED)
find_package (fastrtps REQUIRED)
find_package (fastcdr REQUIRED)
find_package (Threads REQUIRED)
find_package (Boost REQUIRED filesystem iostreams system)

find_package (RapidJSON)
find_package (Pistache)

include_directories(
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_BINARY_DIR}/gen
        ${BioGears_INCLUDE_DIRS}
        ${fastrtps_INCLUDE_DIR}
        ${fastcdr_INCLUDE_DIR}
)

set (DATAMODEL amm_types)
set (DDS_MANAGER dds_manager)

set (idls AMM.idl)
foreach(idl ${idls})
    FastRTPS_IDLGEN (${PROJECT_SOURCE_DIR}/../IDL/${idl})
    string (REGEX REPLACE "\(.*\).idl" "./gen/\\1.cxx" VARS_1 ${idl})
    string (REGEX REPLACE "\(.*\).idl" "./gen/\\1.h" VARS_2 ${idl})
    string (REGEX REPLACE "\(.*\).idl" "./gen/\\1PubSubTypes.cxx" VARS_3 ${idl})
    string (REGEX REPLACE "\(.*\).idl" "./gen/\\1PubSubTypes.h" VARS_4 ${idl})
    set(DATAMODEL_SOURCES ${DATAMODEL_SOURCES} ${VARS_1} ${VARS_2} ${VARS_3} ${VARS_4})
endforeach(idl)
set(DDS_MANAGER_SOURCES src/AMM/DDS_Manager.cpp src/AMM/DDS_Listeners.cpp src/AMM/ListenerInterface.h)

add_library (${DATAMODEL} SHARED ${DATAMODEL_SOURCES})

set (SIM_MANAGER_SOURCES src/SimManager.cpp src/AMM/SimulationManager.cpp)
set (SIM_MANAGER_EXE amm_sim_manager)
add_executable (${SIM_MANAGER_EXE} ${SIM_MANAGER_SOURCES} ${DATAMODEL_SOURCES} ${DDS_MANAGER_SOURCES})

set (PHYSIOLOGY_MANAGER_SOURCES src/PhysiologyManager.cpp src/AMM/PhysiologyEngineManager.cpp src/AMM/PhysiologyThread.cpp)
set (PHYSIOLOGY_MANAGER_EXE amm_physiology_manager)
add_executable (${PHYSIOLOGY_MANAGER_EXE} ${PHYSIOLOGY_MANAGER_SOURCES} ${DATAMODEL_SOURCES} ${DDS_MANAGER_SOURCES})

set (VIRTUAL_EQUIPMENT_SOURCES src/VirtualEquipment.cpp src/AMM/VirtualEquipmentListener.cpp src/AMM/VirtualEquipmentListener.h)
set (VIRTUAL_EQUIPMENT_EXE amm_virtual_equipment)
add_executable (${VIRTUAL_EQUIPMENT_EXE} ${VIRTUAL_EQUIPMENT_SOURCES} ${DATAMODEL_SOURCES} ${DDS_MANAGER_SOURCES})

set (COMMAND_EXECUTOR_SOURCES src/CommandExecutor.cpp)
set (COMMAND_EXECUTOR_EXE amm_command_executor)
add_executable (${COMMAND_EXECUTOR_EXE} ${COMMAND_EXECUTOR_SOURCES} ${DATAMODEL_SOURCES} ${DDS_MANAGER_SOURCES})

set (HEART_RATE_LED_SOURCES src/HeartRateLED.cpp)
set (HEART_RATE_LED_EXE amm_heartrate_led)
add_executable (${HEART_RATE_LED_EXE} ${HEART_RATE_LED_SOURCES} ${DATAMODEL_SOURCES} ${DDS_MANAGER_SOURCES})

set (ARDUINO_BRIDGE_SOURCES src/ArduinoSensor.cpp)
set (ARDUINO_BRIDGE_EXE amm_arduino_bridge)
add_executable (${ARDUINO_BRIDGE_EXE} ${ARDUINO_BRIDGE_SOURCES} ${DATAMODEL_SOURCES} ${DDS_MANAGER_SOURCES})

if (RAPIDJSON_FOUND AND Pistache_FOUND)
    set (REST_ADAPTER_SOURCES src/REST_Adapter.cpp)
    set (REST_ADAPTER_EXE amm_rest_adapter)
    add_executable (${REST_ADAPTER_EXE} ${REST_ADAPTER_SOURCES} ${DATAMODEL_SOURCES} ${DDS_MANAGER_SOURCES})
endif()


target_link_libraries (
        ${DATAMODEL}
        fastrtps fastcdr
)


target_link_libraries (${SIM_MANAGER_EXE}
        fastrtps fastcdr
        Threads::Threads
        ${DATAMODEL}
        )

target_link_libraries (${PHYSIOLOGY_MANAGER_EXE}
        fastrtps fastcdr
        ${BioGears_LIBRARIES}
        Threads::Threads
        ${DATAMODEL}
        )

target_link_libraries (${VIRTUAL_EQUIPMENT_EXE}
        fastrtps fastcdr
        ${DATAMODEL}
        )

target_link_libraries (${COMMAND_EXECUTOR_EXE}
        fastrtps fastcdr
        ${DATAMODEL}
        )

target_link_libraries (${HEART_RATE_LED_EXE}
        fastrtps fastcdr
        ${DATAMODEL}
        )

target_link_libraries (${ARDUINO_BRIDGE_EXE}
        fastrtps fastcdr
        Boost::filesystem Boost::iostreams
        ${DATAMODEL}
        )

if (RAPIDJSON_FOUND AND Pistache_FOUND)
target_link_libraries (${REST_ADAPTER_EXE}
        fastrtps fastcdr ${RAPIDJSON_LIBRARIES} ${Pistache_LIBRARY}
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
