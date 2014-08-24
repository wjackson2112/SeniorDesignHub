#include "DiscoveredDeviceModel.h"
#include "DiscoveredDevice.h"

#include <QtCore/QUuid>
#include <QtCore/QStringList>

#include <stdint.h>

#define tr(s) QObject::trUtf8(s)

DiscoveredDeviceModel::DiscoveredDeviceModel() : QObject()
{
	connect(&mTimer, SIGNAL(timeout()), this, SLOT(updateLastSeen()));

	mTimer.setSingleShot(false);
	mTimer.start(5000);
}

void DiscoveredDeviceModel::addDevice(DiscoveredDevice *dev)
{
	QString addr = dev->getAddress();

	if(mDeviceMap.contains(addr))
	{
		DiscoveredDevice *oldDev = mDeviceMap.value(addr);

		mDevices.replace(mDevices.indexOf(oldDev), dev);
		mDeviceMap[addr] = dev;
		dev->copyItems(oldDev);
		dev->setLastSeen(oldDev->getLastSeen());

		delete oldDev;
	}
	else
	{
		mDevices.append(dev);
		mDeviceMap[addr] = dev;

		QStandardItem *item = new QStandardItem;
		dev->setItem("root", item);
		item->setEditable(false);
		mModel.appendRow(item);
	}

	updateItem(dev);
}

QStandardItemModel* DiscoveredDeviceModel::model()
{
	return &mModel;
}

void DiscoveredDeviceModel::updateLastSeen()
{
	QDateTime now = QDateTime::currentDateTime();

	foreach(DiscoveredDevice *dev, mDevices)
	{
		if(!dev->getLastSeen().isNull())
		{
			qint64 diff = now.toMSecsSinceEpoch() -
				dev->getLastSeen().toMSecsSinceEpoch();

			updateText(dev, dev->getItem("root"), "last_seen",
				tr("Last Seen: %1 sec").arg((double)diff / 1000.0));

			if(diff > 5000)
				dev->disableAllItems();
		}
	}
}

void DiscoveredDeviceModel::updateItem(DiscoveredDevice *dev)
{
	static const QString advTypes[] = {
		"Connectable",
		"Directed",
		"Scannable",
		"Non-connectable"
	};

	QStandardItem *item = dev->getItem("root");
	item->setData(QVariant::fromValue<DiscoveredDevice*>(dev));
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
}

