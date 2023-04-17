#pragma once

#include "Common.h"
#include "BackgroundBase.h"

class Stage;
class BackgroundUnderWater : public BackgroundBase
{
public:
	BackgroundUnderWater(Stage *pStage);
	virtual ~BackgroundUnderWater();

	virtual void BuildBackground();
};
