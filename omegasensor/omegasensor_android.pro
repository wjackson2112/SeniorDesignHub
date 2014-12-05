TEMPLATE = app

QT += widgets androidextras

INCLUDEPATH += src

SOURCES += src/main.cpp \
	src/SensorHub.cpp \
	src/SensorHubAndroid.cpp \
	src/SensorHubConfig.cpp \
	src/SensorHubService.cpp \
	src/SensorHubAndroidService.cpp \
	src/ThreadEmit.cpp \
 	\
	src/AddHub.cpp \
	src/Config.cpp \
	src/HubList.cpp \
	src/HubListModel.cpp \
 	\
	src/Driver.cpp \
	src/DriverDemo.cpp \
	src/DriverDemoModel.cpp \
	src/DriverDemoView.cpp \
	src/DriverUART.cpp \
	src/DriverI2C.cpp \
	src/DriverSPI.cpp \
	src/DriverAnalog.cpp \
	src/DriverDigital.cpp \
	src/Driver10DOF.cpp \
	src/Driver10DOFModel.cpp \
	src/Driver10DOFView.cpp \
 	\
	src/I2COperationChain.cpp \
 	\
	src/Compass.cpp \
	src/SimpleGraph.cpp \
	\
	src/JNI.cpp

HEADERS += src/SensorHub.h \
	src/SensorHubAndroid.h \
	src/SensorHubConfig.h \
	src/SensorHubService.h \
	src/SensorHubAndroidService.h \
	src/ThreadEmit.h \
	\
	src/AddHub.h \
	src/Config.h \
	src/HubList.h \
	src/HubListModel.h \
	\
	src/Driver.h \
	src/DriverDemo.h \
	src/DriverDemoModel.h \
	src/DriverDemoView.h \
	src/DriverUART.h \
	src/DriverI2C.h \
	src/DriverSPI.h \
	src/DriverAnalog.h \
	src/DriverDigital.h \
	src/Driver10DOF.h \
	src/Driver10DOFModel.h \
	src/Driver10DOFView.h \
	\
	src/I2COperationChain.h \
	\
	src/Compass.h \
	src/SimpleGraph.h

FORMS += ui/AddHub.ui \
	ui/Config.ui \
	ui/HubList.ui \
	ui/DriverDemo.ui \
	ui/Driver10DOF.ui

RESOURCES += res/omegasensor.qrc

# Default rules for deployment.
include(deployment.pri)

OTHER_FILES += \
    android/src/edu/uccs/omegasensor/SensorHubAndroid.java \
    android/src/edu/uccs/omegasensor/JNI.java \
    android/src/edu/uccs/omegasensor/SensorHubAndroidService.java \
    android/src/edu/uccs/omegasensor/BluetoothScanRecord.java \
    android/AndroidManifest.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
