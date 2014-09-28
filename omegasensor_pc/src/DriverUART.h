#ifndef __DriverUART_h__
#define __DriverUART_h__

#include "Driver.h"

#include <gato/gatoservice.h>
#include <gato/gatocharacteristic.h>

class DriverUART : public Driver
{
	Q_OBJECT

public:
	static QString Name() { return "Generic UART"; }

	virtual void Initialize(SensorHub *hub);
	virtual void Send(const QByteArray& data);

protected slots:
	virtual void Recv(const GatoCharacteristic& characteristic,
		const QByteArray& data);

protected:
	GatoCharacteristic mRX, mTX, mConfig;
};

#endif // __DriverUART_h__
