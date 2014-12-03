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

// BMP180 registers.
#define BMP180_REG_CHIPID     (0xD0)
#define BMP180_REG_SOFT_RESET (0xE0)
#define BMP180_REG_CTRL_MEAS  (0xF4)
#define BMP180_REG_OUT_MSB    (0xF6)
#define BMP180_REG_OUT_LSB    (0xF7)
#define BMP180_REG_OUT_XLSB   (0xF8)
#define BMP180_REG_OUT_CALIB0 (0xAA)

// Control register values to start a conversion.
// OSS of 0=4.5ms, 1=7.5ms, 2=13.5ms, 3=25.5ms
#define BMP180_CTRL_MEAS_TEMP       (0x2E)
#define BMP180_CTRL_MEAS_PRESS(oss) (0x34 + ((oss) << 6))

// I2C slave address of the BMP180.
#define BMP180_SLAVE_ADDR        (0x77)

// Expected Chip ID (read from the CHIPID register).
#define BMP180_CHIPID            (0x55)

const uint16_t OMEGA_CHAR_I2C_SERVICE = 0x4740;
const uint16_t OMEGA_CHAR_I2C_RX      = 0x4741;
const uint16_t OMEGA_CHAR_I2C_CONFIG  = 0x4743;

static uint8_t _bmp085Mode = 1;

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
	op.SetTransactionID(255);
	op.SetTransactionIDEnabled(true);

	// Accel config.
	op.AddRegisterWrite(0x00, 0x00, 0x00); // Reset the I2C bus.
	op.AddRegisterWrite(ACCEL_ADDR, 0x20, 0x03); // Power Down
	op.AddRegisterWrite(ACCEL_ADDR, 0x23, 0x20); // 8g range
	op.AddRegisterWrite(ACCEL_ADDR, 0x20, 0x27); // Run Mode
	//op.AddRegisterRead(ACCEL_ADDR, 0x23); // Check range

	Send(op);
	op.Clear();
	op.SetTransactionID(254);
	op.SetTransactionIDEnabled(true);

	// Mag config.
	op.AddRegisterWrite(MAG_ADDR, CRA_REG_M, TEMP_ENABLE | 0x10); // 15Hz
	op.AddRegisterWrite(MAG_ADDR, MR_REG_M, 0x00); // Continuous mode.

	// Gyro config.
	op.AddRegisterWrite(GYRO_ADDR, GYRO_CTRL4, GYRO_CTRL4_245DPS);
	op.AddRegisterWrite(GYRO_ADDR, GYRO_CTRL1, GYRO_X_ENABLE |
		GYRO_Y_ENABLE | GYRO_Z_ENABLE| GYRO_NORMAL_MODE);

	Send(op);

	op.Clear();
	op.SetTransactionID(4);
	op.SetTransactionIDEnabled(true);
	op.AddRegisterRead(BMP180_SLAVE_ADDR, BMP180_REG_OUT_CALIB0, 12);

	Send(op);

	op.Clear();
	op.SetTransactionID(5);
	op.SetTransactionIDEnabled(true);
	op.AddRegisterRead(BMP180_SLAVE_ADDR, BMP180_REG_OUT_CALIB0 + 12, 10);

	Send(op);

	SamplePressTemp();
}

#define ACCEL_REG_DATA (0x28 | 0x80)
#define MAG_REG_DATA (0x03)
#define GYRO_REG_DATA (0x28 | 0x80)

void Driver10DOF::Sample()
{
	I2COperationChain op;

	// Sample the accel, mag, and gyro.
	op.SetTransactionIDEnabled(true);
	op.SetTransactionID(0);
	op.AddRegisterRead(ACCEL_ADDR, ACCEL_REG_DATA, 6); // Accel X, Y, Z
	op.AddRegisterRead(MAG_ADDR, MAG_REG_DATA, 6); // Mag X, Z, Y
	op.AddRegisterRead(GYRO_ADDR, GYRO_REG_DATA, 6); // Gyro X, Z, Y

	Send(op);
}

