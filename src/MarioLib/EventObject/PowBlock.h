#pragma once

#include "Common.h"

#include "EventObjectBase.h"
#include "PropertyMap.h"

class PowBlock : public EventObjectBase
{
public:
	PowBlock(Game *pGame, Stage *pStage);
	virtual ~PowBlock();

	virtual void Process();
	virtual void OnBeforeProcess();
	virtual void OnAfterProcess();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void OnIdle();
	virtual void OnReachWall(bool bIsLeftWall);
	virtual void OnPressed();
	virtual void OnPushedUp();
	virtual void OnKicked();
	virtual void OnLanding();
	virtual void OnExplode();

	virtual bool IsAlive();

	virtual bool CanBornFromBlock();
	virtual bool CanCarried();
	virtual bool CanBurnt(int nType);

	virtual void AddPassenger(GameObjectBase *pNewObj);

	virtual int GetSpriteIndex();
	virtual int GetZOrder();
	virtual void GetScale(float &fScaleX, float &fScaleY);
	virtual SIZE GetSize();

	bool m_bKicked;
};