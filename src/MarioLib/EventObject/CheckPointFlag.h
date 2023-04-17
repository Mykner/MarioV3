#pragma once

#include "Common.h"

#include "EventObjectBase.h"
#include "PropertyMap.h"

class CheckPointFlag : public EventObjectBase
{
public:
	CheckPointFlag(Game *pGame, Stage *pStage);
	virtual ~CheckPointFlag();

	virtual void Process();
	virtual void OnBeforeProcess();
	virtual void OnAfterProcess();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	
	virtual SIZE GetSize();
	virtual int GetSpriteIndex();

	bool m_bChecked;
};