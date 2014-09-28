#ifndef __Driver_h__
#define __Driver_h__

#include <QtCore/QObject>
#include <QtCore/QString>

class QWidget;
class SensorHub;

class Driver : public QObject
{
	Q_OBJECT

public:
	virtual QWidget* CreateView() { return 0; }
	virtual void Initialize(SensorHub *hub) { mHub = hub; }

protected:
	SensorHub *mHub;
};

#define DRIVER(name) Driver* Construct##name() { return new name(); } \
	QString GetName##name() { return name::Name(); };

#endif // __Driver_h__
