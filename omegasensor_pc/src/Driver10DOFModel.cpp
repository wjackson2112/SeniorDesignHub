#include "Driver10DOFModel.h"
#include "Driver10DOF.h"

#include <QtCore/QStringList>

#include <iostream>

const int MAG_AVG_COUNT = 30;

Driver10DOFModel::Driver10DOFModel(Driver10DOF *drv,
	QObject * p) : QObject(p), mDriver(drv), mAccelCount(0),
	mMagCount(0), mGyroCount(0), mPressureCount(0), mTempCount(0),
	mAccelLog("10dof-accel.csv"), mMagLog("10dof-mag.csv"),
	mGyroLog("10dof-gyro.csv"), mPressureLog("10dof-press.csv"),
	mTempLog("10dof-temp.csv")
{
	connect(&mTimer, SIGNAL(timeout()), drv, SLOT(Sample()));

	mTimer.setSingleShot(false);
	mTimer.start(10); // 10 msec

	connect(&mTimer2, SIGNAL(timeout()), drv, SLOT(SamplePressTemp()));

	mTimer2.setSingleShot(false);
	mTimer2.start(60 * 1000); // 1 min

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
		mPressure[i] = 0.0f;
		mTemp[i] = 0.0f;
	}

	connect(drv, SIGNAL(Accel(float, float, float)),
		this, SLOT(RecordAccel(float, float, float)));

	connect(drv, SIGNAL(Mag(float, float, float)),
		this, SLOT(RecordMag(float, float, float)));

	connect(drv, SIGNAL(Gyro(float, float, float)),
		this, SLOT(RecordGyro(float, float, float)));

	connect(drv, SIGNAL(Pressure(float)),
		this, SLOT(RecordPressure(float)));

	connect(drv, SIGNAL(Temp(float)),
		this, SLOT(RecordTemp(float)));

	mAccelLog.open(QIODevice::ReadWrite);
	mAccelLog.write(tr("Time (msec),X (g),Y (g),Z (g)\n").toUtf8());

	mMagLog.open(QIODevice::ReadWrite);
	mMagLog.write(tr("Time (msec),X (uT),Y (uT),Z (uT)\n").toUtf8());

	mGyroLog.open(QIODevice::ReadWrite);
	mGyroLog.write(tr("Time (msec),X (dps),Y (dps),Z (dps)\n").toUtf8());

	mPressureLog.open(QIODevice::ReadWrite);
	mPressureLog.write(tr("Time (msec),Pressure (Pa)\n").toUtf8());

	mTempLog.open(QIODevice::ReadWrite);
	mTempLog.write(tr("Time (msec),Temp (C)\n").toUtf8());
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

int64_t Driver10DOFModel::AccelTime(int idx) const
{
	if(idx < 0 || idx >= MAX_HISTORY)
		return 0;

	return mAccelTime[idx].toMSecsSinceEpoch();
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
		mAccelTime[i] = mAccelTime[i - 1];
		mAccelX[i] = mAccelX[i - 1];
		mAccelY[i] = mAccelY[i - 1];
		mAccelZ[i] = mAccelZ[i - 1];
	}

	// Save the new value.
	mAccelTime[0] = QDateTime::currentDateTime();
	mAccelX[0] = x;
	mAccelY[0] = y;
	mAccelZ[0] = z;

	// Emit the signal to the view.
	//std::cout << "Send accel to view..." << std::endl;
	emit NewAccel(x, y, z);

	QStringList row;
	row.append(QString::number(mAccelTime[0].toMSecsSinceEpoch()));
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

float Driver10DOFModel::AvgMagX() const
{
	if(mMagCount < MAG_AVG_COUNT)
		return mMagX[0];

	float val = 0.0f;
	
	for(int i = 0; i < MAG_AVG_COUNT; i++)
		val += mMagX[i];

	return val / MAG_AVG_COUNT;
}

float Driver10DOFModel::AvgMagY() const
{
	if(mMagCount < MAG_AVG_COUNT)
		return mMagY[0];

	float val = 0.0f;
	
	for(int i = 0; i < MAG_AVG_COUNT; i++)
		val += mMagY[i];

	return val / MAG_AVG_COUNT;
}

