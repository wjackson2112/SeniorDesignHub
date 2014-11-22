#include "HubList.h"

#include <SensorHubLinuxService.h>

#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setOrganizationName("Omega Team");
	app.setOrganizationDomain("omegateam");
	app.setApplicationName("Omega Sensor Hub Client");

	// Create and display the main window.
	//MainWindow *mainWindow = new MainWindow;
	//mainWindow->show();

	SensorHubLinuxService serv;

	HubList *list = new HubList;
	list->show();

	// Run the main application event loop.
	int ret = app.exec();

	// Free the MainWindow object.
	//delete mainWindow;

	return ret;
}
