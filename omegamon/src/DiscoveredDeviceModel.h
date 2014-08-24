#ifndef __DiscoveredDeviceModel_h__
#define __DiscoveredDeviceModel_h__

#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QTimer>

#include <QtGui/QStandardItemModel>

class DiscoveredDevice;

Q_DECLARE_METATYPE(DiscoveredDevice*);

class DiscoveredDeviceModel : public QObject
{
	Q_OBJECT

public:
	DiscoveredDeviceModel();

	void addDevice(DiscoveredDevice *dev);

	QStandardItemModel* model();

private slots:
	void updateLastSeen();

private:
	void updateItem(DiscoveredDevice *dev);
	void updateAdvertData(DiscoveredDevice *dev);
	void updateText(DiscoveredDevice *dev, QStandardItem *root,
		const QString& key, const QString& text);

	QTimer mTimer;
	QStandardItemModel mModel;
	QList<DiscoveredDevice*> mDevices;
	QHash<QString, DiscoveredDevice*> mDeviceMap;
};

#endif // __DiscoveredDeviceModel_h__
