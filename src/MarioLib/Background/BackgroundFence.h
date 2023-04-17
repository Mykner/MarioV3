#pragma once

#include "Common.h"
#include "BackgroundBase.h"

class Stage;
class BackgroundFence : public BackgroundBase
{
public:
	BackgroundFence(Stage *pStage);
	virtual ~BackgroundFence();

	virtual void BuildBackground();
};
