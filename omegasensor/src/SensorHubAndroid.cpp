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

#include "SensorHubAndroid.h"
#include "SensorHubService.h"

#include "Driver.h"

#include <QAndroidJniEnvironment>

#include <QDebug>

#include <iostream>
#include <iomanip>
#include <stdint.h>

SensorHubAndroid::SensorHubAndroid(const QAndroidJniObject &obj,
	const QString& hwName, const QString& mac, int rssi, QObject *p) :
	SensorHub(rssi, p), mAndroidHub(obj)
{
	SetAddress(mac);
	SetHardwareName(hwName);

	connect(this, SIGNAL(QueuedRecv(uint16_t, const QByteArray&)),
		this, SIGNAL(Recv(uint16_t, const QByteArray&)), Qt::QueuedConnection);
}

SensorHubAndroid::~SensorHubAndroid()
{
	if(SensorHubAndroid::IsConnected())
		SensorHubAndroid::Disconnect();
}

void SensorHubAndroid::Connect()
{
	mAndroidHub.callMethod<void>("Connect");
	SensorHubService::GetSingletonPtr()->StopScan();
}

void SensorHubAndroid::Disconnect()
{
	mAndroidHub.callMethod<void>("Disconnect");
	SensorHubService::GetSingletonPtr()->StartScan();
}

bool SensorHubAndroid::IsConnected() const
{
	return mAndroidHub.callMethod<jboolean>("IsConnected");
}

void SensorHubAndroid::Write(uint16_t service, uint16_t characteristic,
	const QByteArray& data)
{
	QAndroidJniEnvironment env;
	jbyteArray jdata = env->NewByteArray(data.count());

	jbyte *buffer = new jbyte[data.count()];
	for(int i = 0; i < data.count(); i++)
		buffer[i] = data.at(i);

	env->SetByteArrayRegion(jdata, 0, data.count(), buffer);

	mAndroidHub.callMethod<void>("Write", "(II[B)V",
		service, characteristic, jdata);

	env->DeleteLocalRef(jdata);
	delete[] buffer;

	UpdateLastSeen();
}

void SensorHubAndroid::Read(uint16_t service, uint16_t characteristic)
{
	mAndroidHub.callMethod<void>("Read", "(II)V",
		service, characteristic);
}

void SensorHubAndroid::SetNotify(uint16_t service, uint16_t characteristic)
{
	mAndroidHub.callMethod<void>("SetNotify", "(II)V",
		service, characteristic);
	UpdateLastSeen();
}

void SensorHubAndroid::DoRecv(uint16_t characteristic, const QByteArray& data)
{
	emit QueuedRecv(characteristic, data);
}
