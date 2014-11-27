#ifndef __Driver_h__
#define __Driver_h__

#include <QtCore/QObject>
#include <QtCore/QString>

#include <iostream>
#include <stdint.h>

#include "SensorHub.h"

class QWidget;

class Driver : public QObject
{
	Q_OBJECT

public:
	virtual ~Driver();
	virtual QWidget* CreateView() { return 0; }
	virtual void Initialize(const SensorHubPtr& hub) { mHub = hub; }

	typedef enum _DriverType
	{
		DriverType_Unknown = -1,
		DriverType_UART = 0,
		DriverType_I2C,
		DriverType_SPI,
		DriverType_Analog,
		DriverType_Digital,
	}DriverType;

	virtual DriverType Type() = 0;

protected:
	SensorHubPtr mHub;
};

#define DRIVER(name) Driver* Construct##name() { return new name(); } \
	QString GetName##name() { return name::Name(); };

#endif // __Driver_h__
