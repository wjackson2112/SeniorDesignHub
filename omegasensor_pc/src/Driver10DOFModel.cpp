#include "Driver10DOFModel.h"
#include "Driver10DOF.h"

#include <QtCore/QStringList>

#include <iostream>

Driver10DOFModel::Driver10DOFModel(Driver10DOF *drv,
	QObject * p) : QObject(p), mDriver(drv), mAccelCount(0),
	mMagCount(0), mGyroCount(0), mAccelLog("10dof-accel.csv"),
	mMagLog("10dof-mag.csv"), mGyroLog("10dof-gyro.csv")
{
	connect(&mTimer, SIGNAL(timeout()), drv, SLOT(Sample()));

	mTimer.setSingleShot(false);
	mTimer.start(100);

	for(size_t i = 0; i < MAX_HISTORY; i++)
	{
		mAccelX[i] = 0.0f;
		mAccelY[i] = 0.0f;
		mAccelZ[i] = 0.0f;
		mMagX[i] = 0.0f;
		mMagY[i] = 0.0f;
		mMagZ[i] = 0.0f;
		mGyroX[i] = 0.0f;
		mGyroY[i] = 0.0f;
		mGyroZ[i] = 0.0f;
	}

	connect(drv, SIGNAL(Accel(float, float, float)),
		this, SLOT(RecordAccel(float, float, float)));

	connect(drv, SIGNAL(Mag(float, float, float)),
		this, SLOT(RecordMag(float, float, float)));

	connect(drv, SIGNAL(Gyro(float, float, float)),
		this, SLOT(RecordGyro(float, float, float)));

	mAccelLog.open(QIODevice::ReadWrite);
	mMagLog.open(QIODevice::ReadWrite);
	mGyroLog.open(QIODevice::ReadWrite);
}

float Driver10DOFModel::AccelX(int idx) const
{
	if(idx < 0 || idx >= MAX_HISTORY)
		return 0.0f;

	return mAccelX[idx];
}

float Driver10DOFModel::AccelY(int idx) const
{
	if(idx < 0 || idx >= MAX_HISTORY)
		return 0.0f;

	return mAccelY[idx];
}

float Driver10DOFModel::AccelZ(int idx) const
{
	if(idx < 0 || idx >= MAX_HISTORY)
		return 0.0f;

	return mAccelZ[idx];
}

int Driver10DOFModel::NumAccelValues() const
{
	return mAccelCount;
}

void Driver10DOFModel::RecordAccel(float x, float y, float z)
{
	// Keep track of how many values.
	if(mAccelCount < MAX_HISTORY)
		mAccelCount++;

	// Shift the old values.
	for(size_t i = MAX_HISTORY - 1; i > 0; i--)
	{
		mAccelX[i] = mAccelX[i - 1];
		mAccelY[i] = mAccelY[i - 1];
		mAccelZ[i] = mAccelZ[i - 1];
	}

	// Save the new value.
	mAccelX[0] = x;
	mAccelY[0] = y;
	mAccelZ[0] = z;

	// Emit the signal to the view.
	//std::cout << "Send accel to view..." << std::endl;
	emit NewAccel(x, y, z);

	QStringList row;
	row.append(QString::number(x));
	row.append(QString::number(y));
	row.append(QString::number(z));
	mAccelLog.write(QString("%1\n").arg(row.join(",")).toUtf8());
	mAccelLog.flush();
}

float Driver10DOFModel::MagX(int idx) const
{
	if(idx < 0 || idx >= MAX_HISTORY)
		return 0.0f;

	return mMagX[idx];
}

float Driver10DOFModel::MagY(int idx) const
{
	if(idx < 0 || idx >= MAX_HISTORY)
		return 0.0f;

	return mMagY[idx];
}

float Driver10DOFModel::MagZ(int idx) const
{
	if(idx < 0 || idx >= MAX_HISTORY)
		return 0.0f;

	return mMagZ[idx];
}

int Driver10DOFModel::NumMagValues() const
{
	return mMagCount;
}

void Driver10DOFModel::RecordMag(float x, float y, float z)
{
	// Keep track of how many values.
	if(mMagCount < MAX_HISTORY)
		mMagCount++;

	// Shift the old values.
	for(size_t i = MAX_HISTORY - 1; i > 0; i--)
	{
		mMagX[i] = mMagX[i - 1];
		mMagY[i] = mMagY[i - 1];
		mMagZ[i] = mMagZ[i - 1];
	}

	// Save the new value.
	mMagX[0] = x;
	mMagY[0] = y;
	mMagZ[0] = z;

	// Emit the signal to the view.
	//std::cout << "Send mag to view..." << std::endl;
	emit NewMag(x, y, z);

	QStringList row;
	row.append(QString::number(x));
	row.append(QString::number(y));
	row.append(QString::number(z));
	mMagLog.write(QString("%1\n").arg(row.join(",")).toUtf8());
	mMagLog.flush();
}

float Driver10DOFModel::GyroX(int idx) const
{
	if(idx < 0 || idx >= MAX_HISTORY)
		return 0.0f;

	return mGyroX[idx];
}

float Driver10DOFModel::GyroY(int idx) const
{
	if(idx < 0 || idx >= MAX_HISTORY)
		return 0.0f;

	return mGyroY[idx];
}

float Driver10DOFModel::GyroZ(int idx) const
{
	if(idx < 0 || idx >= MAX_HISTORY)
		return 0.0f;

	return mGyroZ[idx];
}

int Driver10DOFModel::NumGyroValues() const
{
	return mGyroCount;
}

void Driver10DOFModel::RecordGyro(float x, float y, float z)
{
	// Keep track of how many values.
	if(mGyroCount < MAX_HISTORY)
		mGyroCount++;

	// Shift the old values.
	for(size_t i = MAX_HISTORY - 1; i > 0; i--)
	{
		mGyroX[i] = mGyroX[i - 1];
		mGyroY[i] = mGyroY[i - 1];
		mGyroZ[i] = mGyroZ[i - 1];
	}

	// Save the new value.
	mGyroX[0] = x;
	mGyroY[0] = y;
	mGyroZ[0] = z;

	// Emit the signal to the view.
	//std::cout << "Send gyro to view..." << std::endl;
	emit NewGyro(x, y, z);

	QStringList row;
	row.append(QString::number(x));
	row.append(QString::number(y));
	row.append(QString::number(z));
	mGyroLog.write(QString("%1\n").arg(row.join(",")).toUtf8());
	mGyroLog.flush();
}

void Driver10DOFModel::CleanClose()
{
	delete mDriver;
	mDriver = 0;
}
