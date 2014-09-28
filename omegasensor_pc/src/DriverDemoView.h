#ifndef __DriverDemoView_h__
#define __DriverDemoView_h__

#include "ui_DriverDemo.h"

class DriverDemoModel;

class DriverDemoView : public QWidget
{
	Q_OBJECT

public:
	DriverDemoView(QWidget *parent = 0);

	void SetModel(DriverDemoModel *model);
	void SetDeviceName(const QString& name);

public slots:
	void SetMin();
	void SetMax();
	void Reset();
	void Update();

private:
	QString mDeviceName;
	DriverDemoModel *mModel;

	Ui::DriverDemo ui;
};

#endif // __DriverDemoView_h__
