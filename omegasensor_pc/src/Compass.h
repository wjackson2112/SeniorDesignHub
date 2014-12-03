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
