#pragma once

#include "Common.h"

#include "ItemObjectBase.h"

class Vine : public ItemObjectBase
{
public:
	Vine(Game *pGame, Stage *pStage);
	virtual ~Vine();

	virtual void Process();
	virtual void ProcessBase();
	virtual void OnBeforeProcess();
	
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);

	void OnVineGrow();

	bool CanApplied();
	virtual bool CanEat();

	virtual SIZE GetSize();

	int m_nLength;
	int m_nMaxLength;
};

