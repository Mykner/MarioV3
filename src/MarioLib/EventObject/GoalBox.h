#pragma once

#include "Common.h"

#include "EventObjectBase.h"

class GoalBox : public EventObjectBase
{
public:
	GoalBox(Game *pGame, Stage *pStage);
	virtual ~GoalBox();
	virtual void RequestSprites(Stage *pStage = nullptr);

	virtual void Process();
	virtual void OnBeforeProcess();
	virtual void OnAfterProcess();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual void OnChangeState(int nState);

	virtual void OnIdle();
	virtual void OnFly();
	virtual bool CanApplied();

	virtual SIZE GetSize();

	float m_fItemX;
	float m_fItemY;
	float m_fItemYS;
	int m_nItemIdx;
	Sprite** m_ppSpriteItem;
};