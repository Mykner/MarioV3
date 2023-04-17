#pragma once

#include "Common.h"

#include "EventObjectBase.h"
#include "PropertyMap.h"

#include "LoopHandler.h"

class LoopFlag : public EventObjectBase
{
public:
	LoopFlag(Game *pGame, Stage *pStage);
	virtual ~LoopFlag();

	virtual void OnBeforeProcess();
	virtual void OnAfterProcess();

	virtual void Process();
	virtual void RenderDesign(int nColor = 0xffffffff, int nZOrder = -1);

	virtual void OnIdle();

	virtual SIZE GetSize();

	int m_nSleep;
	LoopHandler *m_pHandler;
};