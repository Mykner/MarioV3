#pragma once

#include "Common.h"

#include "EventObjectBase.h"
#include "PropertyMap.h"

class BowserFireShooter : public EventObjectBase
{
public:
	BowserFireShooter(Game *pGame, Stage *pStage);
	virtual ~BowserFireShooter();

	virtual void RequestSprites(Stage *pStage = nullptr);

	virtual void OnBeforeProcess();
	virtual void OnAfterProcess();

	virtual void RenderDesign(int nColor = 0xffffffff, int nZOrder = -1);

	virtual void OnIdle();

	virtual SIZE GetSize();
};