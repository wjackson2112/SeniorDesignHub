#ifndef __SensorHubLinux_h__
#define __SensorHubLinux_h__

#include "SensorHub.h"

#include <QtCore/QMap>

#include <gato/gatouuid.h>
#include <gato/gatocharacteristic.h>

class GatoService;
class GatoPeripheral;
class GatoCharacteristic;

class SensorHubLinux : public SensorHub
{
	Q_OBJECT

public:
	SensorHubLinux(GatoPeripheral *peripheral, int rssi,
		QObject *parent = 0);
	virtual ~SensorHubLinux();

	virtual void Connect();
	virtual void Disconnect();
	virtual bool IsConnected() const;

	virtual void Write(uint16_t service, uint16_t characteristic,
		const QByteArray& data);
	virtual void Read(uint16_t service, uint16_t characteristic);
	virtual void SetNotify(uint16_t service, uint16_t characteristic);

private slots:
	void Connected();
	void ServicesDiscovered();
	void CharacteristicsDiscovered(const GatoService& service);
	void DescriptorsDiscovered(const GatoCharacteristic& characteristic);
	void ReadGato(const GatoCharacteristic& characteristic,
		const QByteArray& data);

private:
	GatoCharacteristic CharacteristicByUUID(
		uint16_t service, uint16_t characteristic) const;

	GatoPeripheral *mPeripheral;

	/// List of services and if the characteristics for that service have been
	/// discovered yet.
	QMap<GatoUUID, bool> mCharacteristicsDiscovered;
};

#endif // __SensorHubLinux_h__
