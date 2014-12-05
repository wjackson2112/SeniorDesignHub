#ifndef __DeviceConnectionModel_h__
#define __DeviceConnectionModel_h__

#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QTimer>

#include <QtGui/QStandardItemModel>

#include <gato/gatoservice.h>

class DeviceConnection;

Q_DECLARE_METATYPE(DeviceConnection*);

class DeviceConnectionModel : public QObject
{
	Q_OBJECT

public:
	DeviceConnectionModel();

	void addService(const GatoService& serv);

	QStandardItemModel* model();

private:
	QString getServiceName(const QString& uuid) const;

	void updateService(const GatoService& serv);
	void updateText(DeviceConnection *dev, QStandardItem *root,
		const QString& key, const QString& text);

	QStandardItemModel mModel;
	QList<GatoService> mServices;
	QHash<QString, GatoService> mServiceMap;
};

#endif // __DeviceConnectionModel_h__
