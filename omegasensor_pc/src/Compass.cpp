#include "Compass.h"

#include <QtGui/QPainter>

#include <cmath>

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
	float mag = sqrt(x * x + y * y);
	x /= mag;
	y /= mag;

	mDirection = atan2(x, y) * (180.0f / M_PI);

	repaint();
}
