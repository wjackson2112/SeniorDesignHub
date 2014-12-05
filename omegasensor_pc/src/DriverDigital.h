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

#ifndef __DriverDigital_h__
#define __DriverDigital_h__

#include "Driver.h"

class DriverDigital : public Driver
{
	Q_OBJECT

public:
	static QString Name() { return "Generic Digital"; }
	virtual DriverType Type() { return DriverType_Digital; }

	virtual void Initialize(const SensorHubPtr& hub);
	virtual void On();
	virtual void Off();

protected slots:
	virtual void Recv(uint16_t characteristic,
		const QByteArray& data);
};

#endif // __DriverDigital_h__
