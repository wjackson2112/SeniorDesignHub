#include "Driver.h"
#include "SensorHubService.h"

Driver::~Driver()
{
	if(!mHub.isNull()) {
		mHub->Disconnect();
		//SensorHubService::GetSingletonPtr()->Reload();
	}
}
