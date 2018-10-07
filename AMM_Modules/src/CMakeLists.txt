include(cleanCppExtensions)

# configure version.cpp.in with selected version
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/version.cpp.in"
        "${CMAKE_CURRENT_BINARY_DIR}/version.cpp"
)
# If you want to have date and time of the build, your targets has to depend on this.
# This will force recompilation of version.o and thus forcing gcc to update __DATE__ macro.
add_custom_target(versionFileTouchForRebuild
        COMMAND ${CMAKE_COMMAND} -E touch "${CMAKE_CURRENT_BINARY_DIR}/version.cpp")


set(BASE_LOGGER amm_base_logger)
set(BASE_LOGGER_SOURCES AMM/BaseLogger.cpp)
add_library(amm_base_logger SHARED ${BASE_LOGGER_SOURCES})
add_library(amm::logger ALIAS amm_base_logger)

target_link_libraries(
        amm_base_logger
        PUBLIC Threads::Threads
        PUBLIC Boost::log
)

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
set(DDS_MANAGER_SOURCES AMM/DDS_Manager.cpp AMM/Listeners/DDS_Listeners.cpp AMM/Listeners/ListenerInterface.h)
set(DDS_TYPE_SOURCES AMM/DataTypes.cpp AMM/DataTypes.h)
add_library(amm_types SHARED ${DATAMODEL_SOURCES} ${DDS_TYPE_SOURCES} ${DDS_MANAGER_SOURCES})
add_library(amm::datamodel ALIAS amm_types)
add_dependencies(amm_types versionFileTouchForRebuild)
target_link_libraries(
        amm_types
        fastrtps fastcdr
        amm::logger
)
target_include_directories(amm_types PUBLIC
        PUBLIC ${PROJECT_SOURCE_DIR}/src
        PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/gen
        )

set(SIM_MANAGER_SOURCES SimManager.cpp AMM/SimulationManager.cpp)
set(SIM_MANAGER_EXE amm_sim_manager)
add_executable(${SIM_MANAGER_EXE} ${SIM_MANAGER_SOURCES})
add_dependencies(${SIM_MANAGER_EXE} versionFileTouchForRebuild)
install(TARGETS ${SIM_MANAGER_EXE}
        BUNDLE DESTINATION "."
        RUNTIME DESTINATION bin)
addRunAndDebugTargets(${SIM_MANAGER_EXE})
add_custom_target(run DEPENDS ${SIM_MANAGER_EXE}-run)

set(VIRTUAL_EQUIPMENT_SOURCES VirtualEquipment.cpp AMM/Listeners/VirtualEquipmentListener.cpp)
set(VIRTUAL_EQUIPMENT_EXE amm_virtual_equipment)
add_executable(${VIRTUAL_EQUIPMENT_EXE} ${VIRTUAL_EQUIPMENT_SOURCES})

set(COMMAND_EXECUTOR_SOURCES CommandExecutor.cpp)
set(COMMAND_EXECUTOR_EXE amm_command_executor)
add_executable(${COMMAND_EXECUTOR_EXE} ${COMMAND_EXECUTOR_SOURCES})

set(SPI_SOURCES spi_proto/src/crc16.c spi_proto/src/spi_proto.c spi_proto/src/config.h spi_proto/src/spi_proto.h)
#add_library(spi_proto ${SPI_PROTO_SOURCES})
set(SPI_REMOTE_SOURCES spi_proto/src/spi_remote_host.c spi_proto/src/binary_semaphore.c spi_proto/src/spi_proto_master.cpp spi_proto/src/spi_chunks.cpp)
set(SPI_DATAGRAM_SOURCES spi_proto/src/spi_proto_master_datagram.cpp)

set(HEART_RATE_LED_SOURCES HeartRateLED.cpp)
set(HEART_RATE_LED_EXE amm_heartrate_led)
add_executable(${HEART_RATE_LED_EXE} ${HEART_RATE_LED_SOURCES} ${SPI_SOURCES} ${SPI_DATAGRAM_SOURCES})
target_include_directories(${HEART_RATE_LED_EXE} PUBLIC spi_proto/src)

set(FLUID_MANAGER_SOURCES FluidManager.cpp)
set(FLUID_MANAGER_EXE amm_fluid_manager)
add_executable(${FLUID_MANAGER_EXE} ${FLUID_MANAGER_SOURCES} ${SPI_SOURCES} ${SPI_REMOTE_SOURCES})
target_include_directories(${FLUID_MANAGER_EXE} PUBLIC spi_proto/src)

set(IVC_MODULE_SOURCES IVC.cpp)
set(IVC_MODULE_EXE amm_ivc_module)
add_executable(${IVC_MODULE_EXE} ${IVC_MODULE_SOURCES} ${SPI_SOURCES} ${SPI_REMOTE_SOURCES})
target_include_directories(${IVC_MODULE_EXE} PUBLIC spi_proto/src)

set(SERIAL_BRIDGE_SOURCES Serial_Bridge.cpp AMM/SerialPort.cpp AMM/SerialPort.h)
set(SERIAL_BRIDGE_EXE amm_serial_bridge)
add_executable(${SERIAL_BRIDGE_EXE} ${SERIAL_BRIDGE_SOURCES})

