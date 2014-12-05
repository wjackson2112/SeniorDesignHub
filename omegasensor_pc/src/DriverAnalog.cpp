#include "DriverAnalog.h"
#include "SensorHub.h"

#include <iostream>

const uint16_t OMEGA_CHAR_ANA_SERVICE = 0x3740;
const uint16_t OMEGA_CHAR_ANA_RX      = 0x3741;

void DriverAnalog::Initialize(const SensorHubPtr& hub)
{
	Driver::Initialize(hub);

	Q_ASSERT(mHub);
	mHub->SetNotify(OMEGA_CHAR_ANA_SERVICE, OMEGA_CHAR_ANA_RX);

	connect(mHub.data(), SIGNAL(Recv(uint16_t, const QByteArray&)),
		this, SLOT(Recv(uint16_t, const QByteArray&)));
}

void DriverAnalog::Recv(uint16_t characteristic,
	const QByteArray& data)
{
	if(characteristic != OMEGA_CHAR_ANA_RX)
		return;

	std::cout << data.constData() << std::endl;
}

DRIVER(DriverAnalog)
