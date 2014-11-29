#ifndef __Driver10DOF_h__
#define __Driver10DOF_h__

#include "DriverI2C.h"

class Driver10DOF : public DriverI2C
{
	Q_OBJECT

public:
	virtual ~Driver10DOF() { }
	static QString Name();
	virtual QWidget* CreateView();
	virtual void Initialize(const SensorHubPtr& hub);
	virtual void Recv(uint16_t characteristic, const QByteArray& data);

signals:
	void Accel(float x, float y, float z);
	void Mag(float x, float y, float z);
	void Gyro(float x, float y, float z);

protected slots:
	void Sample();
};

#endif // __Driver10DOF_h__
