#ifndef __SimpleGraph_h__
#define __SimpleGraph_h__

#include <QtCore/QList>

#include <QtGui/QWidget>

class SimpleGraph : public QWidget
{
	Q_OBJECT

public:
	SimpleGraph(QWidget *parent = 0);

	void SetData(const QList<int>& data);
	void SetRange(int min, int max);

protected:
	virtual void paintEvent(QPaintEvent *event);

private:
	int mMin, mMax;
	QList<int> mData;
};

#endif // __SimpleGraph_h__
