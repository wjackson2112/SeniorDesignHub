#ifndef __Driver10DOFModel_h__
#define __Driver10DOFModel_h__

#include <QtCore/QFile>
#include <QtCore/QTimer>
#include <QtCore/QObject>

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

	int NumAccelValues() const;

signals:
	void NewAccel(float x, float y, float z);

private slots:
	void RecordAccel(float x, float y, float z);

private:
	Driver10DOF *mDriver;

	QFile mLog;
	QTimer mTimer;

	int mAccelCount;
	float mAccelX[MAX_HISTORY];
	float mAccelY[MAX_HISTORY];
	float mAccelZ[MAX_HISTORY];
};

#endif // __Driver10DOFModel_h__
