#include "SimpleGraph.h"

#include <QtGui/QPainter>

const int MARGIN = 5;
const int POINT_WIDTH = 3;
const int LINE_WIDTH = 1;

SimpleGraph::SimpleGraph(QWidget *p) : QWidget(p),
	mMin(0), mMax(1)
{
	// Nothing to see here.
}

void SimpleGraph::SetData(const QList<int>& data)
{
	mData = data;
	repaint();
}

void SimpleGraph::SetRange(int min, int max)
{
	mMin = min;
	mMax = max;
}

void SimpleGraph::paintEvent(QPaintEvent *evt)
{
	float w = width() - 2 * MARGIN;
	float h = height() - 2 * MARGIN;
	float space = w / (mData.count() - 1);
	float scale = h / (float)(mMax - mMin);

	QPainter p(this);
	p.fillRect(rect(), Qt::transparent);
	p.setPen(QPen(palette().text(), POINT_WIDTH));

	QList<QPoint> points;

	for(int i = 0; i < mData.count(); i++)
	{
		int val = mData.at(i);
		int x = (int)(i * space + MARGIN + 0.5f);
		int y = (int)(scale * (val - mMin) + MARGIN + 0.5f);
		y = height() - y; // Flip the y-axis.

		p.drawPoint(x, y);
		points.append(QPoint(x, y));
	}

	p.setPen(QPen(palette().text(), LINE_WIDTH));
	p.drawRect(0, 0, width() - 1, height() - 1);

	for(int i = 0; i < (points.count() - 1); i++)
		p.drawLine(points.at(i), points.at(i + 1));
}
