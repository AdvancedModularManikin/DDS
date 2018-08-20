cmake_minimum_required(VERSION 3.5.1)

project(AMM_Modules)

IF (WIN32)
    SET(ENV{BIOGEARS_HOME} "c:/tools/physiology/install")
    SET(ENV{FASTRTPS_HOME} "C:/tools/fast-rtps")
ELSE ()
    SET(ENV{BIOGEARS_HOME} "/usr/local/physiology")
    SET(ENV{FASTRTPS_HOME} "/usr/local")
ENDIF ()

set(fastcdr_DIR "$ENV{FASTRTPS_HOME}/lib/fastcdr/cmake")
set(fastrtps_DIR "$ENV{FASTRTPS_HOME}/lib/fastrtps/cmake")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/support/cmake ${fastcdr_DIR} ${fastrtps_DIR})

include(osmacros)

set(THREADS_PREFER_PTHREAD_FLAG ON)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/logs/)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/gen/)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/)

find_package(fastrtps REQUIRED)
find_package(fastcdr REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost REQUIRED filesystem iostreams system signals thread)
find_package(TinyXML2 REQUIRED)

find_package(SQLite3)
find_package(BioGears)
find_package(RapidJSON)
find_package(Pistache)

include_directories(
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/src/thirdparty
        ${CMAKE_CURRENT_BINARY_DIR}/gen
        ${BioGears_INCLUDE_DIRS}
        ${fastrtps_INCLUDE_DIR}
        ${fastcdr_INCLUDE_DIR}
        ${TINYXML2_INCLUDE_DIR}
)

set(DATAMODEL amm_types)

set(idls AMM.idl)
foreach (idl ${idls})
    FastRTPS_IDLGEN(${PROJECT_SOURCE_DIR}/../IDL/${idl})
    string(REGEX REPLACE "\(.*\).idl" "./gen/\\1.cxx" VARS_1 ${idl})
    string(REGEX REPLACE "\(.*\).idl" "./gen/\\1.h" VARS_2 ${idl})
    string(REGEX REPLACE "\(.*\).idl" "./gen/\\1PubSubTypes.cxx" VARS_3 ${idl})
    string(REGEX REPLACE "\(.*\).idl" "./gen/\\1PubSubTypes.h" VARS_4 ${idl})
    set(DATAMODEL_SOURCES ${DATAMODEL_SOURCES} ${VARS_1} ${VARS_2} ${VARS_3} ${VARS_4})
endforeach (idl)

set(DDS_MANAGER dds_manager)
set(DDS_TYPES dds_types)
set(DDS_MANAGER_SOURCES src/AMM/DDS_Manager.cpp src/AMM/DDS_Listeners.cpp src/AMM/ListenerInterface.h)
set(DDS_TYPE_SOURCES src/AMM/DataTypes.cpp src/AMM/DataTypes.h)

add_library(${DATAMODEL} SHARED ${DATAMODEL_SOURCES} ${DDS_MANAGER_SOURCES} ${DDS_TYPE_SOURCES})

target_link_libraries(
        ${DATAMODEL}
        fastrtps fastcdr
)

set(SPI_PROTO_SOURCES src/spi_proto/crc16.c src/spi_proto/spi_proto_lib/spi_proto.c src/spi_proto/config.h src/spi_proto/spi_proto.h)
add_library(spi_proto ${SPI_PROTO_SOURCES})

set(SIM_MANAGER_SOURCES src/SimManager.cpp src/AMM/SimulationManager.cpp)
set(SIM_MANAGER_EXE amm_sim_manager)
add_executable(${SIM_MANAGER_EXE} ${SIM_MANAGER_SOURCES} ${DATAMODEL_SOURCES} ${DDS_MANAGER_SOURCES} ${DDS_TYPE_SOURCES})

set(VIRTUAL_EQUIPMENT_SOURCES src/VirtualEquipment.cpp src/AMM/VirtualEquipmentListener.cpp src/AMM/VirtualEquipmentListener.h)
set(VIRTUAL_EQUIPMENT_EXE amm_virtual_equipment)
add_executable(${VIRTUAL_EQUIPMENT_EXE} ${VIRTUAL_EQUIPMENT_SOURCES} ${DATAMODEL_SOURCES} ${DDS_MANAGER_SOURCES} ${DDS_TYPE_SOURCES})

set(COMMAND_EXECUTOR_SOURCES src/CommandExecutor.cpp)
set(COMMAND_EXECUTOR_EXE amm_command_executor)
add_executable(${COMMAND_EXECUTOR_EXE} ${COMMAND_EXECUTOR_SOURCES} ${DATAMODEL_SOURCES} ${DDS_MANAGER_SOURCES} ${DDS_TYPE_SOURCES})

