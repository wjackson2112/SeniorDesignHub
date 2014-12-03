#ifndef __ThreadEmit_h__
#define __ThreadEmit_h__

#include <QtCore/QObject>

class ThreadEmit : public QObject
{
	Q_OBJECT

public:
	ThreadEmit(QObject *parent = 0);

	void Emit();

signals:
	void triggered();
};

#endif // __ThreadEmit_h__

