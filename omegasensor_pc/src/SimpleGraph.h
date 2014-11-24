#ifndef __SimpleGraph_h__
#define __SimpleGraph_h__

#include <QtCore/QList>

#include <QtGui/QWidget>

class SimpleGraph : public QWidget
{
	Q_OBJECT

public:
	SimpleGraph(QWidget *parent = 0);

	void SetData(const QList<float>& data);
	void SetRange(float min, float max);

protected:
	virtual void paintEvent(QPaintEvent *event);

private:
	float mMin, mMax;
	QList<float> mData;
};

#endif // __SimpleGraph_h__
