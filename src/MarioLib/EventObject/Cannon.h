#pragma once

#include "Common.h"

#include "EventObjectBase.h"
#include "PropertyMap.h"

enum CannonDirections
{
	CANNONDIR_L = 0,
	CANNONDIR_LT,
	CANNONDIR_T,
	CANNONDIR_RT,
	CANNONDIR_R,
	CANNONDIR_RB,
	CANNONDIR_B,
	CANNONDIR_LB,
};

class Cannon : public EventObjectBase
{
public:
	Cannon(Game *pGame, Stage *pStage);
	virtual ~Cannon();

	virtual void RequestSprites(Stage *pStage = nullptr);

	virtual void OnBeforeProcess();
	virtual void OnAfterProcess();

	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	virtual void OnStand();

	virtual bool CanPushedUp();

	virtual void GetScale(float &fScaleX, float &fScaleY);
	virtual SIZE GetSize();

	int m_nFireCooltime;

	int m_nFireType;
	int m_nFireTypeIndex;
	int m_nInterval;
	int m_nChance;
	int m_nMaxCount;

	int m_nDirection;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("Direction", &m_nDirection)
		DECL_PROP("Interval", &m_nInterval)
		DECL_PROP("Chance", &m_nChance)
		DECL_PROP("FireType", &m_nFireTypeIndex)
	END_DECLARE_PROPERTYMAP()
};