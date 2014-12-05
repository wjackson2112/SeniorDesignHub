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

#ifndef __DriverUART_h__
#define __DriverUART_h__

#include "Driver.h"

class DriverUART : public Driver
{
	Q_OBJECT

public:
	static QString Name() { return "Generic UART"; }
	virtual DriverType Type() { return DriverType_UART; }

	virtual void Initialize(const SensorHubPtr& hub);
	virtual void Send(const QByteArray& data);

protected slots:
	virtual void Recv(uint16_t characteristic,
		const QByteArray& data);
};

#endif // __DriverUART_h__
