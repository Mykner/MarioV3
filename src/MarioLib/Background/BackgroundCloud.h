#pragma once

#include "Common.h"
#include "BackgroundBase.h"

class Stage;
class BackgroundCloud : public BackgroundBase
{
public:
	BackgroundCloud(Stage *pStage);
	virtual ~BackgroundCloud();

	virtual void BuildBackground();
};
