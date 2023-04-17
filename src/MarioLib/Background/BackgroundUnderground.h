#pragma once

#include "Common.h"
#include "BackgroundBase.h"

class Stage;
class BackgroundUnderground : public BackgroundBase
{
public:
	BackgroundUnderground(Stage *pStage);
	virtual ~BackgroundUnderground();

	virtual void BuildBackground();
};
