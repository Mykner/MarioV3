#pragma once

#include "Common.h"
#include "BackgroundBase.h"

class Stage;
class BackgroundAirShip : public BackgroundBase
{
public:
	BackgroundAirShip(Stage *pStage);
	virtual ~BackgroundAirShip();

	virtual void BuildBackground();
};
