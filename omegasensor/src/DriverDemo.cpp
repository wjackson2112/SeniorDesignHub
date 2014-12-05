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

#include "DriverDemo.h"
#include "DriverDemoView.h"
#include "DriverDemoModel.h"
#include "SensorHub.h"

const uint16_t OMEGA_CHAR_UART_RX      = 0x3741;

QString DriverDemo::Name()
{
	return "UART Demo";
}

QWidget* DriverDemo::CreateView()
{
	DriverDemoView *view = new DriverDemoView();
	DriverDemoModel *model = new DriverDemoModel(this);
	view->SetDeviceName(mHub->HardwareName());
	view->SetModel(model);

	return view;
}

void DriverDemo::Recv(uint16_t characteristic,
	const QByteArray& data)
{
	if(characteristic != OMEGA_CHAR_UART_RX)
		return;

	bool ok = false;
	int value = QString::fromUtf8(data).toUInt(&ok);
	if(ok)
		emit ADC(value);
}

DRIVER(DriverDemo)
