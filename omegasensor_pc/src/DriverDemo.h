#ifndef __DriverDemo_h__
#define __DriverDemo_h__

#include "DriverUART.h"

class DriverDemo : public DriverUART
{
	Q_OBJECT

public:
	static QString Name();
	virtual QWidget* CreateView();
	virtual void Recv(const GatoCharacteristic& characteristic,
		const QByteArray& data);

signals:
	void ADC(int value);
};

#endif // __DriverDemo_h__
