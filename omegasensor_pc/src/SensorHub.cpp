#include "SensorHub.h"
#include "Driver.h"

#include <gato/gatoservice.h>
#include <gato/gatoperipheral.h>
#include <gato/gatodescriptor.h>
#include <gato/gatocharacteristic.h>

#include <iostream>
#include <iomanip>
#include <stdint.h>

const uint16_t OMEGA_CHAR_UART_SERVICE = 0x3740;
const uint16_t OMEGA_CHAR_UART_RX      = 0x3741;
const uint16_t OMEGA_CHAR_UART_TX      = 0x3742;
const uint16_t OMEGA_CHAR_UART_CONFIG  = 0x3743;

SensorHub::SensorHub(GatoPeripheral *peripheral, int rssi,
	QObject *p) : QObject(p), mRSSI(rssi), mPeripheral(peripheral)
{
	// Sanity check.
	Q_ASSERT(peripheral);

	mTx = new GatoCharacteristic();
	mTx->setUuid(GatoUUID(OMEGA_CHAR_UART_TX));
	mTx->setValueHandle(GatoHandle(OMEGA_CHAR_UART_TX));

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
		const QByteArray&)), this, SIGNAL(Read(const GatoCharacteristic&,
		const QByteArray&)));
}

SensorHub::~SensorHub()
{
	Q_ASSERT(mPeripheral);
	if(mPeripheral->state() != GatoPeripheral::StateDisconnected)
		Disconnect();
}

int SensorHub::GetRSSI() const
{
	return mRSSI;
}

void SensorHub::SetRSSI(int rssi)
{
	mRSSI = rssi;
}

QString SensorHub::HardwareName() const
{
	Q_ASSERT(mPeripheral);
	return mPeripheral->name();
}

void SensorHub::Connect()
{
	Q_ASSERT(mPeripheral);
	return mPeripheral->connectPeripheral();
}

void SensorHub::Disconnect()
{
	Q_ASSERT(mPeripheral);
	return mPeripheral->disconnectPeripheral();
}

void SensorHub::Connected()
{
	// Discover all services.
	Q_ASSERT(mPeripheral);
	mPeripheral->discoverServices();
}

void SensorHub::Write(const GatoCharacteristic& characteristic,
	const QByteArray& data)
{
	Q_ASSERT(mPeripheral);
	mPeripheral->writeValue(characteristic, data);
}

void SensorHub::SetNotify(const GatoCharacteristic& characteristic)
{
	// Unlike Android, this will write descriptor 0x2902.
	Q_ASSERT(mPeripheral);
	mPeripheral->setNotification(characteristic, true);
}

void SensorHub::ServicesDiscovered()
{
	Q_ASSERT(mPeripheral);
	foreach(GatoService serv, mPeripheral->services())
	{
		mCharacteristicsDiscovered[serv.uuid()] = false;
		mPeripheral->discoverCharacteristics(serv);
	}
}

#include "DriverDemo.h"
#include <QtGui/QWidget>

typedef Driver* (*DriverInitializer)();

#undef DRIVER
#define DRIVER(name) Driver* Construct##name(); \
	QString GetName##name();
#include "DriverList.h"



void SensorHub::CharacteristicsDiscovered(const GatoService& service)
{
	mCharacteristicsDiscovered[service.uuid()] = true;

	foreach(GatoCharacteristic characteristic, service.characteristics())
		mPeripheral->discoverDescriptors(characteristic);

	foreach(bool charOK, mCharacteristicsDiscovered)
	{
		if(!charOK)
			return;
	}

	std::cout << "All done!" << std::endl;

	QMap<QString, DriverInitializer> mDriverList;
	#undef DRIVER
	#define DRIVER(name) mDriverList[GetName##name()] = &Construct##name;
	#include "DriverList.h"

	/*foreach(DriverInitializer dint, mDriverList)
	{
		std::cout << dint()->Initialize(this).toUtf8().constData() << std::endl;
	}*/

	DriverDemo *drv = dynamic_cast<DriverDemo*>(
		mDriverList.value("UART Demo")());
	drv->Initialize(this);
	drv->CreateView()->show();
}

void SensorHub::DescriptorsDiscovered(const GatoCharacteristic& characteristic)
{
	/*
	foreach(GatoDescriptor desc, characteristic.descriptors())
	{
		std::cout << "descriptor=" << desc.uuid().toString().toUtf8().constData()
			<< std::endl;
	}
	*/
}

GatoCharacteristic SensorHub::CharacteristicByUUID(
	uint16_t service, uint16_t characteristic) const
{
	Q_ASSERT(mPeripheral);

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
