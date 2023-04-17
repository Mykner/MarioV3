#pragma once

#include "Common.h"

#include "EventObjectBase.h"
#include "PropertyMap.h"

class Trampolin : public EventObjectBase
{
public:
	Trampolin(Game *pGame, Stage *pStage);
	virtual ~Trampolin();

	virtual void OnBeforeProcess();
	virtual void OnAfterProcess();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual void RenderDesign(int nColor = 0xffffffff, int nZOrder = -1);
	void RenderTrampolin(float x, float y, int nColor = 0xffffffff);

	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void OnIdle();
	virtual void OnPressed();
	virtual void OnSpring();

	virtual bool IsAlive();

	virtual void AddPassenger(GameObjectBase *pNewObj);
	void MovePassengers();
	void PushUpPassengers();

	virtual int GetSpriteIndex();
	virtual SIZE GetSize();

	std::vector<GameObjectBase*> m_vecPassenger;
};