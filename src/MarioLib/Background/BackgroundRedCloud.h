#pragma once

#include "Common.h"
#include "BackgroundBase.h"

class Stage;
class BackgroundRedCloud : public BackgroundBase
{
public:
	BackgroundRedCloud(Stage *pStage);
	virtual ~BackgroundRedCloud();

	virtual void BuildBackground();
};
