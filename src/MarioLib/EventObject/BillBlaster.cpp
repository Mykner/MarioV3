#include "BillBlaster.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "FModManager.h"
#include "Stage.h"
#include "Camera.h"
#include "SoundManager.h"

#include "TypeNames.h"

BillBlaster::BillBlaster(Game *pGame, Stage *pStage) :
	EventObjectBase(pGame, pStage)
{
	m_nType = EVENTOBJECT_BILLBLASTER;
	m_nState = STATE_STAND;
	
	RequestSprites(pStage);

	m_nFireType = ENEMY_BULLETBILL;
	m_nInterval = 10;
	m_nChance = 100;
	m_nMaxCount = 5;

	m_bCanCollide = true;

	m_nFireCooltime = 0;
	m_nLength = 1;
}

BillBlaster::~BillBlaster()
{
}

void BillBlaster::RequestSprites(Stage * pStage)
{
	if (pStage == nullptr)
		pStage = CUR_STAGE;

	m_ppSprite = &pStage->m_pSprite;
}

void BillBlaster::OnBeforeProcess()
{
	CheckCollision();
	//CheckCollisionPlayer();
}

void BillBlaster::OnAfterProcess()
{
	m_fX += m_fXS;
	m_fY += m_fYS;

	m_nStateFrame++;
	if (m_nNextState != -1)
	{
		m_nState = m_nNextState;
		m_nNextState = -1;
		m_nStateFrame = 0;

		OnChangeState(m_nState);
	}
}

void BillBlaster::Render(int nColor, int nZOrder)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	float _x = x - pt.x;
	float _y = y - pt.y;

	bool bShadow = (m_pGame->m_bModernStyle);

	if (nZOrder == -1)
		nZOrder = Z_COMMON_OBJ;

	float fScaleX, fScaleY;
	GetScale(fScaleX, fScaleY);

	(*m_ppSprite)->RenderToQueue(
		_x, 
		_y - (16 * (m_nLength-1)), 
		TILE_CANNONHEAD_T, 
		m_bFlip, m_bFlipV, nColor, fScaleX, fScaleY, nZOrder, bShadow);

	if (m_nLength > 1)
	{
		(*m_ppSprite)->RenderToQueue(
			_x,
			_y - (16 * (m_nLength-2)),
			TILE_CANNONHEAD_B,
			m_bFlip, m_bFlipV, nColor, 1.0f, 1.0f, nZOrder, bShadow);
	}

	for (int i = 3; i <= m_nLength; i++)
	{
		(*m_ppSprite)->RenderToQueue(
			_x,
			_y - 16 * (i - 3),
			TILE_CANNONBODY,
			m_bFlip, m_bFlipV, nColor, 1.0f, 1.0f, nZOrder, bShadow);
	}
}

void BillBlaster::OnStand()
{
	if (m_nFireCooltime > 0)
		m_nFireCooltime--;

	CheckFalling();

	int nInterval = m_nInterval;
	if (nInterval <= 0)
		nInterval = 1;
	if (m_nStateFrame % (nInterval * 24) == 23 && 
		rand() % 100 <= m_nChance)
	{
		if (CAMERA->IsOutOfCamera(this))
			return;

		if (m_nFireType > 0)
		{
			if (m_nFireType >= ENEMY_BEGIN && m_nFireType <= ENEMY_END)
			{
				int nDistance = abs(m_fX - CUR_PLAYER_OBJ->m_fX);
				if (nDistance <= 16)
					return;
			}

			int nCount = CUR_STAGE->GetObjectCount(m_nFireType, true);

			SIZE s = GetSize();
			if (m_nMaxCount == -1 || nCount < m_nMaxCount)
			{
				NaRect rcView = CAMERA->GetViewport();
				
				// Static Position
				int nGenX = m_fX;
				int nGenY = m_fY - (s.cy - 16) - 2;
			
				SIZE s = GetSize();
				int nCreateCount = 1;

				if (m_nFireType == ITEM_THROWNCOIN)
					nCreateCount = 20;

				bool bShootToLeft = CUR_PLAYER_OBJ->m_fX < m_fX;
				for (int i = 0; i < nCreateCount; i++)
				{
					GameObjectBase *pObj = nullptr;

					if (m_nFireType >= ITEM_BEGIN && m_nFireType <= ITEM_END)
						pObj = CUR_STAGE->CreateItem(nGenX, nGenY, m_nFireType);
					else if (m_nFireType >= ENEMY_BEGIN && m_nFireType <= ENEMY_END)
						pObj = CUR_STAGE->AddEnemy(nGenX, nGenY, m_nFireType);

					if (pObj)
					{
						if (m_nFireType == ENEMY_BULLETBILL)
							pObj->m_fXS = 2.0f;
						else
							pObj->m_fXS = 3.0f;
						pObj->m_fYS = 0;
						pObj->m_nState = STATE_THROWN;

						if (bShootToLeft)
						{
							pObj->m_bFlip = true;
							pObj->m_fXS *= -1;
						}
						else
							pObj->m_bFlip = false;
					}
				}

				// Effects
				{
					CreateParameter param;
					param.nType = EFFECT_DUST;
					param.fScaleX = 1.0f;
					param.fScaleY = 1.0f;
					param.fXS = bShootToLeft ? -0.5f : 0.5f;
					param.fYS = 0;
					CUR_STAGE->CreateEffect(
						nGenX + (bShootToLeft ? -10 : 10),
						nGenY - 6,
						EFFECT_DUST, &param);

					param.fScaleX = 1.5f;
					param.fScaleY = 1.5f;
					param.fXS = !bShootToLeft ? -1.0f : 1.0f;
					CUR_STAGE->CreateEffect(
						nGenX + (!bShootToLeft ? -10 : 10),
						nGenY - 6,
						EFFECT_DUST, &param);
				}

				m_nFireCooltime = 16;
				SOUND_MANAGER->PlaySoundEffect(L"Cannon");
			}
		}
	}
}

bool BillBlaster::CanPushedUp()
{
	return true;
}

void BillBlaster::GetScale(float & fScaleX, float & fScaleY)
{
	fScaleX = m_fScaleX;
	fScaleY = m_fScaleY;

	if (m_nFireCooltime > 0 && m_pGame->m_bModernStyle)
	{
		fScaleX *= (1.0f + (m_nFireCooltime / 80.0f));
		fScaleY *= (1.0f + (m_nFireCooltime / 80.0f));
	}
}

NaRect BillBlaster::GetRect()
{
	/*
	SIZE s = GetSize();
	NaRect rc;
	rc.left = m_fX;
	rc.top = m_fY;
	rc.right = m_fX + s.cx - 1;
	rc.bottom = m_fY + s.cy - 1;

	return rc;
	*/

	return __super::GetRect();
}

NaRectT<float> BillBlaster::GetRectFloat()
{
	/*
	SIZE s = GetSize();
	NaRectT<float> rc;
	rc.left = m_fX;
	rc.top = m_fY;
	rc.right = m_fX + s.cx - 1;
	rc.bottom = m_fY + s.cy - 1;

	return rc;
	*/

	return __super::GetRectFloat();
}

SIZE BillBlaster::GetSize()
{
	SIZE s = { 16, 16 * m_nLength };
	return s;
}
