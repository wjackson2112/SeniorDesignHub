#include "Driver10DOF.h"
#include "Driver10DOFView.h"
#include "Driver10DOFModel.h"
#include "I2COperationChain.h"
#include "SensorHub.h"

#include <iostream>
#include <iomanip>

#define ACCEL_ADDR  (0x19)
#define MAG_ADDR    (0x1E)
#define GYRO_ADDR   (0x6B)

#define ACCEL_RANGE (8.0f)
#define MAG_RANGE   (1.3f)
#define GYRO_RANGE  (245.0f)

// Mag
#define CRA_REG_M    (0x00)
#define CRB_REG_M    (0x01)
#define MR_REG_M     (0x02)
#define TEMP_ENABLE  (0x80)
#define TEMP_OUT_H_M (0x31)

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

const uint16_t OMEGA_CHAR_I2C_SERVICE = 0x4740;
const uint16_t OMEGA_CHAR_I2C_RX      = 0x4741;
const uint16_t OMEGA_CHAR_I2C_CONFIG  = 0x4743;

QString Driver10DOF::Name()
{
	return "10DOF";
}

void Driver10DOF::Initialize(const SensorHubPtr& hub)
{
	DriverI2C::Initialize(hub);

	uint8_t config = 0x07;

	mHub->Write(OMEGA_CHAR_I2C_SERVICE, OMEGA_CHAR_I2C_CONFIG,
		QByteArray((char*)&config, sizeof(config)));

	I2COperationChain op;
	op.SetTransactionID(0);
	op.SetTransactionIDEnabled(true);

	// Accel config.
	op.AddRegisterWrite(0x00, 0x00, 0x00); // Reset the I2C bus.
	op.AddRegisterWrite(ACCEL_ADDR, 0x20, 0x03); // Power Down
	op.AddRegisterWrite(ACCEL_ADDR, 0x23, 0x20); // 8g range
	op.AddRegisterWrite(ACCEL_ADDR, 0x20, 0x27); // Run Mode
	//op.AddRegisterRead(ACCEL_ADDR, 0x23); // Check range

	Send(op);
	op.Clear();
	op.SetTransactionID(0);
	op.SetTransactionIDEnabled(true);

	// Mag config.
	op.AddRegisterWrite(MAG_ADDR, CRA_REG_M, TEMP_ENABLE | 0x10); // 15Hz
	op.AddRegisterWrite(MAG_ADDR, MR_REG_M, 0x00); // Continuous mode.

	// Gyro config.
	op.AddRegisterWrite(GYRO_ADDR, GYRO_CTRL4, GYRO_CTRL4_245DPS);
	op.AddRegisterWrite(GYRO_ADDR, GYRO_CTRL1, GYRO_X_ENABLE |
		GYRO_Y_ENABLE | GYRO_Z_ENABLE| GYRO_NORMAL_MODE);

	Send(op);

	/*
	op.Clear();
	op.SetTransactionID(1);
	op.SetTransactionIDEnabled(true);
	op.AddRegisterRead(GYRO_ADDR, 0x0F, 1); // WHO_AM_I
	Send(op);
	*/
}

