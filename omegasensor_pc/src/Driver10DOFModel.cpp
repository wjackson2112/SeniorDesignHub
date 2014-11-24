#include "Driver10DOFModel.h"
#include "Driver10DOF.h"

#include <QtCore/QStringList>

#include <iostream>

Driver10DOFModel::Driver10DOFModel(Driver10DOF *drv,
	QObject * p) : QObject(p), mDriver(drv), mAccelCount(0),
	mLog("10dof.csv")
{
	connect(&mTimer, SIGNAL(timeout()), drv, SLOT(Sample()));

	mTimer.setSingleShot(false);
	mTimer.start(100);

	for(size_t i = 0; i < MAX_HISTORY; i++)
	{
		mAccelX[i] = 0.0f;
		mAccelY[i] = 0.0f;
		mAccelZ[i] = 0.0f;
	}

	connect(drv, SIGNAL(Accel(float, float, float)),
		this, SLOT(RecordAccel(float, float, float)));

	mLog.open(QIODevice::ReadWrite);
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
	for(size_t i = MAX_HISTORY; i > 0; i--)
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
	std::cout << "Send to model..." << std::endl;
	emit NewAccel(x, y, z);

	QStringList row;
	row.append(QString::number(x));
	row.append(QString::number(y));
	row.append(QString::number(z));
	mLog.write(QString("%1\n").arg(row.join(",")).toUtf8());
	mLog.flush();
}
