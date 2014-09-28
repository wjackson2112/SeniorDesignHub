#include "SensorHubService.h"
#include "SensorHub.h"

#include <QtCore/QRegExp>
#include <QtCore/QStringList>

#include <gato/gatoperipheral.h>
#include <gato/gatoservice.h>
#include <iostream>
#include <stdint.h>

const uint16_t OMEGA_SERVICE           = 0x180F;

const QRegExp OMEGA_NAME_REGEX("SensorHub[0-9]{3}");

SensorHubService::SensorHubService(QObject *p) : QObject(p)
{
	// Connect the manager signals.
	connect(&mManager, SIGNAL(discoveredPeripheral(GatoPeripheral*,
		quint8, int)), this, SLOT(DiscoveredPeripheral(GatoPeripheral*,
		quint8, int)));
}

void SensorHubService::StartScan()
{
	// Start scanning for bluetooth devices.
	mManager.scanForPeripherals();
}

void SensorHubService::StopScan()
{
	// Stop scanning for bluetooth devices.
	mManager.stopScan();
}

void SensorHubService::DiscoveredPeripheral(GatoPeripheral *peripheral,
	quint8 advertType, int rssi)
{
	/*
	DiscoveredDevice *dev = new DiscoveredDevice(peripheral->name(),
		peripheral->address().toString(),
		peripheral->address().addressType(),
		advertType, peripheral->advertData(), rssi);
	mModel.addDevice(dev);
	*/

	// Filter the devices by name.
	if(!OMEGA_NAME_REGEX.exactMatch(peripheral->name()))
		return;

	const uint8_t *data = (uint8_t*)peripheral->advertData().constData();
	int len = peripheral->advertData().size();

	int pos = 0;

	QStringList uuids;
	QList<uint16_t> services;

	// Parse the advert data for the services.
	while(pos < (len - 2))
	{
		int item_len = data[pos++];
		if(item_len == 0)
			break;

		int type = data[pos];
		if((pos + item_len) > len)
			break;

		pos++;

		switch(type)
		{
			case 0x03: // 16-bit Service UUIDs
			{
				uint8_t count = (item_len - 1) / 2;

				for(uint8_t i = 0; i < count; i++)
				{
					uint16_t uuid = data[pos + i * 2] |
						(data[pos + 1 + i * 2] << 8);
					services.append(uuid);
					uuids.append(tr("0x%1").arg(uuid, 4, 16,
						QLatin1Char('0')).toUpper());
				}

				break;
			}
			default:
			{
				break;
			}
		}

		pos += item_len - 1;
	}

	// Make sure this is an omega sensor hub.
	if(!services.contains(OMEGA_SERVICE))
		return;

	QString mac = peripheral->address().toString();
	if(mSensorHubs.contains(mac))
		mSensorHubs.value(mac)->SetRSSI(rssi);
	else
	{
		mSensorHubs[mac] = new SensorHub(peripheral, rssi);
		mSensorHubs[mac]->Connect();
	}

	std::cout << "Name: " << peripheral->name().toUtf8().constData()
		<< " | MAC: " << peripheral->address().toString().toUtf8().constData()
		<< " | RSSI: " << rssi << " dBm" << std::endl;
	std::cout << "  Services: "
		<< uuids.join(", ").toUtf8().constData() << std::endl;
}
