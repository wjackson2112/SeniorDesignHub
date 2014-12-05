#include "DriverI2C.h"
#include "SensorHub.h"
#include "SensorHubConfig.h"
#include "SensorHubService.h"
#include "I2COperationChain.h"

#include <iostream>
#include <iomanip>

#define I2C_READ  ((uint8_t)1)
#define I2C_WRITE ((uint8_t)0)

#define I2C_INCLUDE_STOP_BIT (0x01)
#define I2C_NO_STOP_BIT      (0x00)

const uint16_t OMEGA_CHAR_CONF_SERVICE    = 0x2740;
const uint16_t OMEGA_CHAR_CONF_ENTER_PASS = 0x2741;

const uint16_t OMEGA_CHAR_I2C_SERVICE = 0x4740;
const uint16_t OMEGA_CHAR_I2C_RX      = 0x4741;
const uint16_t OMEGA_CHAR_I2C_TX      = 0x4742;
const uint16_t OMEGA_CHAR_I2C_CONFIG  = 0x4743;

void DriverI2C::Initialize(const SensorHubPtr& hub)
{
	Driver::Initialize(hub);

	Q_ASSERT(mHub);

	connect(mHub.data(), SIGNAL(Recv(uint16_t, const QByteArray&)),
		this, SLOT(Recv(uint16_t, const QByteArray&)));

	QByteArray passData = SensorHubService::GetSingletonPtr(
		)->GetSensorHubConfig(hub)->Password().toUtf8();

	uint8_t pass[20] = { 0 };
	memcpy(pass, passData.constData(), passData.size() >= 20
		? 19 : passData.size());

	mHub->Write(OMEGA_CHAR_CONF_SERVICE, OMEGA_CHAR_CONF_ENTER_PASS,
		QByteArray((char*)pass, sizeof(pass)));

	//mHub->SetNotify(OMEGA_CHAR_I2C_SERVICE, OMEGA_CHAR_I2C_RX);
}

void DriverI2C::RegisterRead(uint8_t addr, uint8_t reg, uint8_t count)
{
	addr <<= 1;

	DisableStop();

	// Write the register.
	{
		uint8_t data[] = {
			(uint8_t)(addr | I2C_WRITE),
			reg
		};

		Send(QByteArray((char*)data, sizeof(data)));
	}

	EnableStop();

	// Write dummy data to force a register read.
	{
		uint8_t data[count + 1];
		memset(data, 0, count + 1);
		data[0] = addr | I2C_READ;

		Send(QByteArray((char*)data, sizeof(data)));
	}

	mHub->Read(OMEGA_CHAR_I2C_SERVICE, OMEGA_CHAR_I2C_RX);
}

void DriverI2C::RegisterWrite(uint8_t addr, uint8_t reg, uint8_t value)
{
	addr <<= 1;

	EnableStop();

	// Write the register.
	{
		uint8_t data[] = {
			(uint8_t)(addr | I2C_WRITE),
			reg,
			value
		};

		Send(QByteArray((char*)data, sizeof(data)));
	}
}

void DriverI2C::DisableStop()
{//return;
	uint8_t data = I2C_NO_STOP_BIT;

	mHub->Write(OMEGA_CHAR_I2C_SERVICE, OMEGA_CHAR_I2C_CONFIG,
		QByteArray((char*)&data, 1));
}

void DriverI2C::EnableStop()
{//return;
	uint8_t data = I2C_INCLUDE_STOP_BIT;

	mHub->Write(OMEGA_CHAR_I2C_SERVICE, OMEGA_CHAR_I2C_CONFIG,
		QByteArray((char*)&data, 1));
}

void DriverI2C::Send(const QByteArray& data)
{
	mHub->Write(OMEGA_CHAR_I2C_SERVICE, OMEGA_CHAR_I2C_TX, data);
}

void DriverI2C::Send(const I2COperationChain& chain)
{
	QByteArray data = chain.OperationData();
	if(data.isEmpty())
		return;

	mHub->Write(OMEGA_CHAR_I2C_SERVICE, OMEGA_CHAR_I2C_TX, data);

	if(chain.ReadLength())
		mHub->Read(OMEGA_CHAR_I2C_SERVICE, OMEGA_CHAR_I2C_RX);
}

void DriverI2C::Recv(uint16_t characteristic,
	const QByteArray& data)
{
	if(characteristic != OMEGA_CHAR_I2C_RX)
		return;

	for(int i = 0; i < data.count(); i++)
	{
		uint8_t val = ((uint8_t*)data.constData())[i];
		std::cout << "0x" << std::setfill('0') << std::setw(2)
			<< std::hex << (int)val << std::endl;
	}
}

DRIVER(DriverI2C)
