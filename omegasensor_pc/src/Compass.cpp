#include "Compass.h"

#include <QtGui/QPainter>

#include <cmath>
#include <iostream>

Compass::Compass(QWidget *p) : QWidget(p), mCompass(":/compass.png"),
	mCompassNeedle(":/compass_needle.png"), mDirection(0.0f)
{
	// Nothing to see here.
}

void Compass::paintEvent(QPaintEvent *evt)
{
	Q_UNUSED(evt);

	QPainter p(this);
	p.drawPixmap(0, 0, width(), height(), mCompass);
	p.translate(width() / 2, height() / 2);
	p.rotate(mDirection);
	p.translate(width() / -2, height() / -2);
	p.drawPixmap(0, 0, width(), height(), mCompassNeedle);
}

void Compass::setDirection(float x, float y)
{
	x *= -1;
	y *= -1;

	// 30, -60, 10
	// 2, 36
	x += 30;
	y -= 50;

	//x = 0, y = 22.7, z = don't give a fuck
	// y = 27.4 (car off)
	// y = 16.3

	// -38, 2
	//x -= 38;
	//y += 0;//20.7;

	std::cout << "x: " << x << " | y: " << y << std::endl;
	float mag = sqrt(x * x + y * y);
	x /= mag;
	y /= mag;

	mDirection = atan2(x, y) * (180.0f / M_PI);// + 180.0f;
	if(mDirection > 360.0f)
		mDirection -= 360.0f;
	else if(mDirection < 360.0f)
		mDirection += 360.0f;

	repaint();
}
