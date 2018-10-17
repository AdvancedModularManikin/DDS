# copy all from data to install dir /share
# here can be images, models

file(COPY ${PROJECT_SOURCE_DIR}/support/mule1 DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../bin)
file(COPY ${PROJECT_SOURCE_DIR}/support/mule2 DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../bin)

configure_file(${PROJECT_SOURCE_DIR}/support/amm_rtps.xml ${CMAKE_CURRENT_BINARY_DIR}/../bin COPYONLY)

if (Biogears_FOUND)
    file(COPY ${PROJECT_SOURCE_DIR}/support/Actions DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../bin/)
    file(COPY ${PROJECT_SOURCE_DIR}/support/config DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../bin)
    file(COPY ${PROJECT_SOURCE_DIR}/support/ecg DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../bin/)
    file(COPY ${PROJECT_SOURCE_DIR}/support/environments DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../bin/)
    file(COPY ${PROJECT_SOURCE_DIR}/support/nutrition DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../bin/)
    file(COPY ${PROJECT_SOURCE_DIR}/support/override DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../bin/)
    file(COPY ${PROJECT_SOURCE_DIR}/support/patients DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../bin/)
    file(COPY ${PROJECT_SOURCE_DIR}/support/states DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../bin/)
    file(COPY ${PROJECT_SOURCE_DIR}/support/substances DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../bin/)
    file(COPY ${PROJECT_SOURCE_DIR}/support/xsd DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../bin/)

    configure_file(${PROJECT_SOURCE_DIR}/support/log4j.properties ${CMAKE_CURRENT_BINARY_DIR}/../bin COPYONLY)
    configure_file(${PROJECT_SOURCE_DIR}/support/BioGearsConfiguration.xml ${CMAKE_CURRENT_BINARY_DIR}/../bin COPYONLY)
    configure_file(${PROJECT_SOURCE_DIR}/support/OverrideConfig.xml ${CMAKE_CURRENT_BINARY_DIR}/../bin COPYONLY)
    configure_file(${PROJECT_SOURCE_DIR}/support/UCEDefs.conf ${CMAKE_CURRENT_BINARY_DIR}/../bin COPYONLY)
endif ()

