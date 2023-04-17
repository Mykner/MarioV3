#pragma once

#include "Common.h"

#include "EventObjectBase.h"
#include "PropertyMap.h"

class TrampolinMini : public EventObjectBase
{
public:
	TrampolinMini(Game *pGame, Stage *pStage);
	virtual ~TrampolinMini();

	void CheckCollisionTrampolinH();

	virtual void Process();
	virtual void OnBeforeProcess();
	virtual void OnAfterProcess();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual void RenderDesign(int nColor = 0xffffffff, int nZOrder = -1);
	void RenderTrampolinMini(float x, float y, int nColor = 0xffffffff);

	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void OnIdle();
	virtual void OnReachWall(bool bIsLeftWall);
	virtual void OnLanding();

	void CheckSpring();
	void ProcessSpring();

	virtual bool IsAlive();

	virtual bool CanBornFromBlock();
	virtual bool CanCarried();

	virtual void AddPassenger(GameObjectBase *pNewObj);
	void MovePassengers();
	void PushUpPassengers();

	virtual int GetSpriteIndex();
	virtual SIZE GetSize();

	std::vector<GameObjectBase*> m_vecPassenger;
	std::vector<GameObjectBase*> m_vecWillPush;

	bool m_bIsHorizontal;
	int m_nSpringFrame;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		{ L"IsHorizontal", &m_bIsHorizontal },
	END_DECLARE_PROPERTYMAP()
};