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

#ifndef __SensorHubAndroid_h__
#define __SensorHubAndroid_h__

#include "SensorHub.h"

#include <QtCore/QMap>

#include <QAndroidJniObject>

class SensorHubAndroid : public SensorHub
{
	Q_OBJECT

public:
	SensorHubAndroid(const QAndroidJniObject& obj, const QString& hwName,
		const QString& mac, int rssi, QObject *parent = 0);
	virtual ~SensorHubAndroid();

	virtual void Connect();
	virtual void Disconnect();
	virtual bool IsConnected() const;

	virtual void Write(uint16_t service, uint16_t characteristic,
		const QByteArray& data);
	virtual void Read(uint16_t service, uint16_t characteristic);
	virtual void SetNotify(uint16_t service, uint16_t characteristic);

	void DoRecv(uint16_t characteristic, const QByteArray& data);

signals:
	void QueuedRecv(uint16_t characteristic, const QByteArray& data);

private:
	QAndroidJniObject mAndroidHub;
};

#endif // __SensorHubAndroid_h__
