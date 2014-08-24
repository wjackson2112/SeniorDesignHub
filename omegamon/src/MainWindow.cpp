#include "MainWindow.h"
#include "DeviceConnection.h"
#include "DiscoveredDevice.h"

#include <gato/gatoperipheral.h>

MainWindow::MainWindow(QWidget *p) : QMainWindow(p), mScanning(true)
{
	ui.setupUi(this);

	// Connect the manager signals.
	connect(&mManager, SIGNAL(discoveredPeripheral(GatoPeripheral*, quint8, int)),
		this, SLOT(discoveredPeripheral(GatoPeripheral*, quint8, int)));
	connect(ui.scanToggle, SIGNAL(clicked()),
		this, SLOT(toggleDiscovery()));
	connect(ui.deviceList, SIGNAL(doubleClicked(const QModelIndex&)),
		this, SLOT(itemDoubleClicked(const QModelIndex&)));
	connect(ui.selectButton, SIGNAL(clicked()),
		this, SLOT(selectDevice()));

	// Start scanning for bluetooth devices.
	mManager.scanForPeripherals();

	ui.deviceList->setModel(mModel.model());
	ui.deviceList->header()->hide();
	ui.scanToggle->setText(tr("Stop Discovery"));
}

MainWindow::~MainWindow()
{
	// Tell the bluetooth device to stop scanning or things WILL break.
	mManager.stopScan();
}

void MainWindow::toggleDiscovery()
{
	if(mScanning)
	{
		mManager.stopScan();
		ui.scanToggle->setText(tr("Start Discovery"));
	}
	else
	{
		mManager.scanForPeripherals();
		ui.scanToggle->setText(tr("Stop Discovery"));
	}

	mScanning = !mScanning;
}

void MainWindow::discoveredPeripheral(GatoPeripheral *peripheral,
	quint8 advertType, int rssi)
{
	DiscoveredDevice *dev = new DiscoveredDevice(peripheral->name(),
		peripheral->address().toString(),
		peripheral->address().addressType(),
		advertType, peripheral->advertData(), rssi);
	mModel.addDevice(dev);
}

void MainWindow::selectDevice()
{
	if(ui.deviceList->selectionModel()->selectedIndexes().isEmpty())
		return;

	QStandardItem *item = mModel.model()->itemFromIndex(
		ui.deviceList->selectionModel()->selectedIndexes().first());
	if(!item)
		return;

	while(item->parent())
		item = item->parent();

	DiscoveredDevice *dev = item->data().value<DiscoveredDevice*>();
	if(!dev)
		return;

	if(mConnections.contains(dev->getAddress()))
		return;

	DeviceConnection *conn = new DeviceConnection(
		GatoAddress(dev->getAddress(), dev->getAddressType()));
	mConnections[dev->getAddress()] = conn;
	conn->show();
}

void MainWindow::itemDoubleClicked(const QModelIndex& index)
{
	QStandardItem *item = mModel.model()->itemFromIndex(index);
	if(!item)
		return;

	while(item->parent())
		item = item->parent();

	DiscoveredDevice *dev = item->data().value<DiscoveredDevice*>();
	if(!dev)
		return;

	if(mConnections.contains(dev->getAddress()))
		return;

	DeviceConnection *conn = new DeviceConnection(
		GatoAddress(dev->getAddress(), dev->getAddressType()));
	mConnections[dev->getAddress()] = conn;
	conn->show();
}
