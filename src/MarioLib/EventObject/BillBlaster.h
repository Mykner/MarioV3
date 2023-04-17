#pragma once

#include "Common.h"

#include "EventObjectBase.h"
#include "PropertyMap.h"

class BillBlaster : public EventObjectBase
{
public:
	BillBlaster(Game *pGame, Stage *pStage);
	virtual ~BillBlaster();
	
	virtual void RequestSprites(Stage *pStage = nullptr);

	virtual void OnBeforeProcess();
	virtual void OnAfterProcess();

	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	virtual void OnStand();

	virtual bool CanPushedUp();

	virtual void GetScale(float &fScaleX, float &fScaleY);
	virtual NaRect GetRect();
	virtual NaRectT<float> GetRectFloat();
	virtual SIZE GetSize();

	int m_nFireCooltime;

	int m_nFireType;
	int m_nInterval;
	int m_nChance;
	int m_nMaxCount;

	int m_nLength;
};