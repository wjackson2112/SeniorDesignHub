CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(omegasensor)

SET(QT_USE_QTCORE TRUE)
SET(QT_USE_QTGUI TRUE)

FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

MESSAGE("** Configuring ${PROJECT_NAME} **")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

# When building Windows executables, this macro packs the executable. This
# works by simply passing the same name you passed to ADD_EXECUTABLE to this
# macro. The executable will be packed in place. UPX is not used when using
# Microsoft Visual C++ or if DISABLE_UPX is defined.
MACRO(UPX_WRAP exefile)
IF(WIN32 AND NOT MSVC AND NOT DISABLE_UPX)
	# Get the path to the executable.
	GET_PROPERTY(exefile_path TARGET ${exefile} PROPERTY LOCATION)

	# Add a command to run UPX passing a compression of 9 and the path to
	# the target executable.
	ADD_CUSTOM_COMMAND(TARGET ${exefile} POST_BUILD
		COMMAND upx -9 ${exefile_path} 1> nul 2>&1)
ENDIF(WIN32 AND NOT MSVC AND NOT DISABLE_UPX)
ENDMACRO(UPX_WRAP exefile)

# If we are using mingw and the path to windres is not set, add a default path.
IF(MINGW AND NOT CMAKE_WINDRES_PATH)
	SET(CMAKE_WINDRES_PATH windres.exe)
ENDIF(MINGW AND NOT CMAKE_WINDRES_PATH)

# This macro is used to compile Windows resource files for either Microsoft
# Visual C++ or MinGW. Simply pass the name of the output variable followed
# by a list of resource file paths. The output variable will be filled and
# should then be passed as source files to the ADD_EXECUTABLE command.
MACRO(RES_WRAP outfiles)
IF(WIN32)
	IF(MINGW) # MinGW
		FOREACH(it ${ARGN}) # Process each resource file
			# Get the name of the file (without the extension) and the path
			# to the file. These are needed for the custom command.
			GET_FILENAME_COMPONENT(fn ${it} NAME_WE)
			GET_FILENAME_COMPONENT(fp ${it} PATH)

			# This command calls windres with the resource file and outputs
			# an object file with the _res.o suffix. This object file is then
			# linked to the executable (by adding the object file to the output
			# variable). The object file depends on the resource file.
			ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${fn}_res.o
				COMMAND ${CMAKE_WINDRES_PATH}
				-I${CMAKE_CURRENT_SOURCE_DIR}/${fp}
				-i${CMAKE_CURRENT_SOURCE_DIR}/${it}
				-o ${CMAKE_CURRENT_BINARY_DIR}/${fn}_res.o
				DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${it})

			# Add the object file to the list of output files that will be
			# added to the ADD_EXECUTABLE command (and thus linked to the app).
			SET(${outfiles} ${${outfiles}}
				${CMAKE_CURRENT_BINARY_DIR}/${fn}_res.o)
		ENDFOREACH(it ${ARGN})
	ELSE(MINGW) # Microsoft Visual C++
		FOREACH(it ${ARGN}) # Process each resource file
			# Simply add the resource file to the output variable and let cmake
			# handle it for us.
			SET(${outfiles} ${${outfiles}}
				${CMAKE_CURRENT_SOURCE_DIR}/${it})
		ENDFOREACH(it ${ARGN})
	ENDIF(MINGW)
ENDIF(WIN32)
ENDMACRO(RES_WRAP outfiles)

IF(CMAKE_COMPILER_IS_GNUCXX)
	ADD_DEFINITIONS(-std=gnu++0x)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

SET(${PROJECT_NAME}_SRCS
	src/main.cpp
	src/SensorHub.cpp
	src/SensorHubConfig.cpp
	src/SensorHubLinux.cpp
	src/SensorHubService.cpp
	src/SensorHubLinuxService.cpp

	src/AddHub.cpp
	src/Config.cpp
	src/HubList.cpp
	src/HubListModel.cpp

	src/DriverDemo.cpp
	src/DriverDemoModel.cpp
	src/DriverDemoView.cpp
	src/DriverUART.cpp
	src/DriverI2C.cpp
	src/Driver10DOF.cpp
	src/Driver10DOFModel.cpp
	src/Driver10DOFView.cpp

	src/SimpleGraph.cpp
)

SET(${PROJECT_NAME}_HDRS
	src/SensorHub.h
	src/SensorHubConfig.h
	src/SensorHubLinux.h
	src/SensorHubService.h
	src/SensorHubLinuxService.h

	src/AddHub.h
	src/Config.h
	src/HubList.h
	src/HubListModel.h

	src/Driver.h
	src/DriverDemo.h
	src/DriverDemoModel.h
	src/DriverDemoView.h
	src/DriverUART.h
	src/DriverI2C.h
	src/Driver10DOF.h
	src/Driver10DOFModel.h
	src/Driver10DOFView.h

	src/SimpleGraph.h
)
QT4_WRAP_CPP(${PROJECT_NAME}_MOC_SRCS ${${PROJECT_NAME}_HDRS})

SET(${PROJECT_NAME}_HDRS
	src/SensorHub.h
	src/SensorHubService.h

	src/Driver.h
	src/DriverDemo.h
	src/DriverDemoModel.h
	src/DriverDemoView.h
	src/DriverUART.h

	src/SimpleGraph.h
)

SET(${PROJECT_NAME}_RCC
	res/omegasensor.qrc
)
QT4_ADD_RESOURCES(${PROJECT_NAME}_RCC_SRCS ${${PROJECT_NAME}_RCC})

SET(${PROJECT_NAME}_UIS
	ui/AddHub.ui
	ui/Config.ui
	ui/HubList.ui
	ui/DriverDemo.ui
	ui/Driver10DOF.ui
)
QT4_WRAP_UI(${PROJECT_NAME}_UI_SRCS ${${PROJECT_NAME}_UIS})

RES_WRAP(${PROJECT_NAME}_SRCS res/omegasensor.rc)

ADD_EXECUTABLE(${PROJECT_NAME} WIN32 ${${PROJECT_NAME}_SRCS}
	${${PROJECT_NAME}_HDRS} ${${PROJECT_NAME}_RCC_SRCS}
	${${PROJECT_NAME}_MOC_SRCS} ${${PROJECT_NAME}_UI_SRCS})

IF(MINGW)
	SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS -mwindows)
ENDIF(MINGW)

UPX_WRAP(${PROJECT_NAME})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${QT_LIBRARIES}
	${QT_QTMAIN_LIBRARY} gato bluetooth)

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION bin)

IF(NOT WIN32)
	INSTALL(FILES res/${PROJECT_NAME}.desktop DESTINATION share/applications)
ENDIF(NOT WIN32)
