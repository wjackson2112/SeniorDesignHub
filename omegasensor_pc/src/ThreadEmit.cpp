#include "ThreadEmit.h"

ThreadEmit::ThreadEmit(QObject *p) : QObject(p)
{
	// Nothing to see here.
}

void ThreadEmit::Emit()
{
	emit triggered();
}
