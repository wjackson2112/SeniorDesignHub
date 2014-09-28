#include "DriverUART.h"
#include "SensorHub.h"

#include <iostream>

const uint16_t OMEGA_CHAR_UART_SERVICE = 0x3740;
const uint16_t OMEGA_CHAR_UART_RX      = 0x3741;
const uint16_t OMEGA_CHAR_UART_TX      = 0x3742;
const uint16_t OMEGA_CHAR_UART_CONFIG  = 0x3743;

void DriverUART::Initialize(SensorHub *hub)
{
	Driver::Initialize(hub);

	Q_ASSERT(mHub);
	mRX = mHub->CharacteristicByUUID(OMEGA_CHAR_UART_SERVICE,
		OMEGA_CHAR_UART_RX);
	mTX = mHub->CharacteristicByUUID(OMEGA_CHAR_UART_SERVICE,
		OMEGA_CHAR_UART_TX);
	mConfig = mHub->CharacteristicByUUID(OMEGA_CHAR_UART_SERVICE,
		OMEGA_CHAR_UART_CONFIG);
	mHub->SetNotify(mRX);

	connect(mHub, SIGNAL(Read(const GatoCharacteristic&,
		const QByteArray&)), this, SLOT(Recv(const GatoCharacteristic&,
		const QByteArray&)));
}

void DriverUART::Send(const QByteArray& data)
{
	mHub->Write(mTX, data);
}

void DriverUART::Recv(const GatoCharacteristic& characteristic,
	const QByteArray& data)
{
	if(characteristic.uuid() != mRX.uuid())
		return;

	std::cout << data.constData() << std::endl;
}

DRIVER(DriverUART)
