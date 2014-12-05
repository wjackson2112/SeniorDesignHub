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
