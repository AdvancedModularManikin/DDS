# copy all from data to install dir /share
# here can be images, models

file(COPY ${PROJECT_SOURCE_DIR}/support/mule1 DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../bin)
file(COPY ${PROJECT_SOURCE_DIR}/support/mule2 DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../bin)

configure_file(${PROJECT_SOURCE_DIR}/support/amm_rtps.xml ${CMAKE_BINARY_DIR}/bin COPYONLY)

if (Biogears_FOUND)
    file(COPY ${PROJECT_SOURCE_DIR}/support/Actions DESTINATION ${CMAKE_BINARY_DIR}/bin/)
    stage_biogears_schema(DESTINATION ${CMAKE_BINARY_DIR}/bin/)
    stage_biogears_data(DESTINATION ${CMAKE_BINARY_DIR}/bin/)
endif ()