void Driver10DOF::Sample()
{
	I2COperationChain op;

	// Sample the accel, mag, and gyro.
	op.SetTransactionIDEnabled(true);
	op.SetTransactionID(0);
	op.AddRegisterRead(ACCEL_ADDR, 0x28 | 0x80, 6); // Accel X, Y, Z
	op.AddRegisterRead(MAG_ADDR, 0x03 | 0x80, 6); // Mag X, Z, Y
	op.AddRegisterRead(GYRO_ADDR, 0x28 | 0x80, 6); // Gyro X, Z, Y

	Send(op);

	op.Clear();
	op.SetTransactionIDEnabled(true);
	op.SetTransactionID(1);
	op.AddRegisterRead(MAG_ADDR, TEMP_OUT_H_M | 0x80, 2);

	Send(op);
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
	
	/*if(data.count() != 6)
	{
		std::cout << "Invalid data size!" << std::endl;
		return;
	}*/

	uint8_t transID = *((uint8_t*)data.constData());

	std::cout << "Transaction: " << (int)transID << std::endl;
	std::cout << "Size: " << std::dec << data.count() << std::endl;

	for(int i = 0; i < data.count(); i++)
	{
		int8_t val = ((int8_t*)data.constData())[i];
		std::cout << "0x" << std::setfill('0') << std::setw(2)
			<< std::hex << (((uint32_t)val) & 0xFF) << std::endl;
	}

	if(transID == 1)
	{
		uint16_t pt[3] = { 0 };

		// OUT_X_L_A, OUT_X_H_A,
		// OUT_Y_L_A, OUT_Y_H_A,
		// OUT_Z_L_A, OUT_Z_H_A
		const uint8_t *values = (uint8_t*)data.constData() + 1;
		pt[0] = values[1] | ((uint16_t)values[0] << 8);
		pt[0] >>= 4;
		if(pt[0] & 0x0800) // Sign extend
			pt[0] |= 0xF000;

		int16_t *spt = (int16_t*)pt;
		float temp = (float)spt[0] / 16.0f;
		std::cout << "Temp: " << temp << std::endl;
		return;
	}

	// Accel
	{
		uint16_t pt[3] = { 0 };

		// OUT_X_L_A, OUT_X_H_A,
		// OUT_Y_L_A, OUT_Y_H_A,
		// OUT_Z_L_A, OUT_Z_H_A
		const uint8_t *values = (uint8_t*)data.constData() + 1;
		pt[0] = values[0] | ((uint16_t)values[1] << 8);
		pt[1] = values[2] | ((uint16_t)values[3] << 8);
		pt[2] = values[4] | ((uint16_t)values[5] << 8);
		int16_t *spt = (int16_t*)pt;

		float x = (float)spt[0] * (ACCEL_RANGE / 32767.0f);
		float y = (float)spt[1] * (ACCEL_RANGE / 32767.0f);
		float z = (float)spt[2] * (ACCEL_RANGE / 32767.0f);

		emit Accel(x, y, z);

		std::cout << QString("(%1, %2, %3)").arg(x).arg(y).arg(z
			).toUtf8().constData() << std::endl;
	}

	// Mag
	{
		uint16_t pt[3] = { 0 };

		// OUT_X_H_M, OUT_X_L_M,
		// OUT_Z_H_M, OUT_Z_L_M,
		// OUT_Y_H_M, OUT_Y_L_M
		const uint8_t *values = (uint8_t*)data.constData() + 7;
		pt[0] = values[1] | ((uint16_t)values[0] << 8);
		pt[1] = values[5] | ((uint16_t)values[4] << 8);
		pt[2] = values[3] | ((uint16_t)values[2] << 8);
		int16_t *spt = (int16_t*)pt;

		float x = (float)spt[0] * (MAG_RANGE / 2047.0f);
		float z = (float)spt[1] * (MAG_RANGE / 2047.0f);
		float y = (float)spt[2] * (MAG_RANGE / 2047.0f);

		// Convert the mag from gauss to uT.
		x *= 100.0f;
		y *= 100.0f;
		z *= 100.0f;

		emit Mag(x, y, z);

		std::cout << QString("(%1, %2, %3)").arg(x).arg(y).arg(z
			).toUtf8().constData() << std::endl;
	}

	// Gyro
	{
		uint16_t pt[3] = { 0 };

		// OUT_X_L, OUT_X_H,
		// OUT_Y_L, OUT_Y_H,
		// OUT_Z_L, OUT_Z_H
		const uint8_t *values = (uint8_t*)data.constData() + 13;
		pt[0] = values[0] | ((uint16_t)values[1] << 8);
		pt[1] = values[2] | ((uint16_t)values[3] << 8);
		pt[2] = values[4] | ((uint16_t)values[5] << 8);
		int16_t *spt = (int16_t*)pt;

		float x = (float)spt[0] * (GYRO_RANGE / 32767.0f);
		float z = (float)spt[1] * (GYRO_RANGE / 32767.0f);
		float y = (float)spt[2] * (GYRO_RANGE / 32767.0f);

		emit Gyro(x, y, z);

		std::cout << QString("(%1, %2, %3)").arg(x).arg(y).arg(z
			).toUtf8().constData() << std::endl;
	}

	std::cout << "---" << std::endl;

	//std::cout << "10 DOF Data:" << std::endl;
return;

	/*for(int i = 0; i < data.count(); i++)
	{
		int8_t val = ((int8_t*)data.constData())[i];
		pt[i / 2] = val << (8 * (i % 2));
		std::cout << "0x" << std::setfill('0') << std::setw(2)
			<< std::hex << (int)val << std::endl;
	}*/

#if 0
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
#endif
}

DRIVER(Driver10DOF)
