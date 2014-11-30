#include "Driver10DOFView.h"
#include "Driver10DOFModel.h"

#include <iostream>
#include <float.h>

#define ACCEL_RANGE (1.2f)
//#define MAG_RANGE (1.3f * 100.0f) // 1.3 gauss = 130uT
#define MAG_RANGE (40.0f)
//#define GYRO_RANGE (245.0f)
#define GYRO_RANGE (50.0f)

Driver10DOFView::Driver10DOFView(QWidget *p) : QWidget(p), mModel(0)
{
	// Create the GUI.
	ui.setupUi(this);
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
}

void Driver10DOFView::SetDeviceName(const QString& name)
{
	mDeviceName = name;
	setWindowTitle(tr("10DOF Sensor Module - %1").arg(name));
}

void Driver10DOFView::Update()
{
	//std::cout << "View updated!" << std::endl;

	// Accel
	QList<float> accelX, accelY, accelZ;

	float minAccelX = FLT_MAX, maxAccelX = -FLT_MAX;
	float minAccelY = FLT_MAX, maxAccelY = -FLT_MAX;
	float minAccelZ = FLT_MAX, maxAccelZ = -FLT_MAX;

	float minMagX = FLT_MAX, maxMagX = -FLT_MAX;
	float minMagY = FLT_MAX, maxMagY = -FLT_MAX;
	float minMagZ = FLT_MAX, maxMagZ = -FLT_MAX;

	float minGyroX = FLT_MAX, maxGyroX = -FLT_MAX;
	float minGyroY = FLT_MAX, maxGyroY = -FLT_MAX;
	float minGyroZ = FLT_MAX, maxGyroZ = -FLT_MAX;

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

	ui.accelXPlot->SetRange(minAccelX, maxAccelX);
	ui.accelXPlot->SetData(accelX);

	ui.accelYPlot->SetRange(minAccelY, maxAccelY);
	ui.accelYPlot->SetData(accelY);

	ui.accelZPlot->SetRange(minAccelZ, maxAccelZ);
	ui.accelZPlot->SetData(accelZ);

	ui.accelXLabel->setText(tr("X: %1g").arg(
		mModel->AccelX(0)));

	ui.accelYLabel->setText(tr("Y: %1g").arg(
		mModel->AccelY(0)));

	ui.accelZLabel->setText(tr("Z: %1g").arg(
		mModel->AccelZ(0)));

	float accelMag = sqrt(mModel->AccelX(0) * mModel->AccelX(0) +
		mModel->AccelY(0) * mModel->AccelY(0) +
		mModel->AccelZ(0) * mModel->AccelZ(0));

	ui.accelMagLabel->setText(tr("Mag: %1g").arg(
		accelMag));

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

	ui.magXPlot->SetRange(minMagX, maxMagX);
	ui.magXPlot->SetData(magX);

	ui.magYPlot->SetRange(minMagY, maxMagY);
	ui.magYPlot->SetData(magY);

	ui.magZPlot->SetRange(minMagZ, maxMagZ);
	ui.magZPlot->SetData(magZ);

	ui.magXLabel->setText(tr("X: %1uT").arg(
		mModel->MagX(0)));

	ui.magYLabel->setText(tr("Y: %1uT").arg(
		mModel->MagY(0)));

	ui.magZLabel->setText(tr("Z: %1uT").arg(
		mModel->MagZ(0)));

	float magMag = sqrt(mModel->MagX(0) * mModel->MagX(0) +
		mModel->MagY(0) * mModel->MagY(0) +
		mModel->MagZ(0) * mModel->MagZ(0));

	ui.magMagLabel->setText(tr("Mag: %1uT").arg(
		magMag));

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

	ui.gyroXPlot->SetRange(minGyroX, maxGyroX);
	ui.gyroXPlot->SetData(gyroX);

	ui.gyroYPlot->SetRange(minGyroY, maxGyroY);
	ui.gyroYPlot->SetData(gyroY);

	ui.gyroZPlot->SetRange(minGyroZ, maxGyroZ);
	ui.gyroZPlot->SetData(gyroZ);

	ui.gyroXLabel->setText(tr("X: %1dps").arg(
		mModel->GyroX(0)));

	ui.gyroYLabel->setText(tr("Y: %1dps").arg(
		mModel->GyroY(0)));

	ui.gyroZLabel->setText(tr("Z: %1dps").arg(
		mModel->GyroZ(0)));

	float gyroMag = sqrt(mModel->GyroX(0) * mModel->GyroX(0) +
		mModel->GyroY(0) * mModel->GyroY(0) +
		mModel->GyroZ(0) * mModel->GyroZ(0));

	ui.gyroMagLabel->setText(tr("Mag: %1dps").arg(
		gyroMag));

	ui.compassWidget->setDirection(
		mModel->MagX(0), mModel->MagY(0));
}

void Driver10DOFView::closeEvent(QCloseEvent *evt)
{
	mModel->CleanClose();
}
