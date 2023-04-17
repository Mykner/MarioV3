#pragma once

#include "Common.h"
#include "GameObjectBase.h"

#include "PhysicalProperties.h"

class Game;
class ControllableObject : public GameObjectBase
{
public:
	ControllableObject(Game *pGame, Stage *pStage);
	virtual ~ControllableObject();

	virtual void CheckCollision();
	virtual void CheckCollisionGiant();

	virtual bool CheckJump();

	virtual void Process();
	virtual void ProcessBase();
	virtual void ProcessShapeChange(int nStateFrame);
    virtual void ProcessCarry();

	virtual void OnBeforeProcess();
	virtual void OnAfterProcess();

	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual void RenderShapeChange();
	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void OnChangeState(int nState);

	virtual bool CanSit();

	virtual void OnStand();
	virtual void OnSit();
	virtual void OnWalk();
	virtual void OnShellRun();
	virtual void OnShellIdle();
	virtual void OnReachWall(bool bIsLeftWall);
	virtual void OnBreak();
	virtual void OnJump();
	virtual void OnTrampolinJump();
	virtual void OnFall();
	virtual void OnSwim();
	virtual void OnSwimJump();
	virtual void OnSpinJump();
	virtual void OnRide();
	virtual void OnPipeUp();
	virtual void OnPipeDown();
	virtual void OnPipeLeft();
	virtual void OnPipeRight();
	virtual void OnDie();
	virtual void OnBlockJump();
	virtual void OnParalyze();
	virtual void OnSpecialPose();
	virtual void OnVictoryPose();
	virtual void OnLanding();

	virtual void OnApplyItem(GameObjectBase *pItemObject);
	virtual void OnApplyItem(int nType);
	virtual void OnApplyEvent(GameObjectBase *pEventObject);

	// For Mario
	virtual void ChangeShape(int nShape);
	virtual void OnChangeShape(int nShape);

    virtual void CheckOutOfStage();
    virtual void UnderwaterEffect();

	virtual bool CanFire();
	virtual void ThrowFireball();

	virtual void OnAir();
	virtual void OnPressJump();
	virtual void OnFlagDown();
	virtual void OnFlagFlip();
	virtual void OnClimbIdle();
	virtual void OnClimbMove();
	virtual bool OnClimbCheckFlip();
	virtual void OnClimbCheckAway();
	virtual void OnInvinsible(bool bEffect);
	virtual void OnDamaged();

	virtual void Carry(GameObjectBase* pObj);
	virtual void MoveCarryObject();
	virtual void Kick(GameObjectBase* pObj, bool bIsCarried = false);

	virtual bool IsAlive();
	virtual bool HasMushroom();
	virtual bool CanBurnt(int nType = BURN_UNKNOWN);
	virtual bool CanPressEnemy();
	virtual bool CanCarry();
	virtual bool IsSitSpriteIndex();

	virtual int GetPower(int nPowerForWhat);

	virtual void EnterPipe(PipeInfo *pPipeInfo);
	virtual void EnterDoor(GameObjectBase *pObj);

	virtual int GetSpriteIndex();
	virtual int GetZOrder();
	virtual void GetScale(float &fScaleX, float &fScaleY);
	virtual SIZE GetSize();

	virtual POINT GetHandPosition();

	virtual void GetColor(int* pnColor);

	virtual Sprite* GetCostumeSprite(int nCostume = -1);

	int m_nSpriteIndex;
	int m_nWalkFrame;
	int m_nJumpHold;
	int m_nThrowFireFrame;
	int m_nKickFrame;
	int m_nCostume;

	PhysicalProperties m_PhysicalProps;

	GameObjectBase* m_pClimb;
};

