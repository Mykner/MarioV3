#pragma once

/*
#include "Common.h"

#include "RedTurtle.h"

class RedTurtleWingedPatrol : public RedTurtle
{
public:
	RedTurtleWingedPatrol(Game *pGame, Stage *pStage);
	virtual ~RedTurtleWingedPatrol();
	
	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);

	virtual void RenderDesign(int nColor = 0xffffffff, int nZOrder = -1);

	virtual void OnIdle();
	virtual void OnMove();
	virtual void OnWalk();

	virtual int GetSpriteIndex();

	int m_nDistance;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("MoveDistance", &m_nDistance)
		DECL_PROP("PowerUp", &m_bPowerUp)
	END_DECLARE_PROPERTYMAP()
};
*.