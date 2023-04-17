//
// Deprecated
//
#include "CloudItem.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "FModManager.h"
#include "Stage.h"
#include "RenderManager.h"

CloudItem::CloudItem(Game *pGame, Stage *pStage) :
	ItemObjectBase(pGame, pStage)
{
	m_nType = ITEM_CLOUD;
	m_nState = STATE_ITEMPOP;
	m_nBaseState = STATE_FLY;

	RequestSprites(pStage);
}

CloudItem::~CloudItem()
{
}

void CloudItem::RequestSprites(Stage * pStage)
{
	ItemObjectBase::RequestSprites(pStage);

	if (pStage == nullptr)
		pStage = CUR_STAGE;

	m_ppSpriteVehicle = &pStage->m_pSpriteVehicle;
}

void CloudItem::OnItemPop()
{
	if (m_nStateFrame == 30)
		m_fYS = -1.6f;

	if (m_nStateFrame == 42)
	{
		GameObjectBase *pObj = CUR_STAGE->CreateVehicle(m_fX, m_fY, VEHICLE_CLOUD);
		pObj->m_fYS = -1.0f;

		SetState(STATE_DEAD);
	}
}

void CloudItem::OnItemPopDown()
{
	if (m_nStateFrame == 4)
		m_fYS = 2.0f;

	if (m_nStateFrame == 4 + 4)
	{
		GameObjectBase *pObj = CUR_STAGE->CreateVehicle(m_fX, m_fY, VEHICLE_CLOUD);
		pObj->m_fYS = 1.0f;

		SetState(STATE_DEAD);
	}
}

void CloudItem::OnFly()
{
	if (m_nStateFrame == 1)
	{
		GameObjectBase *pObj = CUR_STAGE->CreateVehicle(m_fX, m_fY, VEHICLE_CLOUD);

		SetState(STATE_DEAD);
	}
}

bool CloudItem::CanApplied()
{
	return false;
}

void CloudItem::Render(int nColor, int nZOrder)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	int nFrame = GetSpriteIndex();

	if (nFrame >= 0)
	{
		int nZOrder = Z_ITEM;
		if (m_nState == STATE_ITEMPOP)
			nZOrder = Z_BEHIND_MAP;
		else if (m_nState == STATE_IDLE)
			nZOrder = Z_COMMON_OBJ;

		bool bShadow = (m_pGame->m_bModernStyle);
		float fScaleX, fScaleY;
		GetScale(fScaleX, fScaleY);

		(*m_ppSpriteVehicle)->RenderToQueue(x - pt.x, y - pt.y, nFrame,
			false, m_bFlipV, nColor, fScaleX, fScaleY, nZOrder, bShadow);
	}
}

int CloudItem::GetSpriteIndex()
{
	if (m_nState == STATE_ITEMPOP && m_nStateFrame < 30)
		return -1;

	if (m_nState == STATE_ITEMPOPDOWN && m_nStateFrame < 30)
		return -1;

	return SPRIDX_VEHICLE_CLOUD;
}

void CloudItem::GetScale(float & fScaleX, float & fScaleY)
{
	fScaleX = m_fScaleX;
	fScaleY = m_fScaleY;

	if (m_nState == STATE_ITEMPOP && m_nStateFrame >= 30)
	{
		float fVal = 1.0f;
		if (m_nStateFrame < 42)
			fVal = 0.75f;
		else if (m_nStateFrame < 42 + 12)
		{
			fVal = 1.1f;
		}

		fScaleX = fScaleY = fVal;
	}
}
