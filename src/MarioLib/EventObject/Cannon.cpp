#include "Cannon.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "FModManager.h"
#include "Stage.h"
#include "Camera.h"
#include "SoundManager.h"

#include "TypeNames.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(Cannon)
	PROP_INT("Direction",	VT_I4, CANNONDIR_LT,	nullptr, "Cannon"),
	PROP_INT("Interval",	VT_I4, 3,				nullptr, "Cannon"),
	PROP_INT("Chance",		VT_I4, 30,				nullptr, "Cannon"),
	PROP_INT("FireType",	VT_I4, -1,				g_szCannonBulletTypes, "Cannon"),
END_IMPL_PROPERTYMAP()

Cannon::Cannon(Game *pGame, Stage *pStage) :
	EventObjectBase(pGame, pStage)
{
	m_nType = EVENTOBJECT_CANNON;
	m_nState = STATE_STAND;

	RequestSprites(pStage);

	m_nFireType = ENEMY_CANNONBALL;
	for (int i = 0; ; i++)
	{
		if (g_nCannonBulletTypes[i] == m_nFireType)
		{
			m_nFireTypeIndex = i;
			break;
		}
	}

	m_nInterval = 3;
	m_nChance = 30;
	m_nMaxCount = -1;

	// Default Dir
	m_nDirection = CANNONDIR_LT;

	m_bCanCollide = true;
	m_nFireCooltime = 0;
}

Cannon::~Cannon()
{
}

void Cannon::RequestSprites(Stage * pStage)
{
	if (pStage == nullptr)
		pStage = CUR_STAGE;

	m_ppSprite = &pStage->m_pSpriteEnemy;
}

void Cannon::OnBeforeProcess()
{
	CheckCollision();
	//CheckCollisionPlayer();
}

void Cannon::OnAfterProcess()
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

void Cannon::Render(int nColor, int nZOrder)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	float _x = x - pt.x;
	float _y = y - pt.y;

	bool bShadow = (m_pGame->m_bModernStyle);

	if (nZOrder == -1)
		nZOrder = Z_COMMON_OBJ;

	float fScaleX, fScaleY;
	GetScale(fScaleX, fScaleY);

	bool bOnGround = true;
	switch (m_nDirection)
	{
	case CANNONDIR_RB:
	case CANNONDIR_B:
	case CANNONDIR_LB:
		bOnGround = false;
		break;
	}

	float fAngle = (m_nDirection - 2) * (45.0f);
	if (bOnGround)
	{
		(*m_ppSprite)->RenderToQueue(_x, _y - 2, SPRIDX_CANNON_HEAD,
			m_bFlip, m_bFlipV, nColor, fScaleX, fScaleY, nZOrder, bShadow, fAngle);

		(*m_ppSprite)->RenderToQueue(_x, _y, SPRIDX_CANNON_BODY,
			m_bFlip, m_bFlipV, nColor, 1.0f, 1.0f, nZOrder + 1, bShadow);
	}
	else
	{
		(*m_ppSprite)->RenderToQueue(_x, _y, SPRIDX_CANNON_HEAD,
			m_bFlip, m_bFlipV, nColor, fScaleX, fScaleY, nZOrder, bShadow, fAngle);

		(*m_ppSprite)->RenderToQueue(_x, _y - 4, SPRIDX_CANNON_BODY,
			m_bFlip, !m_bFlipV, nColor, 1.0f, 1.0f, nZOrder + 1, bShadow);
	}
}

