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

#ifndef __Compass_h__
#define __Compass_h__

#include <QtGui/QPixmap>
#include <QWidget>

class Compass : public QWidget
{
	Q_OBJECT

public:
	Compass(QWidget *parent = 0);

	void setDirection(float x, float y);

protected:
	virtual void paintEvent(QPaintEvent *evt);

private:
	QPixmap mCompass;
	QPixmap mCompassNeedle;
	float mDirection;
};

#endif // __Compass_h__
