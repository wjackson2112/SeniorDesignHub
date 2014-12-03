#ifndef __DriverDemoModel_h__
#define __DriverDemoModel_h__

#include <QtCore/QObject>

class DriverDemo;

const int MAX_HISTORY = 30;

class DriverDemoModel : public QObject
{
	Q_OBJECT

public:
	DriverDemoModel(DriverDemo *drv, QObject *parent = 0);

	int Value(int idx = 0) const;
	int RawValue(int idx = 0) const;
	int NumValues() const;

	int Minimum() const;
	int Maximum() const;

public slots:
	void LedOff();
	void LedRed();
	void LedGreen();
	void LedBlue();

	void SetMinimum(int value);
	void SetMaximum(int value);

signals:
	void NewValue(int adc);

private slots:
	void RecordValue(int adc);

private:
	DriverDemo *mDriver;

	int mCount;
	int mMin, mMax;
	int mValues[MAX_HISTORY];
};

#endif // __DriverDemoModel_h__
