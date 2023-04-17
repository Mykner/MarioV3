#pragma once

#include "Common.h"

#include "EventObjectBase.h"

class WarpZoneFlag : public EventObjectBase
{
public:
	WarpZoneFlag(Game *pGame, Stage *pStage);
	virtual ~WarpZoneFlag();

	virtual void CheckCollisionPlayer();

	virtual void Process();
	virtual void OnBeforeProcess();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual void RenderDesign(int nColor = 0xffffffff, int nZOrder = -1);

	virtual SIZE GetSize();
};