set(HEART_RATE_LED_SOURCES src/HeartRateLED.cpp)
set(HEART_RATE_LED_EXE amm_heartrate_led)
add_executable(${HEART_RATE_LED_EXE} ${HEART_RATE_LED_SOURCES} ${DATAMODEL_SOURCES} ${DDS_MANAGER_SOURCES} ${DDS_TYPE_SOURCES})
target_link_libraries(${HEART_RATE_LED_EXE} spi_proto)

set(FLUID_MANAGER_SOURCES src/FluidManager.cpp)
set(FLUID_MANAGER_EXE amm_fluid_manager)
add_executable(${FLUID_MANAGER_EXE} ${FLUID_MANAGER_SOURCES} ${DATAMODEL_SOURCES} ${DDS_MANAGER_SOURCES} ${DDS_TYPE_SOURCES} src/spi_proto/spi_remote_host.c src/spi_proto/binary_semaphore.c src/spi_proto/spi_proto_master.cpp src/spi_proto/spi_proto_lib/spi_chunks.cpp)
target_link_libraries(${FLUID_MANAGER_EXE} spi_proto)
target_include_directories(${FLUID_MANAGER_EXE} PUBLIC src/spi_proto)
set_target_properties(${FLUID_MANAGER_EXE} PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
        )

set(IVC_MODULE_SOURCES src/IVC.cpp)
set(IVC_MODULE_EXE amm_ivc_module)
add_executable(${IVC_MODULE_EXE} ${IVC_MODULE_SOURCES} ${DATAMODEL_SOURCES} ${DDS_MANAGER_SOURCES} ${DDS_TYPE_SOURCES})
target_link_libraries(${IVC_MODULE_EXE} spi_proto)
set_target_properties(${IVC_MODULE_EXE} PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
        )

set(SERIAL_BRIDGE_SOURCES src/Serial_Bridge.cpp src/AMM/SerialPort.cpp src/AMM/SerialPort.h)
set(SERIAL_BRIDGE_EXE amm_serial_bridge)
add_executable(${SERIAL_BRIDGE_EXE} ${SERIAL_BRIDGE_SOURCES} ${DATAMODEL_SOURCES} ${DDS_MANAGER_SOURCES} ${DDS_TYPE_SOURCES})

set(TCP_BRIDGE_SOURCES src/TCP_Bridge.cpp src/Net/Server.cpp src/Net/ServerThread.cpp src/Net/Client.cpp src/Net/UdpDiscoveryServer.cpp)
set(TCP_BRIDGE_EXE amm_tcp_bridge)
add_executable(${TCP_BRIDGE_EXE} ${TCP_BRIDGE_SOURCES} ${DATAMODEL_SOURCES} ${DDS_MANAGER_SOURCES} ${DDS_TYPE_SOURCES})
set_target_properties(${TCP_BRIDGE_EXE} PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
        )


set(CAPTURE_SOURCES src/Capture.cpp src/AMM/CaptureListener.cpp src/AMM/CaptureListener.h)
set(CAPTURE_EXE amm_capture)
add_executable(${CAPTURE_EXE} ${CAPTURE_SOURCES} ${DATAMODEL_SOURCES} ${DDS_MANAGER_SOURCES} ${DDS_TYPE_SOURCES})
set_target_properties(${CAPTURE_EXE} PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
        )

set(LOGGER_SOURCES src/Logger.cpp)
set(LOGGER_EXE amm_logger)
add_executable(${LOGGER_EXE} ${LOGGER_SOURCES})
target_link_libraries(${LOGGER_EXE}
        fastrtps fastcdr
        Boost::system
        ${DATAMODEL}
        )

set(MODULE_MANAGER_SOURCES src/ModuleManager.cpp src/AMM/ModuleManager.cpp)
set(MODULE_MANAGER_EXE amm_module_manager)
add_executable(${MODULE_MANAGER_EXE} ${MODULE_MANAGER_SOURCES} ${DATAMODEL_SOURCES} ${DDS_MANAGER_SOURCES} ${DDS_TYPE_SOURCES})
set_target_properties(${MODULE_MANAGER_EXE} PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
        )
target_link_libraries(${MODULE_MANAGER_EXE}
        fastrtps fastcdr
        Threads::Threads
        ${TINYXML2_LIBRARIES}
        dl sqlite3 pthread
        ${DATAMODEL}
        )

