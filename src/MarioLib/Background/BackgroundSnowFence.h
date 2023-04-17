#pragma once

#include "Common.h"
#include "BackgroundBase.h"

class Stage;
class BackgroundSnowFence : public BackgroundBase
{
public:
	BackgroundSnowFence(Stage *pStage);
	virtual ~BackgroundSnowFence();

	virtual void BuildBackground();
};
