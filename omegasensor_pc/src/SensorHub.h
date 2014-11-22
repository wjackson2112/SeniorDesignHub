#ifndef __SensorHub_h__
#define __SensorHub_h__

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QDateTime>
#include <QtCore/QMetaType>
#include <QtCore/QSharedPointer>

#include <stdint.h>

class SensorHub : public QObject
{
	Q_OBJECT

public:
	SensorHub(int rssi, QObject *parent = 0);
	virtual ~SensorHub();

	int GetRSSI() const;
	void SetRSSI(int rssi);

	QString GetAddress() const;
	QString HardwareName() const;

	virtual void Connect() = 0;
	virtual void Disconnect() = 0;
	virtual bool IsConnected() const = 0;

	virtual void Write(uint16_t service, uint16_t characteristic,
		const QByteArray& data) = 0;
	virtual void SetNotify(uint16_t service, uint16_t characteristic) = 0;

	QDateTime LastSeen() const;

signals:
	void Read(uint16_t characteristic, const QByteArray& data);

protected:
	void SetAddress(const QString& addr);
	void SetHardwareName(const QString& name);

protected slots:
	void UpdateLastSeen();
	void LoadDrivers();

private:
	int mRSSI;
	QDateTime mLastSeen;

	QString mAddress;
	QString mHardwareName;
};

typedef QSharedPointer<SensorHub> SensorHubPtr;

Q_DECLARE_METATYPE(SensorHubPtr);

#endif // __SensorHub_h__