float Driver10DOFModel::AvgMagZ() const
{
	if(mMagCount < MAG_AVG_COUNT)
		return mMagZ[0];

	float val = 0.0f;
	
	for(int i = 0; i < MAG_AVG_COUNT; i++)
		val += mMagZ[i];

	return val / MAG_AVG_COUNT;
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
		mMagTime[i] = mMagTime[i - 1];
		mMagX[i] = mMagX[i - 1];
		mMagY[i] = mMagY[i - 1];
		mMagZ[i] = mMagZ[i - 1];
	}

	// Save the new value.
	mMagTime[0] = QDateTime::currentDateTime();
	mMagX[0] = x;
	mMagY[0] = y;
	mMagZ[0] = z;

	// Emit the signal to the view.
	//std::cout << "Send mag to view..." << std::endl;
	emit NewMag(x, y, z);

	QStringList row;
	row.append(QString::number(mMagTime[0].toMSecsSinceEpoch()));
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
		mGyroTime[i] = mGyroTime[i - 1];
		mGyroX[i] = mGyroX[i - 1];
		mGyroY[i] = mGyroY[i - 1];
		mGyroZ[i] = mGyroZ[i - 1];
	}

	// Save the new value.
	mGyroTime[0] = QDateTime::currentDateTime();
	mGyroX[0] = x;
	mGyroY[0] = y;
	mGyroZ[0] = z;

	// Emit the signal to the view.
	//std::cout << "Send gyro to view..." << std::endl;
	emit NewGyro(x, y, z);

	QStringList row;
	row.append(QString::number(mGyroTime[0].toMSecsSinceEpoch()));
	row.append(QString::number(x));
	row.append(QString::number(y));
	row.append(QString::number(z));
	mGyroLog.write(QString("%1\n").arg(row.join(",")).toUtf8());
	mGyroLog.flush();
}

float Driver10DOFModel::Pressure(int idx) const
{
	if(idx < 0 || idx >= MAX_HISTORY)
		return 0.0f;

	return mPressure[idx];
}

int Driver10DOFModel::NumPressureValues() const
{
	return mPressureCount;
}

void Driver10DOFModel::RecordPressure(float press)
{
	// Keep track of how many values.
	if(mPressureCount < MAX_HISTORY)
		mPressureCount++;

	// Shift the old values.
	for(size_t i = MAX_HISTORY - 1; i > 0; i--)
	{
		mPressureTime[i] = mPressureTime[i - 1];
		mPressure[i] = mPressure[i - 1];
	}

	// Save the new value.
	mPressureTime[0] = QDateTime::currentDateTime();
	mPressure[0] = press;

	// Emit the signal to the view.
	//std::cout << "Send pressure to view..." << std::endl;
	emit NewPressure(press);

	QStringList row;
	row.append(QString::number(mPressureTime[0].toMSecsSinceEpoch()));
	row.append(QString::number(press));
	mPressureLog.write(QString("%1\n").arg(row.join(",")).toUtf8());
	mPressureLog.flush();
}

float Driver10DOFModel::Temp(int idx) const
{
	if(idx < 0 || idx >= MAX_HISTORY)
		return 0.0f;

	return mTemp[idx];
}

int Driver10DOFModel::NumTempValues() const
{
	return mTempCount;
}

void Driver10DOFModel::RecordTemp(float temp)
{
	// Keep track of how many values.
	if(mTempCount < MAX_HISTORY)
		mTempCount++;

	// Shift the old values.
	for(size_t i = MAX_HISTORY - 1; i > 0; i--)
	{
		mTempTime[i] = mTempTime[i - 1];
		mTemp[i] = mTemp[i - 1];
	}

	// Save the new value.
	mTempTime[0] = QDateTime::currentDateTime();
	mTemp[0] = temp;

	// Emit the signal to the view.
	//std::cout << "Send temp to view..." << std::endl;
	emit NewTemp(temp);

	QStringList row;
	row.append(QString::number(mTempTime[0].toMSecsSinceEpoch()));
	row.append(QString::number(temp));
	mTempLog.write(QString("%1\n").arg(row.join(",")).toUtf8());
	mTempLog.flush();
}

void Driver10DOFModel::CleanClose()
{
	delete mDriver;
	mDriver = 0;
}
