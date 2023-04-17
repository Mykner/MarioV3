#pragma once

#include "Common.h"
#include "BackgroundBase.h"

class Stage;
class BackgroundMountain : public BackgroundBase
{
public:
	BackgroundMountain(Stage *pStage);
	virtual ~BackgroundMountain();

	virtual void BuildBackground();
};
