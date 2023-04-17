#pragma once

#include "Common.h"

#include "ItemObjectBase.h"

class CloudItem : public ItemObjectBase
{
public:
	CloudItem(Game *pGame, Stage *pStage);
	virtual ~CloudItem();

	virtual void RequestSprites(Stage *pStage = nullptr);

	virtual void OnItemPop();
	virtual void OnItemPopDown();
	virtual void OnFly();

	virtual bool CanApplied();

	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual int GetSpriteIndex();
	virtual void GetScale(float &fScaleX, float &fScaleY);

	Sprite** m_ppSpriteVehicle;
};