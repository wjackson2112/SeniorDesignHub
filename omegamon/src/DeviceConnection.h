#ifndef __DeviceConnection_h__
#define __DeviceConnection_h__

#include "ui_DeviceConnection.h"

#include "DeviceConnectionModel.h"

#include <QtGui/QWidget>

#include <gato/gatoaddress.h>
#include <gato/gatoperipheral.h>

class DeviceConnection : public QWidget
{
	Q_OBJECT

public:
	DeviceConnection(const GatoAddress& addr, QWidget *parent = 0);
	~DeviceConnection();

protected slots:
	void connected();
	void servicesDiscovered();
	void characteristicsDiscovered(const GatoService& serv);
	void descriptorsDiscovered(const GatoCharacteristic& characteristic);

protected:
	GatoPeripheral mDevice;
	DeviceConnectionModel mModel;

	Ui::DeviceConnection ui;
};

#endif // __DeviceConnection_h__
