#ifndef __DriverI2C_h__
#define __DriverI2C_h__

#include "Driver.h"

class I2COperationChain;

class DriverI2C : public Driver
{
	Q_OBJECT

public:
	virtual ~DriverI2C() { }
	static QString Name() { return "Generic I2C"; }
	virtual DriverType Type() { return DriverType_I2C; }

	virtual void Initialize(const SensorHubPtr& hub);
	virtual void Send(const QByteArray& data);

	void Send(const I2COperationChain& chain);

	void DisableStop();
	void EnableStop();

	void RegisterRead(uint8_t addr, uint8_t reg, uint8_t count = 1);
	void RegisterWrite(uint8_t addr, uint8_t reg, uint8_t data);

protected slots:
	virtual void Recv(uint16_t characteristic,
		const QByteArray& data);
};

#endif // __DriverI2C_h__
