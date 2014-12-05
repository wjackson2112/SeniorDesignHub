#include "DeviceConnectionModel.h"
#include "DeviceConnection.h"

#include <QtCore/QUuid>
#include <QtCore/QStringList>

#include <stdint.h>

#include <gato/gatocharacteristic.h>

#define tr(s) QObject::trUtf8(s)

DeviceConnectionModel::DeviceConnectionModel() : QObject()
{
	// Nothing to see here.
}

QString DeviceConnectionModel::getServiceName(const QString& uuid) const
{
	static QMap<QString, QString> commonServiceMapping;

	if(commonServiceMapping.isEmpty())
	{
		commonServiceMapping["{00001800-0000-1000-8000-00805f9b34fb}"] =
			tr("Generic Access Protocol");
		commonServiceMapping["{00001801-0000-1000-8000-00805f9b34fb}"] =
			tr("Generic Attribute Protocol");
		commonServiceMapping["{7b1e3740-1dc8-11e4-8c21-0800200c9a66}"] =
			tr("Omega Sensor Hub UART");
	}

	if(commonServiceMapping.contains(uuid))
		return tr("%1 (%2)").arg(commonServiceMapping.value(uuid)).arg(
			uuid.mid(1, uuid.size() - 2));

	return uuid;
}

void DeviceConnectionModel::addService(const GatoService& serv)
{
	QString uuid = serv.uuid().toString();

	if(mServiceMap.contains(uuid))
	{
	}
	else
	{
		mServices.append(serv);
		mServiceMap[uuid] = serv;

		QStandardItem *item = new QStandardItem;
		item->setEditable(false);
		item->setText(getServiceName(uuid));
		mModel.appendRow(item);
	}

	qDebug() << "Number of characteristics: " << serv.characteristics().size();
	foreach(GatoCharacteristic c, serv.characteristics())
	{
		qDebug() << c.uuid().toString();
	}

	//updateService(???);
}

QStandardItemModel* DeviceConnectionModel::model()
{
	return &mModel;
}

void DeviceConnectionModel::updateService(const GatoService& serv)
{
	/*
	static const QString advTypes[] = {
		"Connectable",
		"Directed",
		"Scannable",
		"Non-connectable"
	};

	QStandardItem *item = dev->getItem("root");
	item->setData(QVariant::fromValue<DeviceConnection*>(dev));
	if(!item)
		return;

	dev->disableAllItems();
	item->setEnabled(true);

	if(dev->getName().isEmpty())
	{
		item->setText(tr("%1 (%2dBm)").arg(
			dev->getAddress()).arg(
			dev->getRSSI()));
	}
	else
	{
		item->setText(tr("%1 (%2) (%3dBm)").arg(
			dev->getName()).arg(
			dev->getAddress()).arg(
			dev->getRSSI()));
	}

	QDateTime now = QDateTime::currentDateTime();
	dev->setLastSeen(now);

	updateText(dev, item, "last_seen", tr("Last Seen: 0 sec"));
	updateText(dev, item, "rssi", tr("RSSI: %1dBm").arg(dev->getRSSI()));
	updateText(dev, item, "addr", tr("Address: %1").arg(dev->getAddress()));
	updateText(dev, item, "addr_type", tr("Address Type: %1").arg(
		dev->getAddressType() == 0 ? "Public" : "Random"));
	updateText(dev, item, "adv_type", tr("Advertising Type: %1").arg(
		advTypes[dev->getAdvertType()]));
	updateText(dev, item, "bond", tr("Bonded: %1").arg(
		"False"));

	updateText(dev, item, "adv_data", tr("Advertising Data"));
	updateAdvertData(dev);

	updateText(dev, item, "scan_resp", tr("Scan Response Data"));
	*/
}

void DeviceConnectionModel::updateText(DeviceConnection *dev,
	QStandardItem *root, const QString& key,
	const QString& text)
{
	/*
	QStandardItem *item = dev->getService(key);
	if(!item)
	{
		item = new QStandardItem;
		item->setEditable(false);

		root->appendRow(item);
		dev->setItem(key, item);
	}

	item->setEnabled(true);

	if(item->text() != text)
		item->setText(text);
	*/
}
