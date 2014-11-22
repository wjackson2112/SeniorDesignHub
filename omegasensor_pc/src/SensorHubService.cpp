#include "SensorHubService.h"
#include "SensorHub.h"
#include "Driver.h"

#include <QtCore/QSettings>
#include <QtCore/QMapIterator>

#include <QtGui/QWidget>

static SensorHubService *g_service = 0;

SensorHubService::SensorHubService(QObject *p) : QObject(p)
{
	// Save the singleton.
	Q_ASSERT(g_service == 0);
	g_service = this;

	// Register the metatype.
	qRegisterMetaType<SensorHubPtr>("SensorHubPtr");

	// Load the driver list.
	GenerateDriverList();

	// Load the config when the service starts.
	LoadConfig();
}

SensorHubService::~SensorHubService()
{
	// Clear the singleton.
	if(g_service == this)
		g_service = 0;
}

SensorHubService* SensorHubService::GetSingletonPtr()
{
	return g_service;
}

bool SensorHubService::SensorHubIsRegistered(const SensorHubPtr& hub) const
{
	return mRegisteredHubs.contains(hub->GetAddress());
}

void SensorHubService::RegisterHub(const SensorHubPtr& hub)
{
	if(hub.isNull())
		return;

	if(mRegisteredHubs.contains(hub->GetAddress()))
		return;

	// Create a new configuration for this hub.
	SensorHubConfigPtr config(new SensorHubConfig);
	config->SetAddress(hub->GetAddress());
	config->SetHardwareName(hub->HardwareName());
	config->SetName("Bob");

	// Save this hub to the configuration list.
	mRegisteredHubs[hub->GetAddress()] = config;
	SaveConfig();
}

void SensorHubService::UnRegisterHub(const SensorHubPtr& hub)
{
	if(hub.isNull())
		return;

	if(!mRegisteredHubs.contains(hub->GetAddress()))
		return;

	mRegisteredHubs.remove(hub->GetAddress());
	SaveConfig();
}

SensorHubConfigPtr SensorHubService::GetSensorHubConfig(
	const QString& addr) const
{
	return mRegisteredHubs.value(addr);
}

SensorHubConfigPtr SensorHubService::GetSensorHubConfig(
	const SensorHubPtr& hub) const
{
	return mRegisteredHubs.value(hub->GetAddress());
}

void SensorHubService::LoadConfig()
{
	mRegisteredHubs.clear();

	QSettings settings;

	QList<QVariant> config = settings.value("devices").toList();

	foreach(QVariant data, config)
	{
		SensorHubConfigPtr c(new SensorHubConfig(data));
		mRegisteredHubs[c->GetAddress()] = c;
	}
}

void SensorHubService::LoadDriver(const QString& addr,
	const QString& driver)
{
	// Sanity check the hub.
	SensorHubPtr hub = mSensorHubs.value(addr);
	if(hub.isNull())
		return;

	// Sanity check the driver name.
	if(!mDriverList.contains(driver))
		return;

	// TODO: Somehow free this at the right time.
	Driver *drv = mDriverList.value(driver)();
	drv->Initialize(hub);

	QWidget *view = drv->CreateView();
	if(view)
		view->show();
}

Driver::DriverType SensorHubService::GetDriverType(
	const QString& driver) const
{
	// Sanity check the driver name.
	if(!mDriverList.contains(driver))
		return Driver::DriverType_Unknown;

	Driver *drv = mDriverList.value(driver)();
	if(!drv)
		return Driver::DriverType_Unknown;

	Driver::DriverType t = drv->Type();
	delete drv;

	return t;
}

QMultiMap<Driver::DriverType, QString> SensorHubService::DriversByType() const
{
	return mDriversByType;
}

void SensorHubService::SaveConfig()
{
	QList<QVariant> config;
	foreach(SensorHubConfigPtr c, mRegisteredHubs)
		config.append(c->ToVariant());

	QSettings settings;
	settings.setValue("devices", config);
}

#undef DRIVER
#define DRIVER(name) Driver* Construct##name(); \
	QString GetName##name();
#include "DriverList.h"

void SensorHubService::GenerateDriverList()
{
	#undef DRIVER
	#define DRIVER(name) mDriverList[GetName##name()] = &Construct##name;
	#include "DriverList.h"

	QMapIterator<QString, DriverInitializer> it(mDriverList);
	while(it.hasNext())
	{
		it.next();

		mDriversByType.insert(GetDriverType(it.key()), it.key());
	}
}
