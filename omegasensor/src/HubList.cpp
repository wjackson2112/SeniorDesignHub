/*
 * Omega Wireless Sensor Module
 * Copyright (C) 2014 Team Omega
 * Viva la Resistance
 *
 * Hardware:    Bogdan Crivin <bcrivin@uccs.edu>
 * Firmware:    William Jackson <wjackson@uccs.edu>
 * Software:    John Martin <jmarti25@uccs.edu>
 * Integration: Angela Askins <aaskins@uccs.edu>
 */

#include "HubList.h"
#include "HubListModel.h"
#include "SensorHubService.h"
#include "AddHub.h"
#include "Config.h"

#ifdef Q_OS_ANDROID
#include "SensorHubAndroidService.h"
#endif // Q_OS_ANDROID

#include <QMessageBox>

HubList::HubList() : QMainWindow()
{
	mModel = new HubListModel;
	mAddDialog = new AddHub(this);

	ui.setupUi(this);
	ui.removeButton->setEnabled(false);
	ui.configButton->setEnabled(false);
	ui.hubList->setModel(mModel);

	connect(ui.hubList, SIGNAL(doubleClicked(const QModelIndex&)),
		this, SLOT(doubleClicked(const QModelIndex&)));
	connect(ui.addButton, SIGNAL(clicked(bool)), mAddDialog, SLOT(show()));
	connect(ui.configButton, SIGNAL(clicked(bool)), this, SLOT(ShowConfig()));
	connect(ui.removeButton, SIGNAL(clicked(bool)), this, SLOT(RemoveHub()));
	connect(ui.hubList->selectionModel(), SIGNAL(selectionChanged(
		const QItemSelection&, const QItemSelection&)), this,
		SLOT(selectionChanged(const QItemSelection&,
		const QItemSelection&)));
}

void HubList::doubleClicked(const QModelIndex& index)
{
#ifdef Q_OS_ANDROID
	dynamic_cast<SensorHubAndroidService*>(SensorHubService::GetSingletonPtr()
		)->CreateEmitters();
#endif // Q_OS_ANDROID

	SensorHubPtr hub = mModel->DeviceByRow(index.row());
	if(hub.isNull()) // Should never happen.
		return;

	// Don't try to connect again.
	if(hub->IsConnected())
	{
		QMessageBox::warning(this, tr("Already Connected"),
			tr("You are already connected to this sensor hub!"));

		return;
	}

	SensorHubConfigPtr config =  SensorHubService::GetSingletonPtr(
		)->GetSensorHubConfig(hub);
	if(config.isNull()) // Should never happen.
		return;

	if(config->DriverList().isEmpty())
	{
		QMessageBox::warning(this, tr("No Drivers"),
			tr("Please configure the sensor hub with a driver first!"));

		return;
	}

	hub->Connect();
}

void HubList::RemoveHub()
{
	if(ui.hubList->selectionModel()->selectedRows().isEmpty())
		return;

	SensorHubPtr hub = mModel->DeviceByRow(ui.hubList->selectionModel(
		)->selectedRows().first().row());
	if(hub.isNull())
		return;

	SensorHubService::GetSingletonPtr()->UnRegisterHub(hub);

	mModel->RemoveDevice(hub);
}

void HubList::selectionChanged(const QItemSelection& selected,
	const QItemSelection& deselected)
{
	Q_UNUSED(deselected);

	ui.removeButton->setEnabled(!selected.isEmpty());
	ui.configButton->setEnabled(!selected.isEmpty());
}

void HubList::ShowConfig()
{
	if(ui.hubList->selectionModel()->selectedRows().isEmpty())
		return;

	SensorHubPtr hub = mModel->DeviceByRow(ui.hubList->selectionModel(
		)->selectedRows().first().row());
	if(hub.isNull())
		return;

	Config *c = new Config(hub, this);
	c->show();
}
