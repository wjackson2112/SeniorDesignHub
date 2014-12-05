#ifndef __DriverAnalog_h__
#define __DriverAnalog_h__

#include "Driver.h"

class DriverAnalog : public Driver
{
	Q_OBJECT

public:
	static QString Name() { return "Generic Analog"; }
	virtual DriverType Type() { return DriverType_Analog; }

	virtual void Initialize(const SensorHubPtr& hub);

protected slots:
	virtual void Recv(uint16_t characteristic,
		const QByteArray& data);
};

#endif // __DriverAnalog_h__
