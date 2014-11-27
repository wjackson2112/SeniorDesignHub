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

	typedef enum _State
	{
		State_Wait,
		State_Accel,
		State_Mag,
		State_Gyro
	}State;

signals:
	void Accel(float x, float y, float z);
	void Mag(float x, float y, float z);
	void Gyro(float x, float y, float z);

protected slots:
	void Sample();

private:
	State mCurrentState;
};

#endif // __Driver10DOF_h__
