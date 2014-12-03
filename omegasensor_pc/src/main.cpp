#include "HubList.h"

#ifdef Q_OS_ANDROID
#include "SensorHubAndroidService.h"
#else // Q_OS_LINUX
#include "SensorHubLinuxService.h"
#endif // Q_OS_ANDROID

#include <QApplication>
#include <QMetaType>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setOrganizationName("Omega Team");
	app.setOrganizationDomain("omegateam");
	app.setApplicationName("Omega Sensor Hub Client");

	qRegisterMetaType<uint16_t>("uint16_t");

	// Create and display the main window.
	//MainWindow *mainWindow = new MainWindow;
	//mainWindow->show();

#ifdef Q_OS_ANDROID
	SensorHubAndroidService serv;
#else // Q_OS_LINUX
	SensorHubLinuxService serv;
#endif // Q_OS_ANDROID

	HubList *list = new HubList;
	list->show();

	// Run the main application event loop.
	int ret = app.exec();

	// Free the MainWindow object.
	//delete mainWindow;

	return ret;
}
