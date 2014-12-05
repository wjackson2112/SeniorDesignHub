/*
 * Omega Wireless Sensor Module
 * Copyright (C) 2014 Team Omega
 * Viva la Resistance
 *
 * Hardware:    Bogdan Crivin <bcrivin@uccs.edu>
 * Firmware:    William Jackson <wjackson@uccs.edu>
 * Software:    John Martin <jmarti25@uccs.edu>
 * Integration: Angela Askins <aaskins@uccs.edu>
 */

#include "DriverDigital.h"
#include "SensorHub.h"

#include <iostream>

const uint16_t OMEGA_CHAR_DIG_SERVICE = 0x5740;
const uint16_t OMEGA_CHAR_DIG_RX      = 0x5741;
const uint16_t OMEGA_CHAR_DIG_TX      = 0x5742;

void DriverDigital::Initialize(const SensorHubPtr& hub)
{
	Driver::Initialize(hub);

	Q_ASSERT(mHub);
	mHub->SetNotify(OMEGA_CHAR_DIG_SERVICE, OMEGA_CHAR_DIG_RX);

	connect(mHub.data(), SIGNAL(Recv(uint16_t, const QByteArray&)),
		this, SLOT(Recv(uint16_t, const QByteArray&)));
}

void DriverDigital::On()
{
	uint8_t val = 1;

	mHub->Write(OMEGA_CHAR_DIG_SERVICE, OMEGA_CHAR_DIG_TX,
		QByteArray((char*)&val, 1));
}

void DriverDigital::Off()
{
	uint8_t val = 0;

	mHub->Write(OMEGA_CHAR_DIG_SERVICE, OMEGA_CHAR_DIG_TX,
		QByteArray((char*)&val, 1));
}

void DriverDigital::Recv(uint16_t characteristic,
	const QByteArray& data)
{
	if(characteristic != OMEGA_CHAR_DIG_RX)
		return;

	std::cout << (data.at(1) ? "on" : "off") << std::endl;
}

DRIVER(DriverDigital)