void Cannon::OnStand()
{
	if (m_nFireCooltime > 0)
		m_nFireCooltime--;

	switch (m_nDirection % 8)
	{
	case CANNONDIR_L:	
	case CANNONDIR_LT:	
	case CANNONDIR_T:	
	case CANNONDIR_RT:	
	case CANNONDIR_R:	
		CheckFalling();
		break;
	case CANNONDIR_RB:
	case CANNONDIR_B:
	case CANNONDIR_LB:
		break;
	}

	if (m_nInterval != 0 &&
		m_nStateFrame % (m_nInterval * 24) == 23 &&
		rand() % 100 <= m_nChance)
	{
		if (CAMERA->IsOutOfCamera(this))
			return;

		if (m_nDirection < 0)
		{
			for (; m_nDirection < 0; )
			{
				m_nDirection += 8;
			}
		}

		m_nFireType = g_nCannonBulletTypes[m_nFireTypeIndex];
		if (m_nFireType > 0)
		{
			/*
			if (m_nFireType >= ENEMY_BEGIN && m_nFireType <= ENEMY_END)
			{
				int nDistance = abs(m_fX - CUR_PLAYER_OBJ->m_fX);
				if (nDistance <= 16)
					return;
			}
			*/

			int nCount = CUR_STAGE->GetObjectCount(m_nFireType, true);

			SIZE s = GetSize();
			if (m_nMaxCount == -1 || nCount < m_nMaxCount)
			{
				NaRect rcView = CAMERA->GetViewport();
				
				// Static Position
				int nGenX = m_fX;
				int nGenY = m_fY - 3;
			
				SIZE s = GetSize();

				// Only 1..
				int nCreateCount = 1;
				for (int i = 0; i < nCreateCount; i++)
				{
					GameObjectBase *pObj = nullptr;

					if (m_nFireType >= ITEM_BEGIN && m_nFireType <= ITEM_END)
						pObj = CUR_STAGE->CreateItem(nGenX, nGenY, m_nFireType);
					else if (m_nFireType >= ENEMY_BEGIN && m_nFireType <= ENEMY_END)
						pObj = CUR_STAGE->AddEnemy(nGenX, nGenY, m_nFireType);

					if (pObj)
					{
						pObj->m_fXS = 0;
						pObj->m_fYS = 0;

						float fSpeed = 1.5f;
						switch (m_nDirection % 8)
						{
						case CANNONDIR_L:	pObj->m_fXS = -fSpeed; break;
						case CANNONDIR_LT:	pObj->m_fXS = -(fSpeed * 0.7f); pObj->m_fYS = -(fSpeed * 0.7f); break;
						case CANNONDIR_T:	pObj->m_fYS = -fSpeed; break;
						case CANNONDIR_RT:	pObj->m_fXS = (fSpeed * 0.7f); pObj->m_fYS = -(fSpeed * 0.7f); break;
						case CANNONDIR_R:	pObj->m_fXS = fSpeed; break;
						case CANNONDIR_RB:	pObj->m_fXS = (fSpeed * 0.7f); pObj->m_fYS = (fSpeed * 0.7f); break;
						case CANNONDIR_B:	pObj->m_fYS = fSpeed; break;
						case CANNONDIR_LB:	pObj->m_fXS = -(fSpeed * 0.7f); pObj->m_fYS = (fSpeed * 0.7f); break;
						}

						// Effects
						{
							CreateParameter param;
							param.nType = EFFECT_DUST;
							param.fScaleX = 1.0f;
							param.fScaleY = 1.0f;
							param.fXS = pObj->m_fXS * -0.5f;
							param.fYS = pObj->m_fYS * -0.5f;
							CUR_STAGE->CreateEffect(
								m_fX + (pObj->m_fXS * 10),
								m_fY + (pObj->m_fYS * 10) - 8,
								EFFECT_DUST, &param);
						}

						pObj->m_nState = STATE_THROWNFLY;

						if (m_nFireType != ENEMY_CANNONBALL)
						{
							if (CUR_PLAYER_OBJ->m_fX < m_fX)
								pObj->m_bFlip = true;
							else
								pObj->m_bFlip = false;
						}
					}
				}

				m_nFireCooltime = 16;
				SOUND_MANAGER->PlaySoundEffect(L"Cannon");
			}
		}
	}
}

bool Cannon::CanPushedUp()
{
	switch (m_nDirection % 8)
	{
	case CANNONDIR_RB:
	case CANNONDIR_B:
	case CANNONDIR_LB:
		return false;
	}

	return true;
}

void Cannon::GetScale(float & fScaleX, float & fScaleY)
{
	fScaleX = m_fScaleX;
	fScaleY = m_fScaleY;

	if (m_nFireCooltime > 0 && m_pGame->m_bModernStyle)
	{
		fScaleX *= (1.0f + (m_nFireCooltime / 80.0f));
		fScaleY *= (1.0f + (m_nFireCooltime / 80.0f));
	}
}

SIZE Cannon::GetSize()
{
	SIZE s = { 16, 16 };
	return s;
}
