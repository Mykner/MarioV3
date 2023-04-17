#pragma once

#include "Common.h"

#include "EventObjectBase.h"

class GoalPost : public EventObjectBase
{
public:
	GoalPost(Game *pGame, Stage *pStage);
	virtual ~GoalPost();

	virtual void CheckCollisionPlayer();

	virtual void Process();
	virtual void OnBeforeProcess();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual void OnChangeState(int nState);

	virtual void OnIdle();
	void OnFlagDown();
	virtual bool CanApplied();

	virtual SIZE GetSize();

	float m_fLineHeight;
	float m_fLineSpeed;
};

