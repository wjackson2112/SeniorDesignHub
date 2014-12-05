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

#include "Driver10DOFView.h"
#include "Driver10DOFModel.h"

#include <iostream>
#include <float.h>

#define ACCEL_RANGE (1.0f)
#define MAG_RANGE (70.0f)
#define GYRO_RANGE (40.0f)

Driver10DOFView::Driver10DOFView(QWidget *p) : QWidget(p),
	mModel(0), mSpeed(0.0f)
{
	// Create the GUI.
	ui.setupUi(this);
	ui.speedLabel->setVisible(false);

	/*
	minAccelX = FLT_MAX; maxAccelX = -FLT_MAX;
	minAccelY = FLT_MAX; maxAccelY = -FLT_MAX;
	minAccelZ = FLT_MAX; maxAccelZ = -FLT_MAX;

	minMagX = FLT_MAX; maxMagX = -FLT_MAX;
	minMagY = FLT_MAX; maxMagY = -FLT_MAX;
	minMagZ = FLT_MAX; maxMagZ = -FLT_MAX;

	minGyroX = FLT_MAX; maxGyroX = -FLT_MAX;
	minGyroY = FLT_MAX; maxGyroY = -FLT_MAX;
	minGyroZ = FLT_MAX; maxGyroZ = -FLT_MAX;
	*/
}

void Driver10DOFView::SetModel(Driver10DOFModel *model)
{
	// Delete the old model.
	if(mModel)
		delete mModel;

	mModel = model;
	Q_ASSERT(mModel);

	connect(mModel, SIGNAL(NewAccel(float, float, float)),
		this, SLOT(Update()));

	connect(mModel, SIGNAL(NewMag(float, float, float)),
		this, SLOT(Update()));

	connect(mModel, SIGNAL(NewGyro(float, float, float)),
		this, SLOT(Update()));

	connect(mModel, SIGNAL(NewPressure(float)),
		this, SLOT(Update()));

	connect(mModel, SIGNAL(NewTemp(float)),
		this, SLOT(Update()));
}

void Driver10DOFView::SetDeviceName(const QString& name)
{
	mDeviceName = name;
	setWindowTitle(tr("10DOF Sensor Module - %1").arg(name));
}

