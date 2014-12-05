#ifndef __DriverSPI_h__
#define __DriverSPI_h__

#include "Driver.h"

class DriverSPI : public Driver
{
	Q_OBJECT

public:
	static QString Name() { return "Generic SPI"; }
	virtual DriverType Type() { return DriverType_SPI; }

	virtual void Initialize(const SensorHubPtr& hub);
	virtual void Send(const QByteArray& data);

protected slots:
	virtual void Recv(uint16_t characteristic,
		const QByteArray& data);
};

#endif // __DriverSPI_h__
