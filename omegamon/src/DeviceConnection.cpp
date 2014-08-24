#include "DeviceConnection.h"

#include <gato/gatoservice.h>

#include <iostream>
using namespace std;

DeviceConnection::DeviceConnection(const GatoAddress& addr,
	QWidget *p) : QWidget(p), mDevice(addr)
{
	ui.setupUi(this);

	connect(&mDevice, SIGNAL(connected()),
		this, SLOT(connected()));
	connect(&mDevice, SIGNAL(servicesDiscovered()),
		this, SLOT(servicesDiscovered()));

	mDevice.connectPeripheral();
}

DeviceConnection::~DeviceConnection()
{
	mDevice.disconnectPeripheral();
}

void DeviceConnection::connected()
{
	cout << "Device Connected!" << endl;
	mDevice.discoverServices();
}

void DeviceConnection::servicesDiscovered()
{
	foreach(GatoService serv, mDevice.services())
	{
		cout << serv.uuid().toString().toUtf8().constData() << endl;
	}
}
