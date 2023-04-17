#pragma once

#include "Common.h"
#include "BackgroundBase.h"

class Stage;
class BackgroundGhostHouse : public BackgroundBase
{
public:
	BackgroundGhostHouse(Stage *pStage);
	virtual ~BackgroundGhostHouse();

	virtual void BuildBackground();
};
