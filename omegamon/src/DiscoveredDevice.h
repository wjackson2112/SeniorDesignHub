#ifndef __DiscoveredDevice_h__
#define __DiscoveredDevice_h__

#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QByteArray>

class QStandardItem;

class DiscoveredDevice
{
public:
	DiscoveredDevice(const QString& name, const QString& addr,
		quint8 addrType, quint8 advertType,
		const QByteArray& advertData, int rssi);

	int getRSSI() const;
	void setRSSI(int rssi);

	QString getName() const;
	void setName(const QString& name);

	QString getAddress() const;
	void setAddress(const QString& addr);

	quint8 getAddressType() const;
	void setAddressType(quint8 addrType);

	quint8 getAdvertType() const;
	void setAdvertType(quint8 advertType);

	QByteArray getAdvertData() const;
	void setAdvertData(const QByteArray& advertData);

	QDateTime getLastSeen() const;
	void setLastSeen(const QDateTime& timestamp);

	QStandardItem* getItem(const QString& name) const;
	void setItem(const QString& name, QStandardItem *item);
	void copyItems(DiscoveredDevice *other);
	void disableAllItems();

private:
	int mRSSI;
	QString mName;
	QString mAddress;
	quint8 mAddressType;
	quint8 mAdvertType;
	QByteArray mAdvertData;
	QDateTime mLastSeen;
	QMap<QString, QStandardItem*> mItems;
};

#endif // __DiscoveredDevice_h__
