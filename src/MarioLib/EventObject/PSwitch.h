#pragma once

#include "Common.h"

#include "EventObjectBase.h"
#include "PropertyMap.h"

class PSwitch : public EventObjectBase
{
public:
	PSwitch(Game *pGame, Stage *pStage);
	virtual ~PSwitch();

	virtual void Process();
	virtual void OnBeforeProcess();
	virtual void OnAfterProcess();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void OnIdle();
	virtual void OnReachWall(bool bIsLeftWall);
	virtual void OnPressed();
	virtual void OnLanding();

	virtual bool IsAlive();

	virtual bool CanBornFromBlock();
	virtual bool CanCarried();

	virtual void AddPassenger(GameObjectBase *pNewObj);

	virtual int GetSpriteIndex();
	virtual int GetZOrder();
	virtual SIZE GetSize();

	int m_nPressedTimer;
};