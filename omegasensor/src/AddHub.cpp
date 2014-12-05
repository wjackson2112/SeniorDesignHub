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

#include "AddHub.h"
#include "HubListModel.h"
#include "SensorHubService.h"

AddHub::AddHub(QWidget *p) : QDialog(p)
{
	mModel = new HubListModel(false);

	ui.setupUi(this);
	ui.addButton->setEnabled(false);
	ui.hubList->setModel(mModel);

	connect(ui.addButton, SIGNAL(clicked(bool)),
		this, SLOT(AddClicked()));
	connect(ui.hubList, SIGNAL(doubleClicked(const QModelIndex&)),
		this, SLOT(doubleClicked(const QModelIndex&)));
	connect(ui.hubList->selectionModel(), SIGNAL(selectionChanged(
		const QItemSelection&, const QItemSelection&)), this,
		SLOT(selectionChanged(const QItemSelection&,
		const QItemSelection&)));

#ifdef Q_OS_ANDROID
	setWindowState(Qt::WindowMaximized);
#endif // Q_OS_ANDROID
}

AddHub::~AddHub()
{
	// Nothing to see here.
}

void AddHub::showEvent(QShowEvent *evt)
{
	mModel->Exterminate();

	QDialog::showEvent(evt);
}

void AddHub::selectionChanged(const QItemSelection& selected,
	const QItemSelection& deselected)
{
	Q_UNUSED(deselected);

	ui.addButton->setEnabled(!selected.isEmpty());
}

void AddHub::doubleClicked(const QModelIndex& index)
{
	SensorHubPtr hub = mModel->DeviceByRow(index.row());
	if(hub.isNull())
		return;

	SensorHubService::GetSingletonPtr()->RegisterHub(hub);

	close();
}

void AddHub::AddClicked()
{
	if(ui.hubList->selectionModel()->selectedRows().isEmpty())
		return;

	SensorHubPtr hub = mModel->DeviceByRow(ui.hubList->selectionModel(
		)->selectedRows().first().row());
	if(hub.isNull())
		return;

	SensorHubService::GetSingletonPtr()->RegisterHub(hub);

	close();
}
