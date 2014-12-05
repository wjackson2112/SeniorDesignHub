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

#ifndef __SensorHubLinuxService_h__
#define __SensorHubLinuxService_h__

#include "SensorHubService.h"

#include <gato/gatocentralmanager.h>

class SensorHubLinuxService : public SensorHubService
{
	Q_OBJECT

public:
	SensorHubLinuxService(QObject *parent = 0);
	virtual ~SensorHubLinuxService();

public slots:
	virtual void StartScan();
	virtual void StopScan();
	virtual void Reload();

private slots:
	void DiscoveredPeripheral(GatoPeripheral *peripheral,
		quint8 advertType, int rssi);

private:
	/// Qt wrapper around Bluez (Linux BLE).
	GatoCentralManager *mManager;
};

#endif // __SensorHubLinuxService_h__
