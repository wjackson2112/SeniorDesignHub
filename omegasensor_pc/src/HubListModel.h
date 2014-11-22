#ifndef __HubListModel_h__
#define __HubListModel_h__

#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QTimer>

#include <QtCore/QAbstractListModel>

#include <QtGui/QPixmap>

#include "SensorHub.h"

class HubListModel : public QAbstractListModel
{
	Q_OBJECT

public:
	HubListModel(bool registeredOnly = true);
	virtual ~HubListModel();

	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex& index,
		int role = Qt::DisplayRole) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation,
		int role = Qt::DisplayRole) const;
	void Exterminate();

	SensorHubPtr DeviceByRow(int row) const;

public slots:
	void AddDevice(const SensorHubPtr& dev);
	void RemoveDevice(const SensorHubPtr& dev);
	void DeviceChanged(const SensorHubPtr& dev);
	void UpdateLastSeen();

private:
	/*void updateItem(HubList *dev);
	void updateAdvertData(HubList *dev);
	void updateText(HubList *dev, QStandardItem *root,
		const QString& key, const QString& text);*/

	QTimer mTimer;
	QList<SensorHubPtr> mDevices;
	QHash<QString, SensorHubPtr> mDeviceMap;

	QPixmap mRed, mGreen;

	bool mRegisteredOnly;
};

#endif // __HubListModel_h__
