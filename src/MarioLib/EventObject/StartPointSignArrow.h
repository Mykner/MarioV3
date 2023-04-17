#pragma once

#include "Common.h"

#include "EventObjectBase.h"
#include "PropertyMap.h"

class StartPointSignArrow : public EventObjectBase
{
public:
	StartPointSignArrow(Game *pGame, Stage *pStage);
	virtual ~StartPointSignArrow();

	virtual void Process();
	virtual void OnBeforeProcess();
	virtual void OnAfterProcess();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	
	virtual SIZE GetSize();
};