#include "DeviceConnection.h"

#include <gato/gatoservice.h>
#include <gato/gatocharacteristic.h>
#include <gato/gatodescriptor.h>

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
	connect(&mDevice, SIGNAL(characteristicsDiscovered(const GatoService&)),
		this, SLOT(characteristicsDiscovered(const GatoService&)));
	connect(&mDevice, SIGNAL(descriptorsDiscovered(const GatoCharacteristic&)),
		this, SLOT(descriptorsDiscovered(const GatoCharacteristic&)));

	mDevice.connectPeripheral();

	ui.deviceDetails->setModel(mModel.model());
	ui.deviceDetails->header()->hide();
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
		mModel.addService(serv);
		mDevice.discoverCharacteristics(serv);
	}
}

void DeviceConnection::characteristicsDiscovered(const GatoService& serv)
{
	mModel.addService(serv);

	foreach(GatoCharacteristic c, serv.characteristics())
		mDevice.discoverDescriptors(c);
}

void DeviceConnection::descriptorsDiscovered(
	const GatoCharacteristic& characteristic)
{
	//mModel.addService(characteristic.service());

	foreach(GatoDescriptor desc, characteristic.descriptors())
		qDebug() << "Descriptor:" << desc.uuid().toString();
}
