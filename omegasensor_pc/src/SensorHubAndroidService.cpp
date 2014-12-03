#include "SensorHubAndroidService.h"
#include "SensorHubAndroid.h"
#include "ThreadEmit.h"

#include <QDebug>

#include <QtCore/QTimer>
#include <QtCore/QStringList>

#include <QApplication>

#include <QtAndroid>

#include <iostream>

SensorHubAndroidService::SensorHubAndroidService(QObject *p) : SensorHubService(p)
{
	mAndroidService = QAndroidJniObject(
		"edu/uccs/omegasensor/SensorHubAndroidService",
		"(Landroid/content/Context;)V",
		QtAndroid::androidActivity().object());
}

SensorHubAndroidService::~SensorHubAndroidService()
{
	// If we don't stop the scan it may mess up some systems (Linux).
	StopScan();
}

void SensorHubAndroidService::CreateEmitters()
{
	foreach(QString mac, mSensorHubs.keys())
	{
		if(mEmitters.contains(mac))
			continue;

		// Create an object to emit the load drivers signal.
		ThreadEmit *e = new ThreadEmit;
		connect(e, SIGNAL(triggered()), mSensorHubs.value(mac).data(),
			SLOT(LoadDrivers()), Qt::QueuedConnection);

		mEmitters[mac] = e;
	}
}

void SensorHubAndroidService::StartScan()
{
	mAndroidService.callMethod<void>("StartScan");
}

void SensorHubAndroidService::Reload()
{
	mAndroidService.callMethod<void>("Reload");
}

void SensorHubAndroidService::StopScan()
{
	mAndroidService.callMethod<void>("StopScan");
}

void SensorHubAndroidService::AddHub(const QAndroidJniObject& obj,
	const QString& hwName, const QString& mac, int rssi)
{
	SensorHubPtr hub(new SensorHubAndroid(obj, hwName, mac, rssi));
	hub->moveToThread(qApp->thread());

	// Store the hub.
	mSensorHubs[mac] = hub;

	// Notify that the hub was added.
	emit SensorHubDiscovered(hub);
}

void SensorHubAndroidService::UpdateHub(const QString& mac, int rssi)
{
	// Find the sensor hub.
	SensorHubPtr hub = mSensorHubs.value(mac);

	// Sanity check.
	if(hub.isNull())
		return;

	// Update the RSSI for the sensor hub.
	hub->SetRSSI(rssi);

	// Notify that the hub was updated.
	emit SensorHubChanged(hub);
}

void SensorHubAndroidService::HaveData(const QString& mac, int characteristic,
	const QByteArray& data)
{
	if(!mSensorHubs.contains(mac))
		return;

	mSensorHubs.value(mac).dynamicCast<SensorHubAndroid>()->DoRecv(
		characteristic, data);
}

void SensorHubAndroidService::LoadDrivers(const QString& mac)
{
	if(mEmitters.contains(mac))
		mEmitters.value(mac)->Emit();
}
