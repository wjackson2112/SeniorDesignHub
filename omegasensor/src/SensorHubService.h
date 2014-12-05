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

#ifndef __SensorHubService_h__
#define __SensorHubService_h__

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QRegExp>

#include <stdint.h>

#include "SensorHub.h"
#include "SensorHubConfig.h"
#include "Driver.h"

/// The service UUID for a sensor hub.
const uint16_t OMEGA_SERVICE = 0x180F;

/// The pattern the hardware name of a sensor hub should match.
//const QRegExp OMEGA_NAME_REGEX("SensorHub[0-9]{3}");
const QRegExp OMEGA_NAME_REGEX("Hub[0-9]");

/// Used to create a new instance of a specific driver.
typedef Driver* (*DriverInitializer)();

/**
 * The sensor hub service finds and manages the list of sensor hubs you may
 * communicate with. You must create an instance of the platform specific
 * service at the beginning of your application.
 */
class SensorHubService : public QObject
{
	Q_OBJECT

public:
	SensorHubService(QObject *parent = 0);
	virtual ~SensorHubService();

	/**
	 * Get the sensor hub service.
	 * @returns A pointer to the sensor hub service. Don't delete it!
	 */
	static SensorHubService* GetSingletonPtr();

	/**
	 * Determine if a sensor hub has been registered on this system.
	 * @param hub The sensor hub to check.
	 * @returns true if the hub is registered, false otherwise.
	 */
	bool SensorHubIsRegistered(const SensorHubPtr& hub) const;

	/**
	 * Get the sensor hub configuration for the given registered sensor.
	 * @param hub The hardware address of the sensor hub to get the
	 *   configuration for.
	 * @returns The configuration for the sensor hub.
	 */
	SensorHubConfigPtr GetSensorHubConfig(const QString& addr) const;

	/**
	 * Get the sensor hub configuration for the given registered sensor.
	 * @param hub The sensor hub to get the configuration for.
	 * @returns The configuration for the sensor hub.
	 */
	SensorHubConfigPtr GetSensorHubConfig(const SensorHubPtr& hub) const;

public slots:
	/**
	 * Start looking for sensor hubs.
	 */
	virtual void StartScan() = 0;

	/**
	 * Stop looking for sensor hubs.
	 */
	virtual void StopScan() = 0;

	/**
	 * Restart the service.
	 */
	virtual void Reload() = 0;

	/**
	 * Load the driver for the sensor hub at the given address.
	 */
	void LoadDriver(const QString& addr, const QString& driver);

	void RegisterHub(const SensorHubPtr& hub);
	void UnRegisterHub(const SensorHubPtr& hub);

	void LoadConfig();
	void SaveConfig();

	Driver::DriverType GetDriverType(const QString& driver) const;
	QMultiMap<Driver::DriverType, QString> DriversByType() const;

signals:
	/**
	 * A sensor hub has changed (most likely the RSSI value).
	 * @param hub The SensorHub object for the hub that was updated.
	 */
	void SensorHubChanged(const SensorHubPtr& hub);

	/**
	 * A sensor hub has been discovered.
	 * @param hub The SensorHub object for the hub that was discovered.
	 */
	void SensorHubDiscovered(const SensorHubPtr& hub);

protected:
	void GenerateDriverList();

	/// List of sensor hubs.
	QMap<QString, SensorHubPtr> mSensorHubs;
	QMap<QString, SensorHubConfigPtr> mRegisteredHubs;

	/// List of the installed drivers.
	QMap<QString, DriverInitializer> mDriverList;

	/// List of all drivers by their type.
	QMultiMap<Driver::DriverType, QString> mDriversByType;
};

#endif // __SensorHubService_h__
