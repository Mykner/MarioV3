#pragma once

#include "Common.h"

#include "EventObjectBase.h"
#include "PropertyMap.h"

class ShoeSmoke : public EventObjectBase
{
public:
	ShoeSmoke(Game *pGame, Stage *pStage);
	virtual ~ShoeSmoke();

	virtual void RequestSprites(Stage *pStage = nullptr);

	virtual void ParseParameter(CreateParameter * pParam);

	virtual void Process();
	virtual void OnBeforeProcess();
	virtual void OnAfterProcess();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void OnIdle();
	
	virtual bool IsAlive();

	virtual bool CanBornFromBlock();
	virtual bool CanCarried();

	virtual void AddPassenger(GameObjectBase *pNewObj);

	virtual int GetSpriteIndex();
	virtual int GetZOrder();
	virtual void GetScale(float &fScaleX, float &fScaleY);
	virtual SIZE GetSize();
};