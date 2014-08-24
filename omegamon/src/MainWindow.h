#ifndef __MainWindow_h__
#define __MainWindow_h__

#include "ui_MainWindow.h"

#include "DiscoveredDeviceModel.h"

#include <QtCore/QMap>

#include <gato/gatocentralmanager.h>

class DeviceConnection;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

protected slots:
	void discoveredPeripheral(GatoPeripheral *peripheral,
		quint8 advertType, int rssi);
	void itemDoubleClicked(const QModelIndex& index);
	void toggleDiscovery();
	void selectDevice();

protected:
	GatoCentralManager mManager;
	DiscoveredDeviceModel mModel;
	bool mScanning;

	QHash<QString, DeviceConnection*> mConnections;

	Ui::MainWindow ui;
};

#endif // __MainWindow_h__
