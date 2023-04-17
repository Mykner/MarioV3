#pragma once

#include "Common.h"

#include "ItemObjectBase.h"

class DoorKey : public ItemObjectBase
{
public:
	DoorKey(Game *pGame, Stage *pStage);
	virtual ~DoorKey();

	virtual void Process();

	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void OnChangeState(int nState);

	virtual void OnItemPop();
	virtual void OnFly();
	virtual void OnFollowPrepare();
	virtual void OnFollow();

	void MakeTwinkle();

	virtual bool IsOutOfCamera();
	virtual bool CanApplied();
	virtual bool CanEat();

	virtual int GetSpriteIndex();
	virtual int GetZOrder();

	GameObjectBase *m_pFollow;
	std::vector<POINT> m_vecFollow;

	NaPointT<float> m_ptCenter;
	float m_fTargetDistance;
	float m_fCurrentDistance;
	float m_fWalkingBounceOffset;
	float m_fWalkingBounceSpeed;
};

