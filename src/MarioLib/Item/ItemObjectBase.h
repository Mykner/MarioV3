#pragma once

#include "Common.h"

#include "GameObjectBase.h"

class Sprite;
class ItemObjectBase : public GameObjectBase
{
public:
	ItemObjectBase(Game *pGame, Stage *pStage);
	virtual ~ItemObjectBase();

	virtual void CheckCollisionPlayer();
	virtual void Process();
	virtual void OnBeforeProcess();

	virtual void ChangeState(int nState, ChangeStateInfo * pInfo = nullptr);
	virtual void OnChangeState(int nState);

	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	virtual void OnJump();
	virtual void OnItemPop();
	virtual void OnItemPopDown();
	virtual void OnBlockJump();
	virtual void OnLanding();

	virtual bool CanApplied();
	virtual bool CanEat();
	virtual bool CanBornFromBlock();

	virtual void OnReachWall(bool bIsLeftWall);

	virtual int GetSpriteIndex();
	virtual NaRect GetRect();
	virtual SIZE GetSize();
	virtual int GetZOrder();

	static GameObjectBase* CreateInstance(Game *pGame, Stage *pStage, int nType);
};

