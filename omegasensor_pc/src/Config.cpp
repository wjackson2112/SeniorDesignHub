#include "Config.h"
#include "SensorHubService.h"
#include <iostream>
Config::Config(const SensorHubPtr& hub, QWidget *p) : QDialog(p)
{
	ui.setupUi(this);

	setAttribute(Qt::WA_DeleteOnClose);

	mConfig = SensorHubService::GetSingletonPtr(
		)->GetSensorHubConfig(hub);

	ui.name->setText(mConfig->Name());
	ui.hwName->setText(mConfig->HardwareName());
	ui.hwAddress->setText(mConfig->GetAddress());

	connect(ui.okButton, SIGNAL(clicked(bool)),
		this, SLOT(SaveConfig()));

	// Get the list of drivers and populate the combo boxes.
	QMultiMap<Driver::DriverType, QString> drivers =
		SensorHubService::GetSingletonPtr()->DriversByType();

	QStringList uartDrivers, i2cDrivers, spiDrivers;
	QStringList analogDrivers, digitalDrivers;

	foreach(QString driver, drivers.values(Driver::DriverType_UART))
	{
		uartDrivers.append(driver);
		ui.uartDrivers->addItem(driver);
	}

	ui.uartDrivers->setCurrentIndex(uartDrivers.indexOf(
		mConfig->DriverUART()) + 1);

	foreach(QString driver, drivers.values(Driver::DriverType_I2C))
	{
		i2cDrivers.append(driver);
		ui.i2cDrivers->addItem(driver);
	}

	ui.i2cDrivers->setCurrentIndex(i2cDrivers.indexOf(
		mConfig->DriverI2C()) + 1);

	foreach(QString driver, drivers.values(Driver::DriverType_SPI))
	{
		spiDrivers.append(driver);
		ui.spiDrivers->addItem(driver);
	}

	ui.spiDrivers->setCurrentIndex(spiDrivers.indexOf(
		mConfig->DriverSPI()) + 1);

	foreach(QString driver, drivers.values(Driver::DriverType_Analog))
	{
		analogDrivers.append(driver);
		ui.analogDrivers->addItem(driver);
	}

	ui.analogDrivers->setCurrentIndex(analogDrivers.indexOf(
		mConfig->DriverAnalog()) + 1);

	foreach(QString driver, drivers.values(Driver::DriverType_Digital))
	{
		digitalDrivers.append(driver);
		ui.digitalDrivers->addItem(driver);
	}

	ui.digitalDrivers->setCurrentIndex(digitalDrivers.indexOf(
		mConfig->DriverDigital()) + 1);
}

void Config::SaveConfig()
{
	mConfig->SetName(ui.name->text());

	if(ui.uartDrivers->currentIndex() > 0)
		mConfig->SetDriverUART(ui.uartDrivers->currentText());
	else
		mConfig->SetDriverUART(QString());

	if(ui.i2cDrivers->currentIndex() > 0)
		mConfig->SetDriverI2C(ui.i2cDrivers->currentText());
	else
		mConfig->SetDriverI2C(QString());

	if(ui.spiDrivers->currentIndex() > 0)
		mConfig->SetDriverSPI(ui.spiDrivers->currentText());
	else
		mConfig->SetDriverSPI(QString());

	if(ui.analogDrivers->currentIndex() > 0)
		mConfig->SetDriverAnalog(ui.uartDrivers->currentText());
	else
		mConfig->SetDriverAnalog(QString());

	if(ui.digitalDrivers->currentIndex() > 0)
		mConfig->SetDriverDigital(ui.uartDrivers->currentText());
	else
		mConfig->SetDriverDigital(QString());

	SensorHubService::GetSingletonPtr()->SaveConfig();
	close();
}
