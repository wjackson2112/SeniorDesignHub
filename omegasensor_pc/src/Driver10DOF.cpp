#include "Driver10DOF.h"
#include "Driver10DOFView.h"
#include "Driver10DOFModel.h"
#include "SensorHub.h"

#include <iostream>
#include <iomanip>

#define ACCEL_ADDR  (0x19)
#define MAG_ADDR    (0x1E)
#define GYRO_ADDR   (0x6A)

#define ACCEL_RANGE (8.0f)
#define MAG_RANGE   (1.3f)
#define GYRO_RANGE  (245.0f)

// Mag
#define CRA_REG_M (0x00)
#define CRB_REG_M (0x01)
#define MR_REG_M  (0x02)

#define GN_1_3_GAUSS (1 << 5)
#define GN_1_9_GAUSS (2 << 5)
#define GN_2_5_GAUSS (3 << 5)
#define GN_4_0_GAUSS (4 << 5)
#define GN_4_7_GAUSS (5 << 5)
#define GN_5_6_GAUSS (6 << 5)
#define GN_8_1_GAUSS (7 << 5)

// Gyro
#define GYRO_CTRL1         (0x20)
#define GYRO_X_ENABLE      (0x01)
#define GYRO_Y_ENABLE      (0x02)
#define GYRO_Z_ENABLE      (0x04)
#define GYRO_NORMAL_MODE   (0x08)

#define GYRO_CTRL4         (0x23)
#define GYRO_CTRL4_245DPS  (0x00)
#define GYRO_CTRL4_500DPS  (0x10)
#define GYRO_CTRL4_2000DPS (0x20)

const uint16_t OMEGA_CHAR_I2C_RX      = 0x4741;

QString Driver10DOF::Name()
{
	return "10DOF";
}

void Driver10DOF::Initialize(const SensorHubPtr& hub)
{
	DriverI2C::Initialize(hub);

	// Initial state.
	mCurrentState = State_Wait;

	// Accel config.
	//RegisterWrite(0x00, 0x00, 0x00); // Reset the I2C bus.
	RegisterWrite(ACCEL_ADDR, 0x20, 0x03); // Power Down
	RegisterWrite(ACCEL_ADDR, 0x23, 0x20); // 8g range
	RegisterWrite(ACCEL_ADDR, 0x20, 0x27); // Run Mode
	//RegisterRead(ACCEL_ADDR, 0x23); // Check range

	// Mag config.
	RegisterWrite(MAG_ADDR, CRA_REG_M, 0x10); // 15Hz
	RegisterWrite(MAG_ADDR, MR_REG_M, 0x00); // Continuous mode.

	// Gyro config.
	RegisterWrite(GYRO_ADDR, GYRO_CTRL4, GYRO_CTRL4_245DPS);
	RegisterWrite(GYRO_ADDR, GYRO_CTRL1, GYRO_X_ENABLE |
		GYRO_Y_ENABLE | GYRO_Z_ENABLE| GYRO_NORMAL_MODE);
}

static Driver10DOF::State last = Driver10DOF::State_Mag;

