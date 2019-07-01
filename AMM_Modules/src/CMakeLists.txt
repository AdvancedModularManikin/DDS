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

add_library(plog INTERFACE)
target_include_directories(plog INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty)
file(GLOB_RECURSE PLOG_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/plog/*.h)
add_library(plog-headers SHARED ${PLOG_HEADERS})
set_target_properties(plog-headers PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(plog-headers PROPERTIES FOLDER Include)

set(BASE_LOGGER amm_base_logger)
set(BASE_LOGGER_SOURCES AMM/BaseLogger.cpp)
if (MSVC)
    add_library(${BASE_LOGGER} STATIC ${BASE_LOGGER_SOURCES})
else ()
    add_library(${BASE_LOGGER} SHARED ${BASE_LOGGER_SOURCES})
endif ()

target_link_libraries(
        amm_base_logger
        PUBLIC Threads::Threads
        PUBLIC plog
)

set(VARS_1 "AMM/DDS/AMM.cxx")
set(VARS_2 "AMM/DDS/AMM.h")
set(VARS_3 "AMM/DDS/AMMPubSubTypes.cxx")
set(VARS_4 "AMM/DDS/AMMPubSubTypes.h")
set(DATAMODEL_SOURCES ${DATAMODEL_SOURCES} ${VARS_1} ${VARS_2} ${VARS_3} ${VARS_4})

set(DDS_MANAGER dds_manager)
set(DDS_MANAGER_SOURCES AMM/DDS_Manager.cpp AMM/Listeners/DDS_Listeners.cpp AMM/Listeners/ListenerInterface.h)
set(DDS_TYPE_SOURCES AMM/DataTypes.cpp AMM/DataTypes.h)
if (MSVC)
    add_library(amm_types STATIC ${DATAMODEL_SOURCES} ${DDS_TYPE_SOURCES} ${DDS_MANAGER_SOURCES})
else ()
    add_library(amm_types SHARED ${DATAMODEL_SOURCES} ${DDS_TYPE_SOURCES} ${DDS_MANAGER_SOURCES})
endif ()
add_dependencies(amm_types versionFileTouchForRebuild)
target_link_libraries(
        amm_types
        PUBLIC fastrtps
        PUBLIC fastcdr
        PUBLIC amm_base_logger
        ${Boost_LIBRARIES}
)
target_include_directories(amm_types PUBLIC ${PROJECT_SOURCE_DIR}/src)

set(SIM_MANAGER_SOURCES SimManager.cpp AMM/SimulationManager.cpp)
set(SIM_MANAGER_EXE amm_sim_manager)
add_executable(${SIM_MANAGER_EXE} ${SIM_MANAGER_SOURCES})
add_dependencies(${SIM_MANAGER_EXE} versionFileTouchForRebuild)

set(VIRTUAL_EQUIPMENT_SOURCES VirtualEquipment.cpp AMM/Listeners/VirtualEquipmentListener.cpp)
set(VIRTUAL_EQUIPMENT_EXE amm_virtual_equipment)
add_executable(${VIRTUAL_EQUIPMENT_EXE} ${VIRTUAL_EQUIPMENT_SOURCES})

set(COMMAND_EXECUTOR_SOURCES CommandExecutor.cpp)
set(COMMAND_EXECUTOR_EXE amm_command_executor)
add_executable(${COMMAND_EXECUTOR_EXE} ${COMMAND_EXECUTOR_SOURCES})

set(REST_SERVICE_SOURCES RestService.cpp)
set(REST_SERVICE_EXE amm_rest_service)
add_executable(${REST_SERVICE_EXE} ${REST_SERVICE_SOURCES})
target_include_directories(${REST_SERVICE_EXE} PUBLIC ${RAPIDJSON_INCLUDES})

set(SPI_SOURCES spi_proto/src/crc16.c spi_proto/src/spi_proto.c spi_proto/src/spi_proto.h)
set(SPI_REMOTE_SOURCES spi_proto/src/spi_remote_host.c spi_proto/src/binary_semaphore.c spi_proto/src/spi_proto_master.cpp spi_proto/src/spi_chunks.cpp)
set(SPI_DATAGRAM_SOURCES spi_proto/src/spi_proto_master_datagram.cpp)

set(HEART_RATE_LED_SOURCES HeartRateLED.cpp)
set(HEART_RATE_LED_EXE amm_heartrate_led)

set(CHEST_RISE_SOURCES ChestRise.cpp)
set(CHEST_RISE_EXE amm_chest_rise)

set(FLUID_MANAGER_SOURCES FluidManager.cpp)
set(FLUID_MANAGER_EXE amm_fluid_manager)

set(IVC_MODULE_SOURCES IVC.cpp)
set(IVC_MODULE_EXE amm_ivc_module)

set(ARDUINO_BRIDGE_SOURCES Arduino_Bridge.cpp AMM/Utility.cpp)
set(ARDUINO_BRIDGE_EXE amm_arduino_bridge)


if (NOT MSVC)
    add_executable(${HEART_RATE_LED_EXE} ${HEART_RATE_LED_SOURCES} ${SPI_SOURCES} ${SPI_DATAGRAM_SOURCES})
    target_include_directories(${HEART_RATE_LED_EXE} PUBLIC spi_proto/src)
    add_executable(${CHEST_RISE_EXE} ${CHEST_RISE_SOURCES} ${SPI_SOURCES} ${SPI_DATAGRAM_SOURCES})
    target_include_directories(${CHEST_RISE_EXE} PUBLIC spi_proto/src)
    add_executable(${FLUID_MANAGER_EXE} ${FLUID_MANAGER_SOURCES} ${SPI_SOURCES} ${SPI_REMOTE_SOURCES})
    target_include_directories(${FLUID_MANAGER_EXE} PUBLIC spi_proto/src)
    add_executable(${IVC_MODULE_EXE} ${IVC_MODULE_SOURCES} ${SPI_SOURCES} ${SPI_REMOTE_SOURCES})
    target_include_directories(${IVC_MODULE_EXE} PUBLIC spi_proto/src)
    add_executable(${ARDUINO_BRIDGE_EXE} ${ARDUINO_BRIDGE_SOURCES})


endif ()

set(TCP_BRIDGE_SOURCES TCP_Bridge.cpp AMM/Utility.cpp Net/Server.cpp Net/ServerThread.cpp Net/Client.cpp Net/UdpDiscoveryServer.cpp)
set(TCP_BRIDGE_EXE amm_tcp_bridge)
add_executable(${TCP_BRIDGE_EXE} ${TCP_BRIDGE_SOURCES})

set(MODULE_MANAGER_SOURCES ModuleManager.cpp AMM/ModuleManager.cpp AMM/Utility.cpp AMM/Listeners/ModuleManagerListener.cpp)
set(MODULE_MANAGER_EXE amm_module_manager)
add_executable(${MODULE_MANAGER_EXE} ${MODULE_MANAGER_SOURCES})
target_include_directories(${MODULE_MANAGER_EXE} PUBLIC ${SQLITE3_INCLUDE_DIR} ${TinyXML2_INCLUDE_DIRS})
target_link_libraries(${MODULE_MANAGER_EXE}
        fastrtps fastcdr
        amm_base_logger
        Threads::Threads
        ${SQLITE3_LIBRARIES}
        amm_types
        )

set(REST_ADAPTER_SOURCES REST_Adapter.cpp AMM/Utility.cpp Net/UdpDiscoveryServer.cpp)
set(REST_ADAPTER_EXE amm_rest_adapter)

if (Pistache_FOUND)
    add_executable(${REST_ADAPTER_EXE} ${REST_ADAPTER_SOURCES})
    target_include_directories(${REST_ADAPTER_EXE} PUBLIC ${SQLITE3_INCLUDE_DIR} ${TinyXML2_INCLUDE_DIRS})
    target_link_libraries(${REST_ADAPTER_EXE}
            fastrtps fastcdr
            amm_base_logger
            ${RAPIDJSON_LIBRARIES}
            ${Pistache_LIBRARY}
            Boost::system Boost::filesystem Boost::iostreams
            Threads::Threads
            ${SQLITE3_LIBRARIES}
            amm_types
            )
endif ()


if (Biogears_FOUND)
    set(PHYSIOLOGY_MANAGER_SOURCES PhysiologyManager.cpp AMM/Utility.cpp AMM/PhysiologyEngineManager.cpp AMM/PhysiologyThread.cpp)
    set(PHYSIOLOGY_MANAGER_EXE amm_physiology_manager)
    add_executable(${PHYSIOLOGY_MANAGER_EXE} ${PHYSIOLOGY_MANAGER_SOURCES})
    add_dependencies(${PHYSIOLOGY_MANAGER_EXE} stage_biogears_schema stage_biogears_data)
    target_link_libraries(${PHYSIOLOGY_MANAGER_EXE}
            fastrtps fastcdr
            amm_base_logger
            Biogears::libbiogears
            Threads::Threads
            Boost::system
            amm_types
            )
endif ()

target_link_libraries(${SIM_MANAGER_EXE}
        fastrtps fastcdr
        amm_base_logger
        Threads::Threads
        amm_types
        )

target_link_libraries(${VIRTUAL_EQUIPMENT_EXE}
        fastrtps fastcdr
        amm_types
        )


target_link_libraries(${COMMAND_EXECUTOR_EXE}
        fastrtps fastcdr
        amm_types
        )

target_link_libraries(${REST_SERVICE_EXE}
        fastrtps fastcdr
        ${RAPIDJSON_LIBRARIES}
        amm_base_logger
        amm_types
        )
target_link_libraries(${REST_SERVICE_EXE} ${Boost_LIBRARIES})
target_link_libraries(${REST_SERVICE_EXE} ${CMAKE_THREAD_LIBS_INIT})

if (NOT MSVC)
    target_link_libraries(${HEART_RATE_LED_EXE}
            fastrtps fastcdr
            amm_base_logger
            amm_types
            )

    target_link_libraries(${CHEST_RISE_EXE}
            fastrtps fastcdr
            amm_base_logger
            amm_types
            )

    target_link_libraries(${FLUID_MANAGER_EXE}
            fastrtps fastcdr
            amm_base_logger
            amm_types
            )

    target_link_libraries(${IVC_MODULE_EXE}
            fastrtps fastcdr
            amm_base_logger
            amm_types
            )

    target_link_libraries(${ARDUINO_BRIDGE_EXE}
            fastrtps fastcdr
            amm_base_logger
            Threads::Threads
            pthread
            Boost::system
            amm_types
            )
endif ()

target_link_libraries(${TCP_BRIDGE_EXE}
        fastrtps fastcdr
        amm_base_logger
        Boost::system
        Threads::Threads
        amm_types
        )

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
