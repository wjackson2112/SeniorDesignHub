/*
 * Omega Wireless Sensor Module
 * Copyright (C) 2014 Team Omega
 * Viva la Resistance
 *
 * Hardware:    Bogdan Crivin <bcrivin@uccs.edu>
 * Firmware:    William Jackson <wjackson@uccs.edu>
 * Software:    John Martin <jmarti25@uccs.edu>
 * Integration: Angela Askins <aaskins@uccs.edu>
 */

#ifndef __Driver10DOF_h__
#define __Driver10DOF_h__

#include "DriverI2C.h"

class Driver10DOF : public DriverI2C
{
	Q_OBJECT

public:
	virtual ~Driver10DOF() { }
	static QString Name();
	virtual QWidget* CreateView();
	virtual void Initialize(const SensorHubPtr& hub);
	virtual void Recv(uint16_t characteristic, const QByteArray& data);

signals:
	void Accel(float x, float y, float z);
	void Mag(float x, float y, float z);
	void Gyro(float x, float y, float z);
	void Pressure(float press);
	void Temp(float temp);

protected slots:
	void Sample();
	void SamplePressTemp();

private:
	int32_t BMP180_Measure();
	int32_t computeB5(int32_t ut);
	float getPressure();

	int16_t AC1;
	int16_t AC2;
	int16_t AC3;
	uint16_t AC4;
	uint16_t AC5;
	uint16_t AC6;
	int16_t B1;
	int16_t B2;
	int16_t MB;
	int16_t MC;
	int16_t MD;

	int32_t UT;
	int32_t UP;
	int32_t T;
};

#endif // __Driver10DOF_h__
