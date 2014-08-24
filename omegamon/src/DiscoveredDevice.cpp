#include "DiscoveredDevice.h"

#include <QtGui/QStandardItem>

DiscoveredDevice::DiscoveredDevice(const QString& name, const QString& addr,
	quint8 addrType, quint8 advertType, const QByteArray& advertData,
	int rssi) : mRSSI(rssi), mName(name),mAddress(addr),
	mAddressType(addrType), mAdvertType(advertType), mAdvertData(advertData)
{
	// Nothing to see here.
}

int DiscoveredDevice::getRSSI() const
{
	return mRSSI;
}

void DiscoveredDevice::setRSSI(int rssi)
{
	mRSSI = rssi;
}

QString DiscoveredDevice::getName() const
{
	return mName;
}

void DiscoveredDevice::setName(const QString& name)
{
	mName = name;
}

QString DiscoveredDevice::getAddress() const
{
	return mAddress;
}

void DiscoveredDevice::setAddress(const QString& addr)
{
	mAddress = addr;
}

quint8 DiscoveredDevice::getAddressType() const
{
	return mAddressType;
}

void DiscoveredDevice::setAddressType(quint8 addrType)
{
	mAddressType = addrType;
}

quint8 DiscoveredDevice::getAdvertType() const
{
	return mAdvertType;
}

void DiscoveredDevice::setAdvertType(quint8 advertType)
{
	mAdvertType = advertType;
}

QByteArray DiscoveredDevice::getAdvertData() const
{
	return mAdvertData;
}

void DiscoveredDevice::setAdvertData(const QByteArray& advertData)
{
	mAdvertData = advertData;
}

QDateTime DiscoveredDevice::getLastSeen() const
{
	return mLastSeen;
}

void DiscoveredDevice::setLastSeen(const QDateTime& timestamp)
{
	mLastSeen = timestamp;
}

QStandardItem* DiscoveredDevice::getItem(const QString& name) const
{
	return mItems.value(name);
}

void DiscoveredDevice::setItem(const QString& name, QStandardItem *item)
{
	mItems[name] = item;
}

void DiscoveredDevice::copyItems(DiscoveredDevice *other)
{
	mItems = other->mItems;
}

void DiscoveredDevice::disableAllItems()
{
	foreach(QStandardItem *item, mItems)
		item->setEnabled(false);
}