if (RAPIDJSON_FOUND AND Pistache_FOUND)
    set(REST_ADAPTER_SOURCES src/REST_Adapter.cpp)
    set(REST_ADAPTER_EXE amm_rest_adapter)

    add_executable(${REST_ADAPTER_EXE} ${REST_ADAPTER_SOURCES} src/Net/UdpDiscoveryServer.cpp ${DATAMODEL_SOURCES} ${DDS_MANAGER_SOURCES} ${DDS_TYPE_SOURCES})
    set_target_properties(${REST_ADAPTER_EXE} PROPERTIES
            CXX_STANDARD 14
            CXX_STANDARD_REQUIRED ON
            )
    target_link_libraries(${REST_ADAPTER_EXE}
            fastrtps fastcdr
            ${RAPIDJSON_LIBRARIES}
            ${Pistache_LIBRARY}
            ${TINYXML2_LIBRARIES}
            Boost::system Boost::filesystem Boost::iostreams
            Threads::Threads
            dl sqlite3 pthread
            ${DATAMODEL}
            )
endif ()


if (BioGears_FOUND)
    set(PHYSIOLOGY_MANAGER_SOURCES src/PhysiologyManager.cpp src/AMM/PhysiologyEngineManager.cpp src/AMM/PhysiologyThread.cpp)
    set(PHYSIOLOGY_MANAGER_EXE amm_physiology_manager)
    add_executable(${PHYSIOLOGY_MANAGER_EXE} ${PHYSIOLOGY_MANAGER_SOURCES} ${DATAMODEL_SOURCES} ${DDS_MANAGER_SOURCES} ${DDS_TYPE_SOURCES})
    target_link_libraries(${PHYSIOLOGY_MANAGER_EXE}
            fastrtps fastcdr
            ${BioGears_LIBRARIES}
            Threads::Threads
            ${DATAMODEL}
            )

    file(COPY ${PROJECT_SOURCE_DIR}/support/Actions DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/)
    file(COPY ${PROJECT_SOURCE_DIR}/support/patients DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/)
    file(COPY ${PROJECT_SOURCE_DIR}/support/states DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/)

    file(COPY ${PROJECT_SOURCE_DIR}/support/config DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin)
    file(COPY ${PROJECT_SOURCE_DIR}/support/ecg DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/)
    file(COPY ${PROJECT_SOURCE_DIR}/support/environments DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/)
    file(COPY ${PROJECT_SOURCE_DIR}/support/nutrition DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/)
    file(COPY ${PROJECT_SOURCE_DIR}/support/substances DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/)

    #file(COPY $ENV{BIOGEARS_HOME}/library/bin/config DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin)
    #file(COPY $ENV{BIOGEARS_HOME}/library/bin/ecg DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/)
    #file(COPY $ENV{BIOGEARS_HOME}/library/bin/environments DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/)
    #file(COPY $ENV{BIOGEARS_HOME}/library/bin/nutrition DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/)
    #file(COPY $ENV{BIOGEARS_HOME}/library/bin/substances DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/)
    #file(COPY $ENV{BIOGEARS_HOME}/library/bin/xsd DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/)
    #file(COPY $ENV{BIOGEARS_HOME}/library/bin/UCEDefs.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/)

    configure_file(${PROJECT_SOURCE_DIR}/support/BioGearsConfiguration.xml ${CMAKE_CURRENT_BINARY_DIR}/bin COPYONLY)
    configure_file(${PROJECT_SOURCE_DIR}/support/log4j.properties ${CMAKE_CURRENT_BINARY_DIR}/bin COPYONLY)
endif ()

file(COPY ${PROJECT_SOURCE_DIR}/support/mule1 DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin)

target_link_libraries(${SIM_MANAGER_EXE}
        fastrtps fastcdr
        Threads::Threads
        ${DATAMODEL}
        )

target_link_libraries(${VIRTUAL_EQUIPMENT_EXE}
        fastrtps fastcdr
        ${DATAMODEL}
        )

target_link_libraries(${COMMAND_EXECUTOR_EXE}
        fastrtps fastcdr
        ${DATAMODEL}
        )

target_link_libraries(${CAPTURE_EXE}
        fastrtps fastcdr
        dl sqlite3 pthread
        ${DATAMODEL}
        )


target_link_libraries(${HEART_RATE_LED_EXE}
        fastrtps fastcdr
        ${DATAMODEL}
        )

target_link_libraries(${FLUID_MANAGER_EXE}
        fastrtps fastcdr
        ${TINYXML2_LIBRARIES}
        ${DATAMODEL}
        )

target_link_libraries(${IVC_MODULE_EXE}
        fastrtps fastcdr
        ${TINYXML2_LIBRARIES}
        ${DATAMODEL}
        )

target_link_libraries(${SERIAL_BRIDGE_EXE}
        fastrtps fastcdr
        Boost::system Boost::filesystem Boost::iostreams Boost::signals Boost::thread
        Threads::Threads
        pthread
        ${TINYXML2_LIBRARIES}
        ${DATAMODEL}
        )

target_link_libraries(${TCP_BRIDGE_EXE}
        fastrtps fastcdr
        Boost::system
        Threads::Threads
        ${TINYXML2_LIBRARIES}
        ${DATAMODEL}
        )
