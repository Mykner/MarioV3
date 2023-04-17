#pragma once

#include "Common.h"

#include "GameObjectBase.h"

class Sprite;
class EnemyObjectBase : public GameObjectBase
{
public:
	EnemyObjectBase(Game *pGame, Stage *pStage);
	virtual ~EnemyObjectBase();

	virtual void Process();

	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void OnChangeState(int nState);

	virtual void OnBeforeProcess();
	virtual void OnAfterProcess();

	virtual void OnIdle();
	virtual void OnStand();
	virtual void OnWalk();
	virtual void OnFall();
	virtual void OnReachWall(bool bIsLeftWall);
	virtual void OnReachFriend();
	virtual void OnItemPop();
	virtual void OnPushedUp();
	virtual void OnPressed();
	virtual void OnBurnt();
	virtual void OnStacked();
	virtual void OnBlockJump();
	virtual void OnCarryByHand();
	virtual void OnRide();

	virtual void OnPowerUp();

	virtual void GiveScoreAndComboToAttacker(GameObjectBase *pAttacker, int nScoreFrom, bool bAddCombo = true);

	virtual void ReleaseEnemyStack();
	virtual void ReleaseHeadStack();
	virtual void ReleaseFootStack();
	virtual void ReleaseVehicle();

	virtual bool IsAlive();
	virtual bool CanKicked();
	virtual bool CanEat();
	virtual bool CanSpit();
	virtual bool CanDamageFriend();
	virtual bool CanDamageEnemy();
	virtual bool CanBornFromBlock();

	virtual int GetSpriteIndex();
	virtual void GetScale(float &fScaleX, float &fScaleY);
	virtual int GetZOrder();
	virtual SIZE GetSize();
	virtual SIZE GetSpriteSize();

	virtual int GetMaskedType();

    virtual float GetStackXOffset();
    virtual int GetStackUnderMeCount();
    virtual EnemyObjectBase* GetStackEnemyBeforeLast();

	static GameObjectBase* CreateInstance(Game *pGame, Stage *pStage, int nType);

	EnemyObjectBase *m_pOnMyHead;
	EnemyObjectBase *m_pUnderMyFoot;
};

