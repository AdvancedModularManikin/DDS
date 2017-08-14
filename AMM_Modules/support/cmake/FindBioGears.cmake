SET(BIOGEARS_HOME $ENV{BIOGEARS_HOME})

SET(BioGears_INCLUDE_DIRS
	$ENV{BIOGEARS_HOME}/library/include
	$ENV{BIOGEARS_HOME}/library/include/bind
	$ENV{BIOGEARS_HOME}/library/include/cdm
	$ENV{BIOGEARS_HOME}/library/include/Eigen
)

# Find libraries
FIND_LIBRARY(ENGINE_LIBRARY
	NAMES
		BioGearsEngine.so
	PATHS
		$ENV{BIOGEARS_HOME}/library/lib/release
)

FIND_LIBRARY(CDM_LIBRARY
	NAMES
		CommonDataModel.so
	PATHS
		$ENV{BIOGEARS_HOME}/library/lib/release
)

FIND_LIBRARY(DMB_LIBRARY
	NAMES
		DataModelBindings.so
	PATHS
		$ENV{BIOGEARS_HOME}/library/lib/release
)

FIND_LIBRARY(LOG4CPP_LIBRARY
		NAMES
		log4cpp
		PATHS
		$ENV{BIOGEARS_HOME}/library/lib/release
		)

FIND_LIBRARY(XERCES_LIBRARY
		NAMES
		xerces-c
		PATHS
		$ENV{BIOGEARS_HOME}/library/lib/release
		)

SET(BioGears_LIBRARIES
		${XERCES_LIBRARY}
		${LOG4CPP_LIBRARY}
		${ENGINE_LIBRARY}
		${CDM_LIBRARY}
		${DMB_LIBRARY}
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

