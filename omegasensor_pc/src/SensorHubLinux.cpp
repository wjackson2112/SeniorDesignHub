#include "SensorHubLinux.h"
#include "SensorHubService.h"

#include "Driver.h"

#include <gato/gatoservice.h>
#include <gato/gatoperipheral.h>
#include <gato/gatodescriptor.h>
#include <gato/gatocharacteristic.h>

#include <iostream>
#include <iomanip>
#include <stdint.h>

SensorHubLinux::SensorHubLinux(GatoPeripheral *peripheral, int rssi,
	QObject *p) : SensorHub(rssi, p), mPeripheral(peripheral)
{
	// Sanity check.
	Q_ASSERT(peripheral);

	// Signals from gato for BLE communication.
	connect(peripheral, SIGNAL(connected()), this, SLOT(Connected()));
	connect(peripheral, SIGNAL(servicesDiscovered()),
		this, SLOT(ServicesDiscovered()));
	connect(peripheral, SIGNAL(characteristicsDiscovered(const GatoService&)),
		this, SLOT(CharacteristicsDiscovered(const GatoService&)));
	connect(peripheral, SIGNAL(descriptorsDiscovered(
		const GatoCharacteristic&)), this, SLOT(DescriptorsDiscovered(
		const GatoCharacteristic&)));
	connect(peripheral, SIGNAL(valueUpdated(const GatoCharacteristic&,
		const QByteArray&)), this, SLOT(ReadGato(const GatoCharacteristic&,
		const QByteArray&)));

	SetAddress(mPeripheral->address().toString());
	SetHardwareName(mPeripheral->name());
}

SensorHubLinux::~SensorHubLinux()
{
	Q_ASSERT(mPeripheral);
	if(mPeripheral->state() != GatoPeripheral::StateDisconnected)
		SensorHubLinux::Disconnect();
}

void SensorHubLinux::Connect()
{
	Q_ASSERT(mPeripheral);
	return mPeripheral->connectPeripheral();
}

void SensorHubLinux::Disconnect()
{
	Q_ASSERT(mPeripheral);
	return mPeripheral->disconnectPeripheral();
}

bool SensorHubLinux::IsConnected() const
{
	return mPeripheral->state() == GatoPeripheral::StateConnected;
}

void SensorHubLinux::Connected()
{
	// Discover all services.
	Q_ASSERT(mPeripheral);
	mPeripheral->discoverServices();
}

void SensorHubLinux::Write(uint16_t service, uint16_t characteristic,
	const QByteArray& data)
{
	Q_ASSERT(mPeripheral);
	mPeripheral->writeValue(CharacteristicByUUID(
		service, characteristic), data);
	UpdateLastSeen();
}

void SensorHubLinux::SetNotify(uint16_t service, uint16_t characteristic)
{
	// Unlike Android, this will write descriptor 0x2902.
	Q_ASSERT(mPeripheral);
	mPeripheral->setNotification(CharacteristicByUUID(
		service, characteristic), true);
	UpdateLastSeen();
}

void SensorHubLinux::ServicesDiscovered()
{
	Q_ASSERT(mPeripheral);
	foreach(GatoService serv, mPeripheral->services())
	{
		mCharacteristicsDiscovered[serv.uuid()] = false;
		mPeripheral->discoverCharacteristics(serv);
	}
}

void SensorHubLinux::CharacteristicsDiscovered(const GatoService& service)
{
	mCharacteristicsDiscovered[service.uuid()] = true;

	foreach(GatoCharacteristic characteristic, service.characteristics())
	{
		if(service.uuid().toUInt16() == OMEGA_SERVICE)
		{
		std::cout << characteristic.uuid().toString().toUtf8().constData() << std::endl;
		}
		mPeripheral->discoverDescriptors(characteristic);
	}

	foreach(bool charOK, mCharacteristicsDiscovered)
	{
		if(!charOK)
			return;
	}

	//std::cout << "All done!" << std::endl;

	LoadDrivers();
}

void SensorHubLinux::DescriptorsDiscovered(
	const GatoCharacteristic& characteristic)
{
	/*
	foreach(GatoDescriptor desc, characteristic.descriptors())
	{
		std::cout << "descriptor=" << desc.uuid().toString().toUtf8().constData()
			<< std::endl;
	}
	*/
}

GatoCharacteristic SensorHubLinux::CharacteristicByUUID(
	uint16_t service, uint16_t characteristic) const
{
	Q_ASSERT(mPeripheral);

	/*
	std::cout << "service=0x" << std::setfill('0') << std::setw(4)
		<< std::hex << service << std::endl;
	std::cout << "characteristic=0x" << std::setfill('0') << std::setw(4)
		<< std::hex << characteristic << std::endl;
	*/

	foreach(GatoService serv, mPeripheral->services())
	{
		/*
		std::cout << "service=" << serv.uuid().toString().toUtf8().constData()
			<< std::endl;
		std::cout << "service=0x" << std::setfill('0') << std::setw(4)
			<< std::hex << serv.uuid().toUInt16() << std::endl;
		*/

		// Check if this is the right service.
		if(serv.uuid().toUInt16() != service)
			continue;

		foreach(GatoCharacteristic gchar, serv.characteristics())
		{
			/*
			std::cout << "characteristic=0x" << std::setfill('0')
				<< std::setw(4) << std::hex << serv.uuid().toUInt16()
				<< std::endl;
			*/

			// Check if this is the right characteristic.
			if(gchar.uuid().toUInt16() == characteristic)
				return gchar;
		}
	}

	// Not found.
	return GatoCharacteristic();
}

void SensorHubLinux::ReadGato(const GatoCharacteristic& characteristic,
		const QByteArray& data)
{
	std::cout << "wtf" << std::endl;
	UpdateLastSeen();
	emit Read(characteristic.uuid().toUInt16(), data);
}
