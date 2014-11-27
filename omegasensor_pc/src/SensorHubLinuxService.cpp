#include "SensorHubLinuxService.h"
#include "SensorHubLinux.h"

#include <QtCore/QStringList>

#include <gato/gatoperipheral.h>
#include <gato/gatoservice.h>

#include <iostream>

SensorHubLinuxService::SensorHubLinuxService(QObject *p) : SensorHubService(p)
{
	mManager = new GatoCentralManager;

	// Connect the manager signals.
	connect(mManager, SIGNAL(discoveredPeripheral(GatoPeripheral*,
		quint8, int)), this, SLOT(DiscoveredPeripheral(GatoPeripheral*,
		quint8, int)));
}

SensorHubLinuxService::~SensorHubLinuxService()
{
	// If we don't stop the scan it may mess up some systems (Linux).
	StopScan();
}

void SensorHubLinuxService::StartScan()
{
	// Start scanning for bluetooth devices.
	mManager->scanForPeripherals();
}

void SensorHubLinuxService::Reload()
{
	delete mManager;
	mManager = new GatoCentralManager;

	// Connect the manager signals.
	connect(mManager, SIGNAL(discoveredPeripheral(GatoPeripheral*,
		quint8, int)), this, SLOT(DiscoveredPeripheral(GatoPeripheral*,
		quint8, int)));
}

void SensorHubLinuxService::StopScan()
{
	// Stop scanning for bluetooth devices.
	mManager->stopScan();
}

void SensorHubLinuxService::DiscoveredPeripheral(GatoPeripheral *peripheral,
	quint8 advertType, int rssi)
{
	// Filter the devices by name.
	if(!OMEGA_NAME_REGEX.exactMatch(peripheral->name()))
		return;

	std::cout << peripheral->name().toUtf8().constData() << std::endl;

	// We get to hand parse this because it doesn't work right. Yay!
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
	//if(!services.contains(OMEGA_SERVICE))
		//return;

	// Get the hardware address of the hub for lookup.
	QString mac = peripheral->address().toString();

	// Either update or add the hub.
	if(mSensorHubs.contains(mac))
	{
		// Get the hub by it's hardware address.
		SensorHubPtr hub = mSensorHubs.value(mac);

		if(!hub.isNull()) // Should never be null.
		{
			// Update the RSSI for the sensor hub.
			hub->SetRSSI(rssi);

			// Notify that the hub was updated.
			emit SensorHubChanged(hub);
		}
	}
	else
	{
		// Create the sensor hub object.
		SensorHubPtr hub(new SensorHubLinux(peripheral, rssi));

		// Store the hub.
		mSensorHubs[mac] = hub;

		// Connect to the hub.
		//hub->Connect();

		// Notify that the hub was added.
		emit SensorHubDiscovered(hub);
	}

	// Print out some debug info about the sensor hub.
	std::cout << "Name: " << peripheral->name().toUtf8().constData()
		<< " | MAC: " << peripheral->address().toString().toUtf8().constData()
		<< " | RSSI: " << rssi << " dBm" << std::endl;
	std::cout << "  Services: "
		<< uuids.join(", ").toUtf8().constData() << std::endl;
}
