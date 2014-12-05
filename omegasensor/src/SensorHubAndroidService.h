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

#ifndef __SensorHubAndroidService_h__
#define __SensorHubAndroidService_h__

#include "SensorHubService.h"

#include <QtCore/QHash>

#include <QAndroidJniObject>

class ThreadEmit;
class SensorHubAndroid;

class SensorHubAndroidService : public SensorHubService
{
	Q_OBJECT

public:
	SensorHubAndroidService(QObject *parent = 0);
	virtual ~SensorHubAndroidService();

	void AddHub(const QAndroidJniObject &obj, const QString &hwName,
		const QString &mac, int rssi);
	void UpdateHub(const QString& mac, int rssi);
	void LoadDrivers(const QString& mac);
	void CreateEmitters();
	void HaveData(const QString& mac, int characteristic,
		const QByteArray& data);

public slots:
	virtual void StartScan();
	virtual void StopScan();
	virtual void Reload();

private:
	QAndroidJniObject mAndroidService;

	QHash<QString, ThreadEmit*> mEmitters;
};

#endif // __SensorHubAndroidService_h__
