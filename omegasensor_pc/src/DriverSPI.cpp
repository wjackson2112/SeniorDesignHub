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

#include "DriverSPI.h"
#include "SensorHub.h"

#include <iostream>

const uint16_t OMEGA_CHAR_SPI_SERVICE = 0x6740;
const uint16_t OMEGA_CHAR_SPI_RX      = 0x6741;
const uint16_t OMEGA_CHAR_SPI_TX      = 0x6742;
const uint16_t OMEGA_CHAR_SPI_CONFIG  = 0x6743;

void DriverSPI::Initialize(const SensorHubPtr& hub)
{
	Driver::Initialize(hub);

	Q_ASSERT(mHub);
	mHub->SetNotify(OMEGA_CHAR_SPI_SERVICE, OMEGA_CHAR_SPI_RX);

	connect(mHub.data(), SIGNAL(Recv(uint16_t, const QByteArray&)),
		this, SLOT(Recv(uint16_t, const QByteArray&)));
}

void DriverSPI::Send(const QByteArray& data)
{
	mHub->Write(OMEGA_CHAR_SPI_SERVICE, OMEGA_CHAR_SPI_TX, data);
}

void DriverSPI::Recv(uint16_t characteristic,
	const QByteArray& data)
{
	if(characteristic != OMEGA_CHAR_SPI_RX)
		return;

	std::cout << data.constData() << std::endl;
}

DRIVER(DriverSPI)