void Driver10DOFView::Update()
{
	//std::cout << "View updated!" << std::endl;

	minAccelX = FLT_MAX; maxAccelX = -FLT_MAX;
	minAccelY = FLT_MAX; maxAccelY = -FLT_MAX;
	minAccelZ = FLT_MAX; maxAccelZ = -FLT_MAX;

	minMagX = FLT_MAX; maxMagX = -FLT_MAX;
	minMagY = FLT_MAX; maxMagY = -FLT_MAX;
	minMagZ = FLT_MAX; maxMagZ = -FLT_MAX;

	minGyroX = FLT_MAX; maxGyroX = -FLT_MAX;
	minGyroY = FLT_MAX; maxGyroY = -FLT_MAX;
	minGyroZ = FLT_MAX; maxGyroZ = -FLT_MAX;

	// Accel
	QList<float> accelX, accelY, accelZ;

	for(int i = 0; i < mModel->NumAccelValues(); i++)
	{
		float x = mModel->AccelX(i);
		if(x < minAccelX)
			minAccelX = x;
		if(x > maxAccelX)
			maxAccelX = x;

		float y = mModel->AccelY(i);
		if(y < minAccelY)
			minAccelY = y;
		if(y > maxAccelY)
			maxAccelY = y;

		float z = mModel->AccelZ(i);
		if(z < minAccelZ)
			minAccelZ = z;
		if(z > maxAccelZ)
			maxAccelZ = z;

		accelX.prepend(x);
		accelY.prepend(y);
		accelZ.prepend(z);
	}

	//ui.accelXPlot->SetRange(minAccelX, maxAccelX);
	ui.accelXPlot->SetRange(-ACCEL_RANGE, ACCEL_RANGE);
	ui.accelXPlot->SetData(accelX);

	//ui.accelYPlot->SetRange(minAccelY, maxAccelY);
	ui.accelYPlot->SetRange(-ACCEL_RANGE, ACCEL_RANGE);
	ui.accelYPlot->SetData(accelY);

	//ui.accelZPlot->SetRange(minAccelZ, maxAccelZ);
	ui.accelZPlot->SetRange(-ACCEL_RANGE, ACCEL_RANGE);
	ui.accelZPlot->SetData(accelZ);

	ui.accelXLabel->setText(tr("X: %1g").arg(
		mModel->AccelX(0), 0, 'g', 4));

	ui.accelYLabel->setText(tr("Y: %1g").arg(
		mModel->AccelY(0), 0, 'g', 4));

	ui.accelZLabel->setText(tr("Z: %1g").arg(
		mModel->AccelZ(0), 0, 'g', 4));

	float accelMag = sqrt(mModel->AccelX(0) * mModel->AccelX(0) +
		mModel->AccelY(0) * mModel->AccelY(0) +
		mModel->AccelZ(0) * mModel->AccelZ(0));

	ui.accelMagLabel->setText(tr("Mag: %1g").arg(
		accelMag, 0, 'g', 4));

	// Mag
	QList<float> magX, magY, magZ;

	for(int i = mModel->NumMagValues() - 1; i >= 0; i--)
	{
		float x = mModel->MagX(i);
		if(x < minMagX)
			minMagX = x;
		if(x > maxMagX)
			maxMagX = x;

		float y = mModel->MagY(i);
		if(y < minMagY)
			minMagY = y;
		if(y > maxMagY)
			maxMagY = y;

		float z = mModel->MagZ(i);
		if(z < minMagZ)
			minMagZ = z;
		if(z > maxMagZ)
			maxMagZ = z;

		magX.append(x);
		magY.append(y);
		magZ.append(z);
	}

	//ui.magXPlot->SetRange(minMagX, maxMagX);
	ui.magXPlot->SetRange(-MAG_RANGE, MAG_RANGE);
	ui.magXPlot->SetData(magX);

	//ui.magYPlot->SetRange(minMagY, maxMagY);
	ui.magYPlot->SetRange(-MAG_RANGE, MAG_RANGE);
	ui.magYPlot->SetData(magY);

	//ui.magZPlot->SetRange(minMagZ, maxMagZ);
	ui.magZPlot->SetRange(-MAG_RANGE, MAG_RANGE);
	ui.magZPlot->SetData(magZ);

	ui.magXLabel->setText(tr("X: %1uT").arg(
		mModel->MagX(0), 0, 'g', 4));

	ui.magYLabel->setText(tr("Y: %1uT").arg(
		mModel->MagY(0), 0, 'g', 4));

	ui.magZLabel->setText(tr("Z: %1uT").arg(
		mModel->MagZ(0), 0, 'g', 4));

	float magMag = sqrt(mModel->MagX(0) * mModel->MagX(0) +
		mModel->MagY(0) * mModel->MagY(0) +
		mModel->MagZ(0) * mModel->MagZ(0));

	ui.magMagLabel->setText(tr("Mag: %1uT").arg(
		magMag, 0, 'g', 4));

	// Gyro
	QList<float> gyroX, gyroY, gyroZ;

	for(int i = mModel->NumGyroValues() - 1; i >= 0; i--)
	{
		float x = mModel->GyroX(i);
		if(x < minGyroX)
			minGyroX = x;
		if(x > maxGyroX)
			maxGyroX = x;

		float y = mModel->GyroY(i);
		if(y < minGyroY)
			minGyroY = y;
		if(y > maxGyroY)
			maxGyroY = y;

		float z = mModel->GyroZ(i);
		if(z < minGyroZ)
			minGyroZ = z;
		if(z > maxGyroZ)
			maxGyroZ = z;

		gyroX.append(x);
		gyroY.append(y);
		gyroZ.append(z);
	}

	//ui.gyroXPlot->SetRange(minGyroX, maxGyroX);
	ui.gyroXPlot->SetRange(-GYRO_RANGE, GYRO_RANGE);
	ui.gyroXPlot->SetData(gyroX);

	//ui.gyroYPlot->SetRange(minGyroY, maxGyroY);
	ui.gyroYPlot->SetRange(-GYRO_RANGE, GYRO_RANGE);
	ui.gyroYPlot->SetData(gyroY);

	//ui.gyroZPlot->SetRange(minGyroZ, maxGyroZ);
	ui.gyroZPlot->SetRange(-GYRO_RANGE, GYRO_RANGE);
	ui.gyroZPlot->SetData(gyroZ);

	ui.gyroXLabel->setText(tr("X: %1dps").arg(
		mModel->GyroX(0), 0, 'g', 4));

	ui.gyroYLabel->setText(tr("Y: %1dps").arg(
		mModel->GyroY(0), 0, 'g', 4));

	ui.gyroZLabel->setText(tr("Z: %1dps").arg(
		mModel->GyroZ(0), 0, 'g', 4));

	float gyroMag = sqrt(mModel->GyroX(0) * mModel->GyroX(0) +
		mModel->GyroY(0) * mModel->GyroY(0) +
		mModel->GyroZ(0) * mModel->GyroZ(0));

	ui.gyroMagLabel->setText(tr("Mag: %1dps").arg(
		gyroMag, 0, 'g', 4));

	/*ui.compassWidget->setDirection(
		mModel->MagX(0), mModel->MagY(0));*/
	ui.compassWidget->setDirection(
		mModel->AvgMagX(), mModel->AvgMagY());

	float tempC = mModel->Temp(0);
	float tempF = (9.0f / 5.0f) * tempC + 32.0f;

	float press = mModel->Pressure(0);
	float altitude = 44330.0f * (1.0f - pow(press / 101325.0f, 1.0f / 5.255f));
	altitude *= 3.28084; // meters to feet.

	ui.tempLabel->setText(tr("Temperature: %1C / %2F").arg(tempC).arg(tempF));
	ui.pressureLabel->setText(tr("Pressure: %1 Pa").arg(press));
	ui.altitudeLabel->setText(tr("Altitude: %1 ft.").arg(altitude));

	// Calculate speed.
	if(mModel->NumAccelValues() > 10)
	{
		float x = mModel->AccelX(0);
		float y = mModel->AccelY(0);
		float z = mModel->AccelZ(0);
		float newMagnitude = sqrt(x * x + y * y + z * z) - 1.0f;

		x = mModel->AccelX(1);
		y = mModel->AccelY(1);
		z = mModel->AccelZ(1);
		float oldMagnitude = sqrt(x * x + y * y + z * z) - 1.0f;

		float timeDelta = mModel->AccelTime(1) - mModel->AccelTime(0);
		timeDelta /= 1000.0f; // msec to sec.

		float speedChange = oldMagnitude * timeDelta +
			(newMagnitude / 2.0f) * timeDelta * timeDelta;
		speedChange *= 21.937; // g=>mph

		mSpeed += speedChange;

		ui.speedLabel->setText(tr("Speed: %1 MPH").arg(mSpeed));
	}
}

void Driver10DOFView::closeEvent(QCloseEvent *evt)
{
	Q_UNUSED(evt);

	mModel->CleanClose();
}
