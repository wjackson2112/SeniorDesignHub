#ifndef __SensorHubConfig_h__
#define __SensorHubConfig_h__

#include <QtCore/QVariant>
#include <QtCore/QMetaType>
#include <QtCore/QSharedPointer>

#include <QtCore/QString>
#include <QtCore/QStringList>

class SensorHubConfig
{
public:
	SensorHubConfig();
	SensorHubConfig(const QVariant& data);

	QString Name() const;
	void SetName(const QString& name);

	QString HardwareName() const;
	void SetHardwareName(const QString& name);

	QString GetAddress() const;
	void SetAddress(const QString& addr);

	QVariant ToVariant() const;

	QString DriverUART() const;
	void SetDriverUART(const QString& name);

	QString DriverI2C() const;
	void SetDriverI2C(const QString& name);

	QString DriverSPI() const;
	void SetDriverSPI(const QString& name);

	QString DriverAnalog() const;
	void SetDriverAnalog(const QString& name);

	QString DriverDigital() const;
	void SetDriverDigital(const QString& name);

	QStringList DriverList() const;

	QString Password() const;
	void SetPassword(const QString& pass);

private:
	QString mAddress;
	QString mName, mHardwareName;
	QString mDriverUART, mDriverI2C, mDriverSPI;
	QString mDriverAnalog, mDriverDigital;
	QString mPassword;
};

typedef QSharedPointer<SensorHubConfig> SensorHubConfigPtr;

Q_DECLARE_METATYPE(SensorHubConfigPtr);

#endif // __SensorHubConfig_h__