void Driver10DOF::SamplePressTemp()
{
	I2COperationChain op;

	op.SetTransactionIDEnabled(true);
	op.SetTransactionID(1);
	op.AddRegisterRead(MAG_ADDR, TEMP_OUT_H_M, 2);

	// Start a temperature measurement.
	op.AddRegisterWrite(BMP180_SLAVE_ADDR, BMP180_REG_CTRL_MEAS,
		BMP180_CTRL_MEAS_TEMP);

	Send(op);

	op.Clear();
	op.SetTransactionIDEnabled(true);
	op.SetTransactionID(2);

	// Read the temp measurement.
	op.AddRegisterRead(BMP180_SLAVE_ADDR, BMP180_REG_OUT_MSB, 2);

	// Start a pressure measurement (oss=0).
	op.AddRegisterWrite(BMP180_SLAVE_ADDR,BMP180_REG_CTRL_MEAS,
		BMP180_CTRL_MEAS_PRESS(_bmp085Mode));

	Send(op);

	op.Clear();
	op.SetTransactionIDEnabled(true);
	op.SetTransactionID(3);

	// Read the pressure measurement.
	op.AddRegisterRead(BMP180_SLAVE_ADDR, BMP180_REG_OUT_MSB, 3);

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

	uint8_t transID = *((uint8_t*)data.constData());

	/*
	std::cout << "Transaction: " << (int)transID << std::endl;
	std::cout << "Size: " << std::dec << data.count() << std::endl;
	*/

	/*
	for(int i = 0; i < data.count(); i++)
	{
		int8_t val = ((int8_t*)data.constData())[i];
		std::cout << "0x" << std::setfill('0') << std::setw(2)
			<< std::hex << (((uint32_t)val) & 0xFF) << std::endl;
	}
	*/

	switch(transID)
	{
		case 1:
		{
			/*
			uint16_t pt[3] = { 0 };

			// OUT_X_L_A, OUT_X_H_A,
			// OUT_Y_L_A, OUT_Y_H_A,
			// OUT_Z_L_A, OUT_Z_H_A
			const uint8_t *values = (uint8_t*)data.constData() + 1;
			pt[0] = values[1] | ((uint16_t)values[0] << 8);
			int16_t *spt = (int16_t*)pt;

			float temp = ((float)spt[0] * 0.0085989392f) + 15.719639f;

			emit Temp(temp);

			std::cout << "Temp: " << temp << std::endl;
			*/

			return;
		}
		case 2:
		{
			uint16_t pt[3] = { 0 };

			const uint8_t *values = (uint8_t*)data.constData() + 1;
			pt[0] = values[1] | ((uint16_t)values[0] << 8);
			UT = pt[0];

			return;
		}
		case 3:
		{
			uint16_t pt[3] = { 0 };

			const uint8_t *values = (uint8_t*)data.constData() + 1;
			pt[0] = values[1] | ((uint16_t)values[0] << 8);
			UP = pt[0];
			int32_t p = BMP180_Measure();

			float altitude = 44330.0f * (1.0f - pow(p / 101325.0f, 1.0f / 5.255f));
			altitude *= 3.28084;

			emit Temp((float)T / 10.0f);
			emit Pressure(p);

			/*
			std::cout << "Bosch Temp: " << ((float)T / 10.0f) << std::endl;
			std::cout << "Pressure: " << std::dec << p << "Pa" << std::endl;
			std::cout << "Altitude: " << altitude << std::endl;
			*/
			return;
		}
		case 4:
		{
			const uint8_t *calData = (uint8_t*)data.constData() + 1;

			// Save the calibration data.
			AC1 = (calData[0] << 8) | calData[1];
			AC2 = (calData[2] << 8) | calData[3];
			AC3 = (calData[4] << 8) | calData[5];
			AC4 = (calData[6] << 8) | calData[7];
			AC5 = (calData[8] << 8) | calData[9];
			AC6 = (calData[10] << 8) | calData[11];
			return;
		}
		case 5:
		{
			const uint8_t *calData = (uint8_t*)data.constData() + 1;

			// Save the calibration data.
			B1 = (calData[0] << 8) | calData[1];
			B2 = (calData[2] << 8) | calData[3];
			MB = (calData[4] << 8) | calData[5];
			MC = (calData[6] << 8) | calData[7];
			MD = (calData[8] << 8) | calData[9];

			/*
			printf("AC1: %d\n", (int)AC1);
			printf("AC2: %d\n", (int)AC2);
			printf("AC3: %d\n", (int)AC3);
			printf("AC4: %d\n", (int)AC4);
			printf("AC5: %d\n", (int)AC5);
			printf("AC6: %d\n", (int)AC6);
			printf("B1: %d\n", (int)B1);
			printf("B2: %d\n", (int)B2);
			printf("MB: %d\n", (int)MB);
			printf("MC: %d\n", (int)MC);
			printf("MD: %d\n", (int)MD);
			*/
			return;
		}
	} // switch(transID)

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

		//std::cout << QString("(%1, %2, %3)").arg(x).arg(y).arg(z
			//).toUtf8().constData() << std::endl;
	}

	// Mag
	{
		uint16_t pt[3] = { 0 };

		// OUT_X_H_M, OUT_X_L_M,
		// OUT_Z_H_M, OUT_Z_L_M,
		// OUT_Y_H_M, OUT_Y_L_M
		const uint8_t *values = (uint8_t*)data.constData() + 7;
		/*pt[0] = values[1] | ((uint16_t)values[0] << 8);
		pt[1] = values[5] | ((uint16_t)values[4] << 8);
		pt[2] = values[3] | ((uint16_t)values[2] << 8);*/
		pt[0] = values[1] | ((uint16_t)values[0] << 8);
		pt[1] = values[3] | ((uint16_t)values[2] << 8);
		pt[2] = values[5] | ((uint16_t)values[4] << 8);
		int16_t *spt = (int16_t*)pt;

		float x = (float)spt[0] * (MAG_RANGE / 2047.0f);
		float z = (float)spt[1] * (MAG_RANGE / 2047.0f);
		float y = (float)spt[2] * (MAG_RANGE / 2047.0f);

		// Convert the mag from gauss to uT.
		x *= 100.0f;
		y *= 100.0f;
		z *= 100.0f;

		emit Mag(x, y, z);

		//std::cout << QString("(%1, %2, %3)").arg(x).arg(y).arg(z
			//).toUtf8().constData() << std::endl;
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

		//std::cout << QString("(%1, %2, %3)").arg(x).arg(y).arg(z
			//).toUtf8().constData() << std::endl;
	}

	//std::cout << "---" << std::endl;
}

