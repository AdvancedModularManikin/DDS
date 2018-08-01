SET(BIOGEARS_HOME $ENV{BIOGEARS_HOME})

SET(BioGears_INCLUDE_DIRS
        $ENV{BIOGEARS_HOME}/library/include
        /usr/include/eigen3
        )

# Find libraries
FIND_LIBRARY(ENGINE_LIBRARY
        NAMES
        libbiogears.dll libbiogears.a libbiogears.so
        PATHS
        $ENV{BIOGEARS_HOME}/library/lib
        )

FIND_LIBRARY(CDM_LIBRARY
        NAMES
        libbiogears_cdm.dll libbiogears_cdm.a libbiogears_cdm.so
        PATHS
        $ENV{BIOGEARS_HOME}/library/lib
        )

FIND_LIBRARY(LOG4CPP_LIBRARY
        NAMES
        log4cpp
        )

FIND_LIBRARY(XERCES_LIBRARY
        NAMES
        xerces-c
        )

SET(BioGears_LIBRARIES
        ${ENGINE_LIBRARY}
        ${CDM_LIBRARY}
        ${XERCES_LIBRARY}
        ${LOG4CPP_LIBRARY}
        )

IF (BioGears_INCLUDE_DIRS AND BioGears_LIBRARIES)
    SET(BioGears_FOUND TRUE)
ENDIF (BioGears_INCLUDE_DIRS AND BioGears_LIBRARIES)

IF (BioGears_FOUND)
    MESSAGE(STATUS "Found BioGears libraries: ${BioGears_LIBRARIES}")
ELSE (BioGears_FOUND)
    IF (BioGears_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could not find BioGears")
    ENDIF (BioGears_FIND_REQUIRED)
ENDIF (BioGears_FOUND)

MARK_AS_ADVANCED(BioGears_INCLUDE_DIRS BioGears_LIBRARIES)

