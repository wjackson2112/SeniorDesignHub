#include "Driver10DOF.h"
#include "Driver10DOFView.h"
#include "Driver10DOFModel.h"
#include "SensorHub.h"

#include <iostream>
#include <iomanip>

#define ACCEL_ADDR  (0x19)
#define MAG_ADDR    (0x1E)

#define ACCEL_RANGE (16.0f)

#define MR_REG_M (0x02)

const uint16_t OMEGA_CHAR_I2C_RX      = 0x4741;

QString Driver10DOF::Name()
{
	return "10DOF";
}

void Driver10DOF::Initialize(const SensorHubPtr& hub)
{
	DriverI2C::Initialize(hub);

	// Accel config.
	//RegisterWrite(ACCEL_ADDR, 0x20, 0x03); // Power Down
	RegisterWrite(ACCEL_ADDR, 0x23, 0x30); // 16g range
	RegisterWrite(ACCEL_ADDR, 0x20, 0x27); // Run Mode
	//RegisterRead(ACCEL_ADDR, 0x23); // Check range
	//RegisterRead(ACCEL_ADDR, 0x28 | 0x80, 6);

	// Mag config.
	RegisterWrite(MAG_ADDR, MR_REG_M, 0x00); // Continuous mode.
}

void Driver10DOF::Sample()
{
	//RegisterRead(ACCEL_ADDR, 0x28 | 0x80, 6); // Accel
	RegisterRead(MAG_ADDR, 0x03/* | 0x80*/, 6); // Mag
}

QWidget* Driver10DOF::CreateView()
{
	Driver10DOFView *view = new Driver10DOFView();
	Driver10DOFModel *model = new Driver10DOFModel(this);
	view->SetDeviceName(mHub->HardwareName());
	view->SetModel(model);

	return view;
}

void Driver10DOF::Recv(uint16_t characteristic,
	const QByteArray& data)
{
	if(characteristic != OMEGA_CHAR_I2C_RX)
		return;

	for(int i = 0; i < data.count(); i++)
	{
		int8_t val = ((int8_t*)data.constData())[i];
		std::cout << "0x" << std::setfill('0') << std::setw(2)
			<< std::hex << (uint32_t)val << std::endl;
	}

	std::cout << "10 DOF Data:" << std::endl;

	uint16_t pt[3] = { 0 };

	/*for(int i = 0; i < data.count(); i++)
	{
		int8_t val = ((int8_t*)data.constData())[i];
		pt[i / 2] = val << (8 * (i % 2));
		std::cout << "0x" << std::setfill('0') << std::setw(2)
			<< std::hex << (int)val << std::endl;
	}*/

	const uint8_t *values = (uint8_t*)data.constData();
	pt[0] = values[0] | ((uint16_t)values[1] << 8);
	pt[1] = values[2] | ((uint16_t)values[3] << 8);
	pt[2] = values[4] | ((uint16_t)values[5] << 8);

	int16_t *spt = (int16_t*)pt;

	float x = (float)spt[0] * (ACCEL_RANGE / 32767.0f);
	float y = (float)spt[1] * (ACCEL_RANGE / 32767.0f);
	float z = (float)spt[2] * (ACCEL_RANGE / 32767.0f);

	std::cout << QString("(%1, %2, %3)").arg(x).arg(y).arg(z
		).toUtf8().constData() << std::endl;

	emit Accel(x, y, z);
}

DRIVER(Driver10DOF)
