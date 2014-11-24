#ifndef __Driver10DOFView_h__
#define __Driver10DOFView_h__

#include "ui_Driver10DOF.h"

class Driver10DOFModel;

class Driver10DOFView : public QWidget
{
	Q_OBJECT

public:
	Driver10DOFView(QWidget *parent = 0);

	void SetModel(Driver10DOFModel *model);
	void SetDeviceName(const QString& name);

protected slots:
	void Update();

private:
	QString mDeviceName;
	Driver10DOFModel *mModel;

	Ui::Driver10DOF ui;
};

#endif // __Driver10DOFView_h__
