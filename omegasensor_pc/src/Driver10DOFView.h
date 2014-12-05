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

#ifndef __Driver10DOFView_h__
#define __Driver10DOFView_h__

#include "ui_Driver10DOF.h"

class Driver10DOFModel;

class Driver10DOFView : public QWidget
{
	Q_OBJECT

public:
	Driver10DOFView(QWidget *parent = 0);

	void SetModel(Driver10DOFModel *model);
	void SetDeviceName(const QString& name);

protected:
	virtual void closeEvent(QCloseEvent *evt);

protected slots:
	void Update();

private:
	QString mDeviceName;
	Driver10DOFModel *mModel;

	Ui::Driver10DOF ui;

	float mSpeed;

	float minAccelX, maxAccelX;
	float minAccelY, maxAccelY;
	float minAccelZ, maxAccelZ;

	float minMagX, maxMagX;
	float minMagY, maxMagY;
	float minMagZ, maxMagZ;

	float minGyroX, maxGyroX;
	float minGyroY, maxGyroY;
	float minGyroZ, maxGyroZ;
};

#endif // __Driver10DOFView_h__
