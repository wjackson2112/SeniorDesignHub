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

#ifndef __DriverDemo_h__
#define __DriverDemo_h__

#include "DriverUART.h"

class DriverDemo : public DriverUART
{
	Q_OBJECT

public:
	static QString Name();
	virtual QWidget* CreateView();
	virtual void Recv(uint16_t characteristic, const QByteArray& data);

signals:
	void ADC(int value);
};

#endif // __DriverDemo_h__