set(TCP_BRIDGE_SOURCES TCP_Bridge.cpp Net/Server.cpp Net/ServerThread.cpp Net/Client.cpp Net/UdpDiscoveryServer.cpp)
set(TCP_BRIDGE_EXE amm_tcp_bridge)
add_executable(${TCP_BRIDGE_EXE} ${TCP_BRIDGE_SOURCES})

set(LOGGER_SOURCES Logger.cpp)
set(LOGGER_EXE amm_logger)
add_executable(${LOGGER_EXE} ${LOGGER_SOURCES})

set(MODULE_MANAGER_SOURCES ModuleManager.cpp AMM/ModuleManager.cpp)
set(MODULE_MANAGER_EXE amm_module_manager)
add_executable(${MODULE_MANAGER_EXE} ${MODULE_MANAGER_SOURCES})
target_link_libraries(${MODULE_MANAGER_EXE}
        fastrtps fastcdr
        amm::logger
        Threads::Threads
        ${TINYXML2_LIBRARIES}
        sqlite3
        amm::datamodel
        )

if (Pistache_FOUND)
    set(REST_ADAPTER_SOURCES REST_Adapter.cpp Net/UdpDiscoveryServer.cpp)
    set(REST_ADAPTER_EXE amm_rest_adapter)
    add_executable(${REST_ADAPTER_EXE} ${REST_ADAPTER_SOURCES})
    target_link_libraries(${REST_ADAPTER_EXE}
            fastrtps fastcdr
            amm::logger
            ${RAPIDJSON_LIBRARIES}
            ${Pistache_LIBRARY}
            ${TINYXML2_LIBRARIES}
            Boost::system Boost::filesystem Boost::iostreams
            Threads::Threads
            sqlite3
            amm::datamodel
            )
endif ()


if (Biogears_FOUND)
    set(PHYSIOLOGY_MANAGER_SOURCES PhysiologyManager.cpp AMM/PhysiologyEngineManager.cpp AMM/PhysiologyThread.cpp)
    set(PHYSIOLOGY_MANAGER_EXE amm_physiology_manager)
    add_executable(${PHYSIOLOGY_MANAGER_EXE} ${PHYSIOLOGY_MANAGER_SOURCES})
    target_link_libraries(${PHYSIOLOGY_MANAGER_EXE}
            fastrtps fastcdr
            amm::logger
            Biogears::libbiogears
            Threads::Threads
            amm::datamodel
            )
endif ()

target_link_libraries(${LOGGER_EXE}
        PUBLIC fastrtps fastcdr
        PUBLIC amm::logger
        PUBLIC Boost::system
        PUBLIC pthread
        PUBLIC amm::datamodel
        )

target_link_libraries(${SIM_MANAGER_EXE}
        fastrtps fastcdr
        amm::logger
        Threads::Threads
        amm::datamodel
        )

target_link_libraries(${VIRTUAL_EQUIPMENT_EXE}
        fastrtps fastcdr
        amm::datamodel
        )


target_link_libraries(${COMMAND_EXECUTOR_EXE}
        fastrtps fastcdr
        amm::datamodel
        )

target_link_libraries(${HEART_RATE_LED_EXE}
        fastrtps fastcdr
        amm::logger
        amm::datamodel
        )

target_link_libraries(${FLUID_MANAGER_EXE}
        fastrtps fastcdr
        amm::logger
        ${TINYXML2_LIBRARIES}
        amm::datamodel
        )

target_link_libraries(${IVC_MODULE_EXE}
        fastrtps fastcdr
        amm::logger
        ${TINYXML2_LIBRARIES}
        amm::datamodel
        )

target_link_libraries(${SERIAL_BRIDGE_EXE}
        fastrtps fastcdr
        amm::logger
        Boost::system Boost::filesystem Boost::iostreams Boost::signals Boost::thread
        Threads::Threads
        pthread
        ${TINYXML2_LIBRARIES}
        amm::datamodel
        )

target_link_libraries(${TCP_BRIDGE_EXE}
        fastrtps fastcdr
        amm::logger
        Boost::system
        Threads::Threads
        ${TINYXML2_LIBRARIES}
        amm::datamodel
        )


#add_executable(amm_info amm_info.cpp )
#target_link_libraries(amm_info
#        fastrtps
#        fastcdr
#        amm::datamodel
#        )


# add_subdirectory(unittest)

if (MINGW)
    message(STATUS "MinGW detected")
    get_filename_component(GCC_PATH ${CMAKE_C_COMPILER} PATH)
    if (${GCC_PATH} MATCHES "mingw64/bin")
        set(libgcc "libgcc_s_seh-1.dll") #64bit
        message(STATUS "  64bit dlls is building")
    else ()
        set(libgcc "libgcc_s_dw2-1.dll") #32bit
        message(STATUS "  32bit dlls is building")
    endif ()

    install(FILES ${GCC_PATH}/${libgcc}
            ${GCC_PATH}/libwinpthread-1.dll
            ${GCC_PATH}/libstdc++-6.dll
            DESTINATION ./bin/
            )
endif (MINGW)

addMiscTargets()
