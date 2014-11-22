#include "DriverI2C.h"
#include "SensorHub.h"

#include <iostream>

#define I2C_READ  ((uint8_t)1)
#define I2C_WRITE ((uint8_t)0)

#define I2C_INCLUDE_STOP_BIT (0x01)
#define I2C_NO_STOP_BIT      (0x00)

const uint16_t OMEGA_CHAR_CONF_SERVICE = 0x2740;
const uint16_t OMEGA_CHAR_CONF_ENTER_PASS = 0x2741;

const uint16_t OMEGA_CHAR_I2C_SERVICE = 0x4740;
const uint16_t OMEGA_CHAR_I2C_RX      = 0x4741;
const uint16_t OMEGA_CHAR_I2C_TX      = 0x4742;
const uint16_t OMEGA_CHAR_I2C_CONFIG  = 0x4743;

void DriverI2C::Initialize(const SensorHubPtr& hub)
{
	Driver::Initialize(hub);

	Q_ASSERT(mHub);

	connect(mHub.data(), SIGNAL(Read(uint16_t, const QByteArray&)),
		this, SLOT(Recv(uint16_t, const QByteArray&)));

	uint8_t pass[20] = { 0};

	mHub->Write(OMEGA_CHAR_CONF_SERVICE, OMEGA_CHAR_CONF_ENTER_PASS,
		QByteArray((char*)pass, sizeof(pass)));
	mHub->SetNotify(OMEGA_CHAR_I2C_SERVICE, OMEGA_CHAR_I2C_RX);
	RegisterRead(0x19, 0x20);
}

void DriverI2C::RegisterRead(uint8_t addr, uint8_t reg)
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
		uint8_t data[] = {
			(uint8_t)(addr | I2C_READ),
			0x00
		};

		Send(QByteArray((char*)data, sizeof(data)));
	}
}

void DriverI2C::DisableStop()
{
	uint8_t data = I2C_NO_STOP_BIT;

	mHub->Write(OMEGA_CHAR_I2C_SERVICE, OMEGA_CHAR_I2C_CONFIG,
		QByteArray((char*)&data, 1));
}

void DriverI2C::EnableStop()
{
	uint8_t data = I2C_INCLUDE_STOP_BIT;

	mHub->Write(OMEGA_CHAR_I2C_SERVICE, OMEGA_CHAR_I2C_CONFIG,
		QByteArray((char*)&data, 1));
}

void DriverI2C::Send(const QByteArray& data)
{
	mHub->Write(OMEGA_CHAR_I2C_SERVICE, OMEGA_CHAR_I2C_TX, data);
}

void DriverI2C::Recv(uint16_t characteristic,
	const QByteArray& data)
{
	std::cout << "Recv" << std::endl;
	if(characteristic != OMEGA_CHAR_I2C_RX)
		return;

	std::cout << "Data!" << std::endl;
	std::cout << data.constData() << std::endl;
}

DRIVER(DriverI2C)
