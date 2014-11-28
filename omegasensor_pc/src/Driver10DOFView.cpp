#include "Driver10DOFView.h"
#include "Driver10DOFModel.h"

#include <iostream>

#define ACCEL_RANGE (1.2f)
#define MAG_RANGE (1.3f * 100.0f) // 1.3 gauss = 130uT
#define GYRO_RANGE (245.0f)

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

	for(int i = mModel->NumAccelValues() - 1; i >= 0; i--)
	{
		accelX.append(mModel->AccelX(i));
		accelY.append(mModel->AccelY(i));
		accelZ.append(mModel->AccelZ(i));
	}

	ui.accelXPlot->SetRange(-ACCEL_RANGE, ACCEL_RANGE);
	ui.accelXPlot->SetData(accelX);

	ui.accelYPlot->SetRange(-ACCEL_RANGE, ACCEL_RANGE);
	ui.accelYPlot->SetData(accelY);

	ui.accelZPlot->SetRange(-ACCEL_RANGE, ACCEL_RANGE);
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
		magX.append(mModel->MagX(i));
		magY.append(mModel->MagY(i));
		magZ.append(mModel->MagZ(i));
	}

	ui.magXPlot->SetRange(-MAG_RANGE, MAG_RANGE);
	ui.magXPlot->SetData(magX);

	ui.magYPlot->SetRange(-MAG_RANGE, MAG_RANGE);
	ui.magYPlot->SetData(magY);

	ui.magZPlot->SetRange(-MAG_RANGE, MAG_RANGE);
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
		gyroX.append(mModel->GyroX(i));
		gyroY.append(mModel->GyroY(i));
		gyroZ.append(mModel->GyroZ(i));
	}

	ui.gyroXPlot->SetRange(-GYRO_RANGE, GYRO_RANGE);
	ui.gyroXPlot->SetData(gyroX);

	ui.gyroYPlot->SetRange(-GYRO_RANGE, GYRO_RANGE);
	ui.gyroYPlot->SetData(gyroY);

	ui.gyroZPlot->SetRange(-GYRO_RANGE, GYRO_RANGE);
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
}

void Driver10DOFView::closeEvent(QCloseEvent *evt)
{
	mModel->CleanClose();
}