void DiscoveredDeviceModel::updateAdvertData(DiscoveredDevice *dev)
{
	QStandardItem *advData = dev->getItem("adv_data");

	const uint8_t *data = (uint8_t*)dev->getAdvertData().constData();
	int len = dev->getAdvertData().size();

	int pos = 0;

	while(pos < (len - 2))
	{
		int item_len = data[pos++];
		if(item_len == 0)
			break;

		int type = data[pos];
		if((pos + item_len) > len)
			break;

		pos++;
		QString ps = QString::number(pos);

		switch(type)
		{
			case 0x01: // Flags
			{
				QStringList flags;
				if(data[pos] & 0x01)
					flags.append(tr("LimitedDiscoverable"));
				if(data[pos] & 0x02)
					flags.append(tr("GeneralDiscoverable"));
				if(data[pos] & 0x04)
					flags.append(tr("BrEdrNotSupported"));
				if(data[pos] & 0x08)
					flags.append(tr("DualControllerCapable"));
				if(data[pos] & 0x10)
					flags.append(tr("DualHostCapable"));

				updateText(dev, advData, "adv_flags"+ps, tr("Flags: %1").arg(
					flags.join(", ")));
				break;
			}
			case 0xFF: // Manufacturer Specific Data
			{
				if(data[pos] == 0x4C && data[pos + 1] == 0x00 &&
					data[pos + 2] == 0x02 && data[pos + 3] == 0x15)
				{
					updateText(dev, advData, "ibeacon" + ps,
						tr("iBeacon Data"));
					QStandardItem *iItem = dev->getItem("ibeacon" + ps);
					updateText(dev, iItem, "ibeacon_uuid" + ps,
						tr("Proximity UUID: %1").arg(QUuid::fromRfc4122(
						QByteArray((char*)&data[pos + 4], 16)).toString(
						).replace("{", "").replace("}", "")));
					uint16_t major = data[pos + 21] | (data[pos + 20] << 8);
					uint16_t minor = data[pos + 23] | (data[pos + 22] << 8);
					updateText(dev, iItem, "ibeacon_major" + ps,
						tr("Major: %1").arg(major));
					updateText(dev, iItem, "ibeacon_minor" + ps,
						tr("Minor: %1").arg(minor));
					int8_t txPow;
					memcpy(&txPow, &data[pos + 24], 1);
					updateText(dev, iItem, "ibeacon_txpow" + ps,
						tr("TX Power: %1dBm").arg(txPow));
				}
				else
				{
					QStringList mdata;

					for(uint8_t i = 0; i < (item_len - 1); i++)
					{
						mdata.append(tr("%1").arg(data[pos + i], 2, 16,
							QLatin1Char('0')).toUpper());
					}

					updateText(dev, advData, "adv_manufacturer" + ps,
						tr("Manufacturer Specific Data: %1").arg(
							mdata.join("-")));
				}
				break;
			}
			case 0x03: // 16-bit Service UUIDs
			{
				uint8_t count = (item_len - 1) / 2;
				QStringList uuids;

				for(uint8_t i = 0; i < count; i++)
				{
					uint16_t uuid = data[pos + i * 2] |
						(data[pos + 1 + i * 2] << 8);
					QString str = tr("%1").arg(uuid, 4, 16, QLatin1Char('0')).toUpper();
					str = "0x" + str;
					uuids.append(str);
				}

				updateText(dev, advData, "adv_serv16" + ps,
						tr("ServicesCompleteListUuid16: %1").arg(
							uuids.join(", ")));
				break;
			}
			case 0x16:
			{
				uint16_t uuid = data[pos + 1] | (data[pos] << 8);
				QString str = tr("%1").arg(uuid, 4, 16, QLatin1Char('0')).toUpper();
				str = "0x" + str;

				QStringList mdata;

				for(uint8_t i = 2; i < (item_len - 1); i++)
				{
					mdata.append(tr("%1").arg(data[pos + i], 2, 16,
						QLatin1Char('0')).toUpper());
				}

				if(mdata.isEmpty())
				{
					updateText(dev, advData, "adv_serv_data16" + ps,
						tr("ServiceDataUuid16: %1").arg(
							str));
				}
				else
				{
					updateText(dev, advData, "adv_serv_data16" + ps,
						tr("ServiceDataUuid16: %1 (%2)").arg(
							str).arg(mdata.join("-")));
				}

				break;
			}
			case 0x09:
			{
				updateText(dev, advData, "adv_local_name" + ps,
						tr("CompleteLocalName: %1").arg(QString::fromAscii(
							(char*)&data[pos], item_len - 1)));
				break;
			}
			case 0x19:
			{
				uint16_t appearance = data[pos + 1] | (data[pos] << 8);
				if(appearance == 833)
				{
					updateText(dev, advData, "adv_appearance" + ps,
						tr("Appearance: 0x%1").arg(appearance, 4, 16, QLatin1Char('0')));
				}
				else
				{
					updateText(dev, advData, "adv_appearance" + ps,
						tr("Appearance: %1").arg("HeartRateSensor"));
				}

				break;
			}
			default:
			{
				updateText(dev, advData, "unk_eir" + ps,
						tr("Unknown EIR Field: 0x%1").arg(
							QString::number(type, 16).toUpper()));
				break;
			}
		}

		pos += item_len - 1;
	}
}

void DiscoveredDeviceModel::updateText(DiscoveredDevice *dev,
	QStandardItem *root, const QString& key,
	const QString& text)
{
	QStandardItem *item = dev->getItem(key);
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
}
