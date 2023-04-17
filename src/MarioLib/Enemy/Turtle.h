#pragma once

#include "Common.h"

#include "EnemyObjectBase.h"

enum TurtleShapes
{
	TURTLESHAPE_SHELLKICKED = 0,
	TURTLESHAPE_SHELL,
	TURTLESHAPE_SHELLAWAKEN,
	TURTLESHAPE_NORMAL,
};

class Turtle : public EnemyObjectBase
{
public:
	Turtle(Game *pGame, Stage *pStage);
	virtual ~Turtle();

	virtual void Process();
	virtual void OnBeforeProcess();
	virtual void OnAfterProcess();

	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	virtual void CheckCollisionPowerUp();

	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void OnChangeState(int nState);

	virtual void OnWalk();
	virtual void OnJump();
	virtual void OnReachWall(bool bIsLeftWall);
	virtual void OnReachFriend();
	virtual void OnPushedUp();
	virtual void OnPressed();
	virtual void OnShellIdle();
	virtual void OnShellRun();
	virtual void OnShellAwake();
	virtual void OnShellKicked();
	virtual void OnBlockJump();
	virtual void OnLanding();

	virtual bool CanDamageEnemy();
	virtual bool CanSpit();
	virtual bool CanCarried();
	virtual bool CanKicked();

	virtual std::vector<POINT> GetWingPosition();
	virtual int GetSpriteIndex();
	virtual SIZE GetSize();

	int m_nSpriteIndex;
	int m_nAwakeTimer;

	bool m_bDeadInside;

	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("PowerUp", &m_bPowerUp)
		DECL_PROP("Winged", &m_bWinged)
	END_DECLARE_PROPERTYMAP()
};

