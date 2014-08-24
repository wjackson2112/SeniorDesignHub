#include "MainWindow.h"

#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setOrganizationName("Omega Team");
	app.setOrganizationDomain("omegateam");
	app.setApplicationName("Omega Monitor");

	// Create and display the main window.
	MainWindow *mainWindow = new MainWindow;
	mainWindow->show();

	// Run the main application event loop.
	int ret = app.exec();

	// Free the MainWindow object.
	delete mainWindow;

	return ret;
}
