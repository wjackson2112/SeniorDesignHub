#include "DriverUART.h"
#include "SensorHub.h"

#include <iostream>

const uint16_t OMEGA_CHAR_UART_SERVICE = 0x3740;
const uint16_t OMEGA_CHAR_UART_RX      = 0x3741;
const uint16_t OMEGA_CHAR_UART_TX      = 0x3742;
const uint16_t OMEGA_CHAR_UART_CONFIG  = 0x3743;

void DriverUART::Initialize(const SensorHubPtr& hub)
{
	Driver::Initialize(hub);

	Q_ASSERT(mHub);
	mHub->SetNotify(OMEGA_CHAR_UART_SERVICE, OMEGA_CHAR_UART_RX);

	connect(mHub.data(), SIGNAL(Recv(uint16_t, const QByteArray&)),
		this, SLOT(Recv(uint16_t, const QByteArray&)));
}

void DriverUART::Send(const QByteArray& data)
{
	mHub->Write(OMEGA_CHAR_UART_SERVICE, OMEGA_CHAR_UART_TX, data);
}

void DriverUART::Recv(uint16_t characteristic,
	const QByteArray& data)
{
	if(characteristic != OMEGA_CHAR_UART_RX)
		return;

	std::cout << data.constData() << std::endl;
}

DRIVER(DriverUART)
