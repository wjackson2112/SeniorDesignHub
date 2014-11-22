#include "SensorHub.h"
#include "SensorHubConfig.h"
#include "SensorHubService.h"

SensorHub::SensorHub(int rssi, QObject *p) : QObject(p), mRSSI(rssi),
	mLastSeen(QDateTime::currentDateTime())
{
	// Nothing to see here.
}

SensorHub::~SensorHub()
{
	// Nothing to see here.
}

int SensorHub::GetRSSI() const
{
	return mRSSI;
}

void SensorHub::SetRSSI(int rssi)
{
	UpdateLastSeen();
	mRSSI = rssi;
}

QString SensorHub::GetAddress() const
{
	return mAddress;
}

QString SensorHub::HardwareName() const
{
	return mHardwareName;
}

QDateTime SensorHub::LastSeen() const
{
	return mLastSeen;
}

void SensorHub::SetAddress(const QString& addr)
{
	mAddress = addr;
}

void SensorHub::SetHardwareName(const QString& name)
{
	mHardwareName = name;
}

void SensorHub::UpdateLastSeen()
{
	mLastSeen = QDateTime::currentDateTime();
}

void SensorHub::LoadDrivers()
{
	SensorHubConfigPtr config = SensorHubService::GetSingletonPtr(
		)->GetSensorHubConfig(GetAddress());
	if(config.isNull())
		return;

	// For each driver name, create a driver object and give it this
	// sensor hub (identified by it's hardware address).
	foreach(QString driver, config->DriverList())
	{
		SensorHubService::GetSingletonPtr()->LoadDriver(
			GetAddress(), driver);
	}
}
