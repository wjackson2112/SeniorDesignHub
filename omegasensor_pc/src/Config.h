#ifndef _Config_h__
#define _Config_h__

#include "SensorHub.h"
#include "SensorHubConfig.h"

#include "ui_Config.h"

class Config : public QDialog
{
	Q_OBJECT

public:
	Config(const SensorHubPtr& hub, QWidget *parent = 0);

public slots:
	void SaveConfig();

private:
	Ui::Config ui;

	SensorHubConfigPtr mConfig;
};

#endif // _Config_h__
