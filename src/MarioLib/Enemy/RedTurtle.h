#pragma once

#include "Common.h"

#include "Turtle.h"

class RedTurtle : public Turtle
{
public:
	RedTurtle(Game *pGame, Stage *pStage);
	virtual ~RedTurtle();

	virtual void OnIdle();
	virtual void OnWalk();
	virtual void OnMove();
	virtual void OnAfterItemPop(int nNextStatePopUp /* = STATE_WALK */, int nNextStatePopDown /* = STATE_FALL */);

	virtual void RenderDesign(int nColor = 0xffffffff, int nZOrder = -1);

	virtual int GetSpriteIndex();

	int m_nDistance;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("MoveDistance", &m_nDistance)
        DECL_PROP("IsEmptyShell", &m_bIsEmptyShell)
		DECL_PROP("PowerUp", &m_bPowerUp)
		DECL_PROP("Winged", &m_bWinged)
	END_DECLARE_PROPERTYMAP()
};

