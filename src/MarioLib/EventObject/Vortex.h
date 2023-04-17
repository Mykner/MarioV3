#pragma once

#include "Common.h"

#include "EventObjectBase.h"
#include "PropertyMap.h"

class Vortex : public EventObjectBase
{
public:
	Vortex(Game *pGame, Stage *pStage);
	virtual ~Vortex();

	virtual void OnBeforeProcess();
	virtual void OnAfterProcess();

	virtual void Process();
	virtual void RenderDesign(int nColor = 0xffffffff, int nZOrder = -1);

	virtual SIZE GetSize();

	int m_nWidth;
};