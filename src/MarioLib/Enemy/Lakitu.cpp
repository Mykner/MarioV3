#include "Lakitu.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "TypeNames.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(Lakitu)
	PROP_INT("ThrowType", VT_I4, -1, g_szThrowableTypes, "Lakitu"),
	PROP_BOOL("Winged", VT_BOOL, false, nullptr, "Lakitu")
END_IMPL_PROPERTYMAP()

Lakitu::Lakitu(Game *pGame, Stage *pStage) :
	EnemyObjectBase(pGame, pStage)
{
	m_nType = ENEMY_LAKITU;
	m_nState = STATE_IDLE;
	m_nBaseState = STATE_IDLE;

	m_bFlip = false;
	m_nMaxCount = 3;
	m_nThrowCooltime = 0;
	m_nMoveMode = 0;

	m_nThrowTypeIndex = -1;
	m_nThrowType = ENEMY_SPINY;
	for (int i = 0; ; i++)
	{
		if (g_nThrowableTypes[i] == ENEMY_SPINY)
		{
			m_nThrowTypeIndex = i;
			break;
		}
	}
}

Lakitu::~Lakitu()
{
}

void Lakitu::Render(int nColor, int nZOrder)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	int nFrame = GetSpriteIndex();

	int nCloudZOrder = Z_VEHICLE;
	if (nZOrder == -1)
		nZOrder = GetZOrder();

	if (nFrame != -1)
	{
		bool bShadow = m_pGame->m_bModernStyle && nZOrder != Z_MAP_SHADOW;
		float fAngle = 0.0f;

		if (IS_SPINNING)
		{
			fAngle = m_nStateFrame * -14;
			if (m_fXS < 0)
				fAngle *= -1;
			if (!m_bFlip)
				fAngle *= -1;
		}

		float _x = x - pt.x;
		float _y = y - pt.y;
		float fScaleX, fScaleY;
		GetScale(fScaleX, fScaleY);
		
		if (IS_CLASSIC)
		{
			(*m_ppSprite)->RenderToQueue(_x, _y, nFrame, m_bFlip, m_bFlipV, nColor,
				fScaleX, fScaleY, nZOrder, bShadow, fAngle);

			if (m_bWinged)
				RenderWing(nColor, nZOrder);
		}
		else
		{
			if (IsAlive())
			{
#define CLOUD_SPRITE_INDEX_CYCLE(from, cnt, delay) (from + (m_pGame->m_nGameFrame % (cnt * delay)) / delay)

				int nCloudFrame = CLOUD_SPRITE_INDEX_CYCLE(SPRIDX_LAKITU_MODERN_CLOUD1, 4, 8);
				(*m_ppSprite)->RenderToQueue(_x, _y,
					nCloudFrame,
					m_bFlip, m_bFlipV, nColor,
					fScaleX, fScaleY, nCloudZOrder, bShadow, fAngle);
			}

			if (nFrame == SPRIDX_LAKITU)
				nFrame = SPRIDX_LAKITU_MODERN;
			else if (nFrame = SPRIDX_LAKITU_HIDE)
				nFrame = SPRIDX_LAKITU_MODERN_HIDE;

			(*m_ppSprite)->RenderToQueue(_x, _y - 8,
				nFrame,
				m_bFlip, m_bFlipV, nColor,
				fScaleX, fScaleY, nZOrder, bShadow, fAngle);

			if (m_bWinged)
				RenderWing(nColor, nZOrder);
		}
	}
}

void Lakitu::ChangeState(int nState, ChangeStateInfo *pInfo)
{
	switch (nState)
	{
	case STATE_MOVE:
		if (!m_bFlip)
			m_fXS = -2.0f;
		else
			m_fXS = 2.0f;
		break;
	case STATE_RETREAT:
		m_fXS = -1.0f;
		break;
	case STATE_PRESSED:
		if (m_bWinged)
		{
			m_fYS = 0;
			ReleaseWing();
		}

		if (!IS_CLASSIC)
		{
			CUR_STAGE->CreateVehicle(m_fX, m_fY, VEHICLE_CLOUD);
		}

		EnemyObjectBase::ChangeState(nState, pInfo);
		return;
		break;
	case STATE_PUSHEDUP:		
	case STATE_BURNT:
		if (!IS_CLASSIC)
		{
			CUR_STAGE->CreateVehicle(m_fX, m_fY, VEHICLE_CLOUD);
		}

		EnemyObjectBase::ChangeState(nState, pInfo);
		return;
		break;
	default:
		EnemyObjectBase::ChangeState(nState, pInfo);
		return;
		break;
	}

	m_nNextState = nState;
}

void Lakitu::OnIdle()
{
	if (CAMERA->IsOutOfCamera(this))
		return;

	m_nLastCameraX = CAMERA->m_ptPos.x;
	m_nThrowType = g_nThrowableTypes[m_nThrowTypeIndex];

	ChangeState(STATE_MOVE);
}

void Lakitu::OnMove()
{
	CheckThrowSpiny();

	NaRect rcView = CAMERA->GetViewport();
	int nTargetX = CUR_PLAYER_OBJ->m_fX;

	if (m_bWinged)
		m_fYS = sin((m_nStateFrame * 5) * fToRadian) * 2.0f;

	if (CUR_STAGE->m_ptEnemyStop.x < rcView.right)
	{
		ChangeState(STATE_RETREAT);
	}
	else
	{
		switch (m_nMoveMode)
		{
		case 0:
			OnMoveInPage();
			break;
		case 1:
			OnMoveToTarget();
			break;
		}

		CheckAnotherLakitu();
	}

	m_nLastCameraX = CAMERA->m_ptPos.x;
}

void Lakitu::OnMoveInPage()
{
	NaRect rcView = CAMERA->GetViewport();
	int nTargetX = CUR_PLAYER_OBJ->m_fX;
	float fAccel = 0.06f;
	float fMaxSpeed = 1.0f;

	int nCameraOffset = CAMERA->m_ptPos.x - m_nLastCameraX;

	if (nTargetX > rcView.Center())
		nTargetX = rcView.Center();

	if (m_fX < rcView.left - 16 * 3)
	{
		m_nMoveMode = 1;
		m_bFlip = true;	
		m_fXS = 5.0f;
	}
	else if (!m_bFlip)
	{
		// <=========
		if (m_fX < nTargetX - 16*2)
		{
			m_fXS *= 0.85f;
			if (m_fXS >= -0.1f)
				m_bFlip = !m_bFlip;
		}
		else
		{
			if (nCameraOffset > 0 && m_fX < rcView.right - TILE_XS * 3)
			{
				m_fXS += fAccel;
			}
			else
				m_fXS -= fAccel;			

			if (m_fXS <= -fMaxSpeed)
			{
				m_fXS = -fMaxSpeed;
				if (nCameraOffset > 0)
					m_fXS = -fMaxSpeed * 0.5f;
			}
		}
	}
	else
	{
		// =========>
		if (m_fX > nTargetX + 16*2)
		{
			m_fXS *= 0.95f;
			if (m_fXS <= 0.1f)
				m_bFlip = !m_bFlip;
		}
		else
		{
			if (nCameraOffset > 0)
			{
				if (m_fX < nTargetX - 16 * 3)
					fMaxSpeed = 4.0f;
				else if (m_fX < nTargetX + 16 * 3)
					fMaxSpeed = 3.5f;
				else
					fMaxSpeed = 2.0f;
			}
			else
				fMaxSpeed = 1.0f;

			m_fXS += fAccel;

			if (m_fXS >= fMaxSpeed)
			{
				m_fXS = fMaxSpeed;
			}
		}
	}
}

void Lakitu::OnMoveToTarget()
{
	NaRect rcView = CAMERA->GetViewport();
	int nTargetX = CUR_PLAYER_OBJ->m_fX;
	float fAccel = 0.03f;

	if (nTargetX > rcView.Center())
		nTargetX = rcView.Center();

	int nCameraOffset = CAMERA->m_ptPos.x - m_nLastCameraX;

	if (!m_bFlip)
	{
		// Slow Down
		m_fXS *= 0.90f;
		if (m_fXS <= 0.1f)
		{
			m_fXS = 0;
			m_nMoveMode = 0;
		}
	}
	else
	{
		// =========>
		if (m_fX > nTargetX + 16 * 4)
		{
			m_bFlip = false;
		}
		else
		{
			// Fast Moving Forward
			float fMaxSpeed = 3.0f;
			if (nCameraOffset > 0)
				m_fXS = nCameraOffset + 1.0f;
			else
			{
				if (m_fX < nTargetX - 16 * 3)
					fMaxSpeed = 5.0f;
				else if (m_fX < nTargetX + (16 * 3))
					fMaxSpeed = 4.0f;
			}

			if (m_fXS < fMaxSpeed)
				m_fXS += (fAccel * 4);
		}
	}
}

void Lakitu::OnTrampolinJump()
{
	if (CheckHitBlock(1))
		return;

	if (m_bWinged)
	{
		if (m_fYS >= 0)
			ChangeState(STATE_MOVE);
	}
	else
	{
		if (m_fYS >= 0)
			ChangeState(STATE_FALL);
	}

	OnFall();
}

void Lakitu::OnPressed()
{
	if (m_nStateFrame > 40)
		ChangeState(STATE_DEAD);
}

bool Lakitu::IsAlive()
{
	return EnemyObjectBase::IsAlive();
}

bool Lakitu::CanBounceTrampolinMiniHorz()
{
	return false;
}

