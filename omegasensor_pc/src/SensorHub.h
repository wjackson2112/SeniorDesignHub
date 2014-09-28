#ifndef __SensorHub_h__
#define __SensorHub_h__

#include <QtCore/QMap>
#include <QtCore/QObject>

#include <gato/gatouuid.h>
#include <gato/gatocharacteristic.h>

#include <stdint.h>

class GatoService;
class GatoPeripheral;
class GatoCharacteristic;

class SensorHub : public QObject
{
	Q_OBJECT

public:
	SensorHub(GatoPeripheral *peripheral, int rssi,
		QObject *parent = 0);
	~SensorHub();

	int GetRSSI() const;
	void SetRSSI(int rssi);

	QString HardwareName() const;

	void Connect();
	void Disconnect();

	void Write(const GatoCharacteristic& characteristic,
		const QByteArray& data);
	void SetNotify(const GatoCharacteristic& characteristic);

	GatoCharacteristic CharacteristicByUUID(
		uint16_t service, uint16_t characteristic) const;

signals:
	void Read(const GatoCharacteristic& characteristic,
		const QByteArray& data);

private slots:
	void Connected();
	void ServicesDiscovered();
	void CharacteristicsDiscovered(const GatoService& service);
	void DescriptorsDiscovered(const GatoCharacteristic& characteristic);

private:
	int mRSSI;
	GatoPeripheral *mPeripheral;
	GatoCharacteristic *mRx, *mTx, *mConfig;

	/// List of services and if the characteristics for that service have been
	// discovered yet.
	QMap<GatoUUID, bool> mCharacteristicsDiscovered;
};

#endif // __SensorHub_h__
