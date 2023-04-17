//
// Deprecated
//
#include "YoshiEgg.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "FModManager.h"
#include "Stage.h"

YoshiEgg::YoshiEgg(Game *pGame, Stage *pStage) :
	ItemObjectBase(pGame, pStage)
{
	m_nType = ITEM_YOSHIEGG;
	m_nState = STATE_ITEMPOP;
	
	m_nBaseState = STATE_STAND;
}

YoshiEgg::~YoshiEgg()
{
}

void YoshiEgg::OnItemPop()
{
	if (m_nStateFrame == 30)
		m_fYS = -1.5f;

	if (m_nStateFrame == 42)
	{
		ChangeState(m_nBaseState);
	}
}

void YoshiEgg::OnStand()
{
	ItemObjectBase::OnStand();

	if (m_nStateFrame == 42)
	{
		CUR_STAGE->CreateVehicle(m_fX, m_fY, VEHICLE_YOSHI);
		
		ChangeState(STATE_DEAD);
	}
}

bool YoshiEgg::CanApplied()
{
	return false;
}

int YoshiEgg::GetSpriteIndex()
{
	if (m_nState == STATE_ITEMPOP && m_nStateFrame < 30)
		return -1;

	if (m_nState == STATE_ITEMPOPDOWN && m_nStateFrame < 4)
		return -1;

	return SPRIDX_YOSHIEGG;
}
