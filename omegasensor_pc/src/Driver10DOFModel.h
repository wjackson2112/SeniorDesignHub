#ifndef __Driver10DOFModel_h__
#define __Driver10DOFModel_h__

#include <QtCore/QFile>
#include <QtCore/QTimer>
#include <QtCore/QObject>
#include <QtCore/QDateTime>

class Driver10DOF;

const size_t MAX_HISTORY = 30;

class Driver10DOFModel : public QObject
{
	Q_OBJECT

public:
	Driver10DOFModel(Driver10DOF *drv, QObject *parent = 0);

	float AccelX(int idx = 0) const;
	float AccelY(int idx = 0) const;
	float AccelZ(int idx = 0) const;

	int64_t AccelTime(int idx = 0) const;

	int NumAccelValues() const;

	float MagX(int idx = 0) const;
	float MagY(int idx = 0) const;
	float MagZ(int idx = 0) const;

	int NumMagValues() const;

	float GyroX(int idx = 0) const;
	float GyroY(int idx = 0) const;
	float GyroZ(int idx = 0) const;

	int NumGyroValues() const;

	float Pressure(int idx = 0) const;
	int NumPressureValues() const;

	float Temp(int idx = 0) const;
	int NumTempValues() const;

	void CleanClose();

signals:
	void NewAccel(float x, float y, float z);
	void NewMag(float x, float y, float z);
	void NewGyro(float x, float y, float z);
	void NewPressure(float press);
	void NewTemp(float temp);

private slots:
	void RecordAccel(float x, float y, float z);
	void RecordMag(float x, float y, float z);
	void RecordGyro(float x, float y, float z);
	void RecordPressure(float press);
	void RecordTemp(float temp);

private:
	Driver10DOF *mDriver;

	QFile mAccelLog, mMagLog, mGyroLog, mPressureLog, mTempLog;
	QTimer mTimer, mTimer2;

	int mAccelCount;
	float mAccelX[MAX_HISTORY];
	float mAccelY[MAX_HISTORY];
	float mAccelZ[MAX_HISTORY];
	QDateTime mAccelTime[MAX_HISTORY];

	int mMagCount;
	float mMagX[MAX_HISTORY];
	float mMagY[MAX_HISTORY];
	float mMagZ[MAX_HISTORY];
	QDateTime mMagTime[MAX_HISTORY];

	int mGyroCount;
	float mGyroX[MAX_HISTORY];
	float mGyroY[MAX_HISTORY];
	float mGyroZ[MAX_HISTORY];
	QDateTime mGyroTime[MAX_HISTORY];

	int mPressureCount;
	float mPressure[MAX_HISTORY];
	QDateTime mPressureTime[MAX_HISTORY];

	int mTempCount;
	float mTemp[MAX_HISTORY];
	QDateTime mTempTime[MAX_HISTORY];
};

#endif // __Driver10DOFModel_h__
