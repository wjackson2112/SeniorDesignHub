#include "HubListModel.h"

#include "SensorHubService.h"
#include "SensorHub.h"

#include <QtCore/QUuid>
#include <QtCore/QStringList>

#include <stdint.h>

#define tr(s) QObject::trUtf8(s)

// Size of the red and green status lights for the sensor hubs.
const int ICON_SIZE = 64;

HubListModel::HubListModel(bool registeredOnly) : QAbstractListModel(),
	mRegisteredOnly(registeredOnly)
{
	SensorHubService::GetSingletonPtr()->StartScan();

	connect(&mTimer, SIGNAL(timeout()), this, SLOT(UpdateLastSeen()));

	mTimer.setSingleShot(false);
	mTimer.start(1000);

	mRed = QPixmap(":/red.png").scaled(ICON_SIZE, ICON_SIZE,
		Qt::KeepAspectRatio, Qt::SmoothTransformation);;
	mGreen = QPixmap(":/green.png").scaled(ICON_SIZE, ICON_SIZE,
		Qt::KeepAspectRatio, Qt::SmoothTransformation);;

	connect(SensorHubService::GetSingletonPtr(),
		SIGNAL(SensorHubChanged(const SensorHubPtr&)), this,
		SLOT(DeviceChanged(const SensorHubPtr&)));
	connect(SensorHubService::GetSingletonPtr(),
		SIGNAL(SensorHubDiscovered(const SensorHubPtr&)), this,
		SLOT(AddDevice(const SensorHubPtr&)));
}

HubListModel::~HubListModel()
{
	SensorHubService::GetSingletonPtr()->StopScan();
}

void HubListModel::AddDevice(const SensorHubPtr& dev)
{
	// Don't show new hubs if not requested.
	if(mRegisteredOnly && !SensorHubService::GetSingletonPtr(
		)->SensorHubIsRegistered(dev))
	{
		return;
	}
	else if(!mRegisteredOnly && SensorHubService::GetSingletonPtr(
		)->SensorHubIsRegistered(dev))
	{
		return;
	}

	QString addr = dev->GetAddress();

	if(mDeviceMap.contains(addr))
	{
		// Get the old device.
		SensorHubPtr hub = mDeviceMap.value(addr);
		int row = mDevices.indexOf(hub);

		// Replace with the new device.
		mDevices[row] = dev;
		mDeviceMap[addr] = dev;

		DeviceChanged(dev);
	}
	else
	{
		beginInsertRows(QModelIndex(), mDevices.count(), mDevices.count());
		mDevices.append(dev);
		mDeviceMap[addr] = dev;
		endInsertRows();
	}
}

void HubListModel::RemoveDevice(const SensorHubPtr& dev)
{
	// Disconnect the device if you remove it from the list.
	if(!dev.isNull() && dev->IsConnected())
		dev->Disconnect();

	int row = mDevices.indexOf(dev);
	if(row < 0)
		return;

	beginRemoveRows(QModelIndex(), row, row);
	mDevices.removeAt(row);
	mDeviceMap.remove(dev->GetAddress());
	endRemoveRows();
}

void HubListModel::DeviceChanged(const SensorHubPtr& dev)
{
	int row = mDevices.indexOf(dev);
	if(row >= 0)
		emit dataChanged(createIndex(row, 0), createIndex(row, 0));
	else
		AddDevice(dev);
}

void HubListModel::UpdateLastSeen()
{
	foreach(SensorHubPtr dev, mDevices)
		DeviceChanged(dev);
}

int HubListModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);

	return mDevices.count();
}

SensorHubPtr HubListModel::DeviceByRow(int row) const
{
	// Sanity check the row.
	if(row < 0 || row >= mDevices.count())
		return SensorHubPtr();

	return mDevices.at(row);
}

QVariant HubListModel::data(const QModelIndex& index, int role) const
{
	// Get the row for this list item.
	int row = index.row();

	// Sanity check the row.
	if(row < 0 || row >= mDevices.count())
		return QVariant();

	// Get the sensor hub object for this row.
	SensorHubPtr hub = mDevices.at(row);
	if(hub.isNull()) // This shouldn't happen.
		return QVariant();

	// Return the data depending on the role.
	if(role == Qt::DisplayRole)
	{
		// Get the sensor hub config.
		SensorHubConfigPtr config = SensorHubService::GetSingletonPtr(
			)->GetSensorHubConfig(hub);

		// Try to use the config name for the hub.
		QString name;
		if(!config.isNull())
			name = config->Name();

		// Fall back to the hardware name.
		if(name.isEmpty())
			name = hub->HardwareName();

		return tr("%1\nRSSI: %2 dBm\nMAC: %3").arg(
			name).arg(hub->GetRSSI()).arg(
			hub->GetAddress());
	}
	else if(role == Qt::DecorationRole)
	{
		// Was the device seen within the last 5 seconds?
		bool seenRecently = hub->LastSeen().addSecs(5) >
			QDateTime::currentDateTime();

		return (hub->IsConnected() || seenRecently) ? mGreen : mRed;
	}

	return QVariant();
}

QVariant HubListModel::headerData(int section, Qt::Orientation orientation,
	int role) const
{
	Q_UNUSED(section);
	Q_UNUSED(orientation);
	Q_UNUSED(role);

	return QVariant();
}

void HubListModel::Exterminate()
{
	if(mDevices.isEmpty())
		return;

	beginRemoveRows(QModelIndex(), 0, mDevices.count() - 1);
	mDevices.clear();
	mDeviceMap.clear();
	endRemoveRows();
}