int32_t Driver10DOF::BMP180_Measure()
{
	int16_t oss = 0;

	// bmp180_get_temperature
	int32_t X1 = (UT - AC6) * AC5 / 32768; // 32768=2^15
	int32_t X2 = (MC * 2048) / (X1 + MD); // 2048=2^11
	int32_t B5 = X1 + X2;
	T = (B5 + 8) / 16; // 16=2^4

	// bmp180_calpressure
	int32_t B6 = B5 - 4000;
	X1 = (B2 * (B6 * B6 / 4096)) / 2048; // 4096=2^12, 2048=2^11
	X2 = AC2 * B6 / 2048; // 2048=2^11
	int32_t X3 = X1 + X2;
	int32_t B3 = (((AC1 * 4 + X3) << oss) + 2) / 4;
	X1 = AC3 * B6 / 8192; // 8192=2^13
	X2 = (B1 * (B6 * B6 / 4096)) / 65536; // 4096=2^12, 65536=2^16
	X3 = ((X1 + X2) + 2) / 4; // 4=2^2
	uint32_t B4 = AC4 * (uint32_t)(X3 + 32768) / 32768; // 32768=2^15
	uint32_t B7 = ((uint32_t)UP - B3) * (50000 >> oss);

	int32_t p;
	if(B7 < 0x80000000)
		p = (B7 * 2) / B4;
	else
		p = (B7 / B4) * 2;

	X1 = (p / 256) * (p / 256); // 256=2^8
	X1 = (X1 * 3038) / 65536; // 65536=2^16
	X2 = (-7357 * p) / 65536; // 65536=2^16
	p = p + (X1 + X2 + 3791) / 16; // 16=2^4

	return p;
}

DRIVER(Driver10DOF)