void Lakitu::CheckThrowSpiny()
{
	switch (m_nThrowCooltime)
	{
	case 16:
		{
			// Check Spiny's Count in Camera
			int nCount = CUR_STAGE->GetObjectCount(m_nThrowType, true);
			if (nCount < m_nMaxCount)
			{
				SIZE s = GetSize();
				int nCreateCount = 1;

				if (m_nThrowType == ITEM_THROWNCOIN)
					nCreateCount = 20;

				for (int i = 0; i < nCreateCount; i++)
				{
					GameObjectBase *pObj = nullptr;

					if (m_nThrowType >= ITEM_BEGIN && m_nThrowType <= ITEM_END)
						pObj = CUR_STAGE->CreateItem(m_fX, m_fY - s.cy, m_nThrowType);
					else if (m_nThrowType >= ENEMY_BEGIN && m_nThrowType <= ENEMY_END)
						pObj = CUR_STAGE->ReserveCreateGameObject(m_fX, m_fY - s.cy, m_nThrowType);

					if (pObj)
					{
						pObj->m_fXS = 0;

						if (m_nThrowType == ENEMY_SPINY)
							pObj->m_fYS = -2.0f;
						else
							pObj->m_fYS = -3.0f;

						pObj->m_nState = STATE_THROWN;

						if (m_nThrowType == ENEMY_BULLETBILL)
						{
							pObj->m_fXS = 3.0f;
							pObj->m_fYS = 0;
							pObj->m_nState = STATE_WALK;
						}

						pObj->m_bFlip = !m_bFlip;

						if (CUR_PLAYER_OBJ->m_fX < m_fX)
						{
							pObj->m_bFlip = true;
							pObj->m_fXS *= -1;
						}
						else
							pObj->m_bFlip = false;
					}
				}

				/*
				GameObjectBase* pObj = CUR_STAGE->ReserveCreateGameObject(m_fX, m_fY - s.cy, ENEMY_SPINY);
				if (pObj)
				{
					pObj->m_nState = STATE_THROWN;
					pObj->m_fXS = 0;
					pObj->m_fYS = -2.0f;
					pObj->m_bFlip = !m_bFlip;
				}
				*/
			}
			else
			{
				m_nThrowCooltime = 1;
			}
		}
		break;
	case 128:
		m_nThrowCooltime = 0;
		break;
	}

	if (m_nThrowType != -1)
	{
		if ((m_nThrowCooltime == 0 && rand() % 30 == 0) || m_nThrowCooltime > 0)
			m_nThrowCooltime++;
	}
}

void Lakitu::CheckAnotherLakitu()
{
	if (!IS_CLASSIC)
		return;

	if (m_nStateFrame % 10 == 0)
	{
		BEGIN_VECTOR_LOOP(GameObjectBase*, CUR_STAGE->m_vecEnemy) {
			if (pObj == this)
				continue;
			if (abs(m_fX - pObj->m_fX) > GameDefaults::nPageWidth)
				continue;
			if (pObj->m_nType == ENEMY_LAKITU)
			{
				if (m_fX < pObj->m_fX)
				{
					if (pObj->IsOutOfCamera())
						pObj->ChangeState(STATE_DEAD);
					else
						ChangeState(STATE_RETREAT);
				}
			}
		} END_VECTOR_LOOP();
	}
}

bool Lakitu::CanPressed(int nPower)
{
	if (m_nThrowType >= ITEM_BEGIN && m_nThrowType <= ITEM_END)
		return false;

	return true;
}

bool Lakitu::CanBurnt(int nType)
{
	if (m_nThrowType >= ITEM_BEGIN && m_nThrowType <= ITEM_END)
		return false;

	return true;
}

bool Lakitu::CanDamageEnemy()
{
	if (m_nThrowType >= ITEM_BEGIN && m_nThrowType <= ITEM_END)
		return false;

	return EnemyObjectBase::CanDamageEnemy();
}

std::vector<POINT> Lakitu::GetWingPosition()
{
	POINT pt;
	SIZE s = GetSize();
	pt.x = -11;
	pt.y = -5;

	std::vector<POINT> vec;
	vec.push_back(pt);

	pt.x = +11;
	vec.push_back(pt);

	return vec;
}

int Lakitu::GetWingSpriteIndex()
{
	int nFrame = SPRIDX_WING1;
	if (m_nStateFrame % 16 >= 8)
		nFrame = SPRIDX_WING2;

	return nFrame;
}

int Lakitu::GetSpriteIndex()
{
	if (EnemyObjectBase::GetSpriteIndex() == -1)
		return -1;

	int nFrame;
	switch (m_nState)
	{
	case STATE_PRESSED:
	case STATE_PUSHEDUP:
	case STATE_BURNT:
		nFrame = SPRIDX_LAKITU;
		break;
	default:
		if (m_nThrowCooltime > 0 && m_nThrowCooltime <= 16)
			nFrame = SPRIDX_LAKITU_HIDE;
		else 
			nFrame = SPRIDX_LAKITU;
		break;
	}

	return nFrame;
}
