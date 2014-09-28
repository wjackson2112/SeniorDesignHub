#include "DriverDemoModel.h"
#include "DriverDemo.h"

DriverDemoModel::DriverDemoModel(DriverDemo *drv,
	QObject * p) : QObject(p), mDriver(drv), mCount(0), mMin(0), mMax(1023)
{
	for(size_t i = 0; i < MAX_HISTORY; i++)
		mValues[i] = 0;

	connect(drv, SIGNAL(ADC(int)), this, SLOT(RecordValue(int)));
}

int DriverDemoModel::Value(int idx) const
{
	if(idx < 0 || idx >= MAX_HISTORY)
		return mMin;

	if(mValues[idx] < mMin)
		return mMin;
	else if(mValues[idx] > mMax)
		return mMax;
	else
		return mValues[idx];
}

int DriverDemoModel::RawValue(int idx) const
{
	if(idx < 0 || idx >= MAX_HISTORY)
		return mMin;

	return mValues[idx];
}

int DriverDemoModel::NumValues() const
{
	return mCount;
}

int DriverDemoModel::Minimum() const
{
	return mMin;
}

int DriverDemoModel::Maximum() const
{
	return mMax;
}

void DriverDemoModel::LedOff()
{
	Q_ASSERT(mDriver);
	mDriver->Send("0");
}

void DriverDemoModel::LedRed()
{
	Q_ASSERT(mDriver);
	mDriver->Send("1");
}

void DriverDemoModel::LedGreen()
{
	Q_ASSERT(mDriver);
	mDriver->Send("2");
}

void DriverDemoModel::LedBlue()
{
	Q_ASSERT(mDriver);
	mDriver->Send("3");
}

void DriverDemoModel::SetMinimum(int value)
{
	mMin = value;
}

void DriverDemoModel::SetMaximum(int value)
{
	mMax = value;
}

void DriverDemoModel::RecordValue(int adc)
{
	// Keep track of how many values.
	if(mCount < MAX_HISTORY)
		mCount++;

	// Shift the old values.
	for(size_t i = MAX_HISTORY; i > 0; i--)
		mValues[i] = mValues[i - 1];

	// Save the new value.
	mValues[0] = adc;

	// Emit the signal to the view.
	if(adc < mMin)
		emit NewValue(mMin);
	else if(adc > mMax)
		emit NewValue(mMax);
	else
		emit NewValue(adc);
}
