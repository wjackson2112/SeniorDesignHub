#ifndef __DriverDigital_h__
#define __DriverDigital_h__

#include "Driver.h"

class DriverDigital : public Driver
{
	Q_OBJECT

public:
	static QString Name() { return "Generic Digital"; }
	virtual DriverType Type() { return DriverType_Digital; }

	virtual void Initialize(const SensorHubPtr& hub);
	virtual void On();
	virtual void Off();

protected slots:
	virtual void Recv(uint16_t characteristic,
		const QByteArray& data);
};

#endif // __DriverDigital_h__
