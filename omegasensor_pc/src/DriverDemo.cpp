#include "DriverDemo.h"
#include "DriverDemoView.h"
#include "DriverDemoModel.h"
#include "SensorHub.h"

QString DriverDemo::Name()
{
	return "UART Demo";
}

QWidget* DriverDemo::CreateView()
{
	DriverDemoView *view = new DriverDemoView();
	DriverDemoModel *model = new DriverDemoModel(this);
	view->SetDeviceName(mHub->HardwareName());
	view->SetModel(model);

	return view;
}

void DriverDemo::Recv(const GatoCharacteristic& characteristic,
	const QByteArray& data)
{
	if(characteristic.uuid() != mRX.uuid())
		return;

	bool ok = false;
	int value = QString::fromAscii(data).toUInt(&ok);
	if(ok)
		emit ADC(value);
}

DRIVER(DriverDemo)