void Driver10DOF::Sample()
{
	// Not done with the last sample, drop the request.
	if(mCurrentState != State_Wait)
	{
		//std::cout << "Request dropped. Too slowwwwwwww!" << std::endl;
		return;
	}

	//std::cout << "Sample!" << std::endl;

	// Accel first.
	mCurrentState = State_Accel;
	RegisterRead(ACCEL_ADDR, 0x28 | 0x80, 6); // Accel X, Y, Z
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
	{
		std::cout << "Event is not an I2C read." << std::endl;
		return;
	}
	
	if(data.count() != 6)
	{
		std::cout << "Invalid data size!" << std::endl;
		return;
	}

	/*
	for(int i = 0; i < data.count(); i++)
	{
		int8_t val = ((int8_t*)data.constData())[i];
		std::cout << "0x" << std::setfill('0') << std::setw(2)
			<< std::hex << (((uint32_t)val) & 0xFF) << std::endl;
	}

	std::cout << "10 DOF Data:" << std::endl;
	*/

	uint16_t pt[3] = { 0 };

	/*for(int i = 0; i < data.count(); i++)
	{
		int8_t val = ((int8_t*)data.constData())[i];
		pt[i / 2] = val << (8 * (i % 2));
		std::cout << "0x" << std::setfill('0') << std::setw(2)
			<< std::hex << (int)val << std::endl;
	}*/

	int16_t *spt = (int16_t*)pt;

	float x, y, z;
	switch(mCurrentState)
	{
		case State_Accel:
		{
			// OUT_X_L_A, OUT_X_H_A,
			// OUT_Y_L_A, OUT_Y_H_A,
			// OUT_Z_L_A, OUT_Z_H_A
			const uint8_t *values = (uint8_t*)data.constData();
			pt[0] = values[0] | ((uint16_t)values[1] << 8);
			pt[1] = values[2] | ((uint16_t)values[3] << 8);
			pt[2] = values[4] | ((uint16_t)values[5] << 8);

			x = (float)spt[0] * (ACCEL_RANGE / 32767.0f);
			y = (float)spt[1] * (ACCEL_RANGE / 32767.0f);
			z = (float)spt[2] * (ACCEL_RANGE / 32767.0f);

			emit Accel(x, y, z);

			// Mag next.
			mCurrentState = State_Mag;
			RegisterRead(MAG_ADDR, 0x03 | 0x80, 6); // Mag X, Z, Y
			std::cout << "Accel" << std::endl;
			break;
		}
		case State_Mag:
		{
			// OUT_X_H_M, OUT_X_L_M,
			// OUT_Z_H_M, OUT_Z_L_M,
			// OUT_Y_H_M, OUT_Y_L_M
			const uint8_t *values = (uint8_t*)data.constData();
			pt[0] = values[1] | ((uint16_t)values[0] << 8);
			pt[1] = values[5] | ((uint16_t)values[4] << 8);
			pt[2] = values[3] | ((uint16_t)values[2] << 8);

			x = (float)spt[0] * (MAG_RANGE / 32767.0f);
			z = (float)spt[1] * (MAG_RANGE / 32767.0f);
			y = (float)spt[2] * (MAG_RANGE / 32767.0f);

			// Convert the mag from gauss to uT.
			x *= 100.0f;
			y *= 100.0f;
			z *= 100.0f;

			emit Mag(x, y, z);

			// Gyro next.
			mCurrentState = State_Gyro;
			RegisterRead(GYRO_RANGE, 0x28 | 0x80, 6); // Gyro X, Z, Y
			std::cout << "Mag" << std::endl;
			break;
		}
		case State_Gyro:
		{
			// OUT_X_L, OUT_X_H,
			// OUT_Y_L, OUT_Y_H,
			// OUT_Z_L, OUT_Z_H
			const uint8_t *values = (uint8_t*)data.constData();
			pt[0] = values[0] | ((uint16_t)values[1] << 8);
			pt[1] = values[2] | ((uint16_t)values[3] << 8);
			pt[2] = values[4] | ((uint16_t)values[5] << 8);

			x = (float)spt[0] * (GYRO_RANGE / 32767.0f);
			z = (float)spt[1] * (GYRO_RANGE / 32767.0f);
			y = (float)spt[2] * (GYRO_RANGE / 32767.0f);

			emit Gyro(x, y, z);

			// Wait to start another sample.
			mCurrentState = State_Wait;
			std::cout << "Gyro" << std::endl;
			break;
		}
		default: // Wait
			break;
	}

	std::cout << QString("(%1, %2, %3)").arg(x).arg(y).arg(z
		).toUtf8().constData() << std::endl;
}

DRIVER(Driver10DOF)
