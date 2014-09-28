#ifndef __SensorHubService_h__
#define __SensorHubService_h__

#include <QtCore/QMap>
#include <QtCore/QObject>

#include <gato/gatocentralmanager.h>

class SensorHub;

class SensorHubService : public QObject
{
	Q_OBJECT

public:
	SensorHubService(QObject *parent = 0);

public slots:
	void StartScan();
	void StopScan();

private slots:
	void DiscoveredPeripheral(GatoPeripheral *peripheral,
		quint8 advertType, int rssi);

private:
	/// List of sensor hubs.
	QMap<QString, SensorHub*> mSensorHubs;

	/// Qt wrapper around Bluez (Linux BLE).
	GatoCentralManager mManager;
};

#endif // __SensorHubService_h__
