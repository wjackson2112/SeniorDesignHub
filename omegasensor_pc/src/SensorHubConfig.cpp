#include "SensorHubConfig.h"

SensorHubConfig::SensorHubConfig()
{
	// Nothing to see here.
}

SensorHubConfig::SensorHubConfig(const QVariant& data)
{
	// Load the configuration.
	QMap<QString, QVariant> dataMap = data.toMap();

	// Bail if the map doesn't look right.
	if(!dataMap.contains("addr"))
		return;

	mName = dataMap.value("name").toString();
	mHardwareName = dataMap.value("hwname").toString();
	mDriverUART = dataMap.value("uart").toString();
	mDriverI2C = dataMap.value("i2c").toString();
	mDriverSPI = dataMap.value("spi").toString();
	mDriverAnalog = dataMap.value("analog").toString();
	mDriverDigital = dataMap.value("digital").toString();

	// Load this last since it is required.
	mAddress = dataMap.value("addr").toString();
}

QString SensorHubConfig::Name() const
{
	return mName;
}

void SensorHubConfig::SetName(const QString& name)
{
	mName = name;
}

QString SensorHubConfig::HardwareName() const
{
	return mHardwareName;
}

void SensorHubConfig::SetHardwareName(const QString& name)
{
	mHardwareName = name;
}

QString SensorHubConfig::GetAddress() const
{
	return mAddress;
}

void SensorHubConfig::SetAddress(const QString& addr)
{
	mAddress = addr;
}

QString SensorHubConfig::DriverUART() const
{
	return mDriverUART;
}

void SensorHubConfig::SetDriverUART(const QString& name)
{
	mDriverUART = name;
}

QString SensorHubConfig::DriverI2C() const
{
	return mDriverI2C;
}

void SensorHubConfig::SetDriverI2C(const QString& name)
{
	mDriverI2C = name;
}

QString SensorHubConfig::DriverSPI() const
{
	return mDriverSPI;
}

void SensorHubConfig::SetDriverSPI(const QString& name)
{
	mDriverSPI = name;
}

QString SensorHubConfig::DriverAnalog() const
{
	return mDriverAnalog;
}

void SensorHubConfig::SetDriverAnalog(const QString& name)
{
	mDriverAnalog = name;
}

QString SensorHubConfig::DriverDigital() const
{
	return mDriverDigital;
}

void SensorHubConfig::SetDriverDigital(const QString& name)
{
	mDriverDigital = name;
}

QStringList SensorHubConfig::DriverList() const
{
	QStringList drivers;

	if(!mDriverUART.isEmpty())
		drivers.append(mDriverUART);
	if(!mDriverI2C.isEmpty())
		drivers.append(mDriverI2C);
	if(!mDriverSPI.isEmpty())
		drivers.append(mDriverSPI);
	if(!mDriverAnalog.isEmpty())
		drivers.append(mDriverAnalog);
	if(!mDriverDigital.isEmpty())
		drivers.append(mDriverDigital);

	return drivers;
}

QVariant SensorHubConfig::ToVariant() const
{
	QMap<QString, QVariant> dataMap;
	dataMap["addr"] = mAddress;
	dataMap["name"] = mName;
	dataMap["hwname"] = mHardwareName;
	dataMap["uart"] = mDriverUART;
	dataMap["i2c"] = mDriverI2C;
	dataMap["spi"] = mDriverSPI;
	dataMap["analog"] = mDriverAnalog;
	dataMap["digital"] = mDriverDigital;

	return dataMap;
}
