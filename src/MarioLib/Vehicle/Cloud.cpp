#include "Cloud.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "FModManager.h"
#include "Stage.h"

Cloud::Cloud(Game *pGame, Stage *pStage) :
	VehicleObjectBase(pGame, pStage)
{
	m_nType = VEHICLE_CLOUD;
	m_nState = STATE_FLY;
	RequestSprites(pStage);

	m_nRideType = STATE_STAND;

	m_nLifeFrame = 1200;
}

Cloud::~Cloud()
{
}

void Cloud::RequestSprites(Stage * pStage)
{
	if (pStage == nullptr)
		pStage = CUR_STAGE;

	m_ppSprite = &pStage->m_pSpriteEnemy;
}

void Cloud::OnAfterProcess()
{
	m_fX += m_fXS;
	m_fY += m_fYS;

	if (m_nDamageCooltime > 0)
	{
		m_nDamageCooltime--;
	}

	if (IsAlive() && m_pPassenger)
	{
		m_pPassenger->m_fX += m_fXS;
		m_pPassenger->m_fY += m_fYS;
	}

	m_nStateFrame++;
	if (m_nNextState != -1)
	{
		m_nState = m_nNextState;
		m_nNextState = -1;
		m_nStateFrame = 0;

		OnChangeState(m_nState);
	}
}

void Cloud::OnFly()
{
	// Random movement
	float fCur = (5.0f / 2.0f) * sin((-90 + (3 * m_nStateFrame)) * fToRadian);
	float fNew = (5.0f / 2.0f) * sin((-90 + (3 * (m_nStateFrame + 1))) * fToRadian);
	float fIdleMove = (fNew - fCur);
	
	bool bFront = m_bFlip ? IsInputHold(INPUT_L) : IsInputHold(INPUT_R);
	bool bBack = m_bFlip ? IsInputHold(INPUT_R) : IsInputHold(INPUT_L);
	bool bUp = IsInputHold(INPUT_U);
	bool bDown = IsInputHold(INPUT_D);
	float fMark = m_bFlip ? (-1) : 1;

	float fAccel = 0.1f;
	float fMaxSpeed = 1.5f;

	if (bFront && !bBack)
	{
		if (abs(m_fXS) < abs(fMaxSpeed))
			m_fXS += (fMark) * (fAccel);
	}
	else
	{
		// StopPress MoveKey -> SlowDown
		m_fXS += (-fMark) * (fAccel / 3);
		if (m_bFlip ? (m_fXS > 0) : (m_fXS < 0))
			m_fXS = 0;
	}

	if (IsInputHold(INPUT_L))
	{
		m_bFlip = true;
	}
	if (IsInputHold(INPUT_R))
	{
		m_bFlip = false;
		if (m_pPassenger)
			m_pPassenger->m_bFlip = m_bFlip;
	}

	if (m_pPassenger && (bFront || bBack))
		m_pPassenger->m_bFlip = m_bFlip;
	
	if (bUp && !bDown)
	{
		if (m_fYS > -(fMaxSpeed))
			m_fYS -= (fAccel * 2);
	}
	else if (!bUp && bDown)
	{
		if (m_fYS < fMaxSpeed)
			m_fYS += (fAccel * 2);
	}
	else if (!bUp && !bDown)
	{
		StopCombo();

		// StopPress MoveKey -> SlowDown
		if (m_fYS < 0)
			m_fYS += (fAccel / 5);
		else if (m_fYS > 0)
			m_fYS -= (fAccel / 5);
		
		if (abs(m_fYS) < 0.5f)
			m_fYS = 0;
	}
	
	if (fIdleMove < 0 && m_fYS > fIdleMove)
		m_fYS += fIdleMove;
	else if (fIdleMove > 0 && m_fYS < fIdleMove)
		m_fYS += fIdleMove;

	if (m_fYS > 0 && m_bCollision[COLLISION_BOTTOM])
	{
		m_fYS = 0;
		m_fY = m_ptCollision[COLLISION_BOTTOM].y;
	}
	if (m_fYS < 0 && m_pPassenger && m_bCollision[COLLISION_TOP])
	{
		NaRect rc = GetRect();
		SIZE size;
		size.cy = rc.Height();

		if (!m_bCollision[COLLISION_BOTTOM])
			m_fYS = 1.0f;
		float fOffsetY = (((m_ptCollision[COLLISION_TOP].y + 1)) + size.cy) - m_fY;

		m_fY += fOffsetY;
		m_pPassenger->m_fY += fOffsetY;

		// Hit Block
		int nPower = m_pPassenger->GetPower(POWERTYPE_HITBLOCK);
		if (nPower > POWER_HITBLOCK_UNKNOWN)
		{
			CUR_STAGE->HitBlock(
				m_ptCollision[COLLISION_TOP].x / TILE_XS, 
				(m_ptCollision[COLLISION_TOP].y) / TILE_YS, 
				nPower, 
				m_pPassenger,
				m_pPassenger);
		}
	}
	else if (m_fXS < 0 && m_bCollision[COLLISION_LEFT])
		OnReachWall(true);
	else if (m_fXS > 0 && m_bCollision[COLLISION_RIGHT])
		OnReachWall(false);

	if (CheckPressEnemy(POWER_PRESS_NORMAL))
	{
		if (!m_bCollision[COLLISION_TOP])
			m_fYS = -1.0f;
	}

	if (m_fY > (CUR_STAGE->m_sizeTile.cy * TILE_YS) - 24)
	{
		m_fY = (CUR_STAGE->m_sizeTile.cy * TILE_YS) - 24;
		m_fYS = -1.0f;
	}
	else if (m_fY < 36)
		m_fY = 36;

	if (m_nLifeFrame > 0)
	{
		m_nLifeFrame--;
		if (m_nLifeFrame == 0)
		{
			OnGetOffPassenger();
			ChangeState(STATE_DEAD);
		}
	}

	if (m_pPassenger)
		PositionPassenger();
}

void Cloud::OnFall()
{
	ChangeState(STATE_FLY);
}

void Cloud::OnItemPop()
{
	if (m_nStateFrame == 30)
		m_fYS = -1.6f;

	if (m_nStateFrame == 42)
	{
		SetState(STATE_FLY);
		m_fYS = -1.0f;
	}
}

void Cloud::OnItemPopDown()
{
	if (m_nStateFrame == 4)
		m_fYS = 2.0f;

	if (m_nStateFrame == 4 + 4)
	{
		SetState(STATE_FLY);
		m_fYS = 1.0f; 
	}
}

void Cloud::PositionPassenger()
{
	SIZE s = GetSize();

	m_pPassenger->m_fX = m_fX;
	m_pPassenger->m_fY = m_fY - (s.cy / 2);
}

void Cloud::OnRidePassenger(GameObjectBase * pPassenger)
{
	VehicleObjectBase::OnRidePassenger(pPassenger);

	if (!m_bCollision[COLLISION_BOTTOM])
		m_fYS = 0.5f;
}

void Cloud::OnGetOffPassenger()
{
	m_fXS = 0;
	m_fYS = 0;

	VehicleObjectBase::OnGetOffPassenger();
}

bool Cloud::CanPressEnemy()
{
	if (m_pPassenger)
	{
		return true;
	}
	return false;
}

int Cloud::GetSpriteIndex()
{
	if (m_nState == STATE_ITEMPOP && m_nStateFrame < 30)
		return -1;

	if (m_nLifeFrame < 8 * 24)
	{
		if (m_pGame->m_nGameFrame % 2 < 1)
			return -1;
	}

#define CLOUD_SPRITE_INDEX_CYCLE(from, cnt, delay) (from + (m_pGame->m_nGameFrame % (cnt * delay)) / delay)

	int nCloudFrame = CLOUD_SPRITE_INDEX_CYCLE(SPRIDX_LAKITU_MODERN_CLOUD1, 4, 8);

	return nCloudFrame;
}

NaRect Cloud::GetRect()
{
	NaRect rcMe = GameObjectBase::GetRect();
	if (m_pPassenger == nullptr)
		return rcMe;

	NaRect rcP = m_pPassenger->GetRect();

	// Add two rect.
	if (rcP.left < rcMe.left)
		rcMe.left = rcP.left;
	if (rcP.right > rcMe.right)
		rcMe.right = rcP.right;
	if (rcP.top < rcMe.top)
		rcMe.top = rcP.top;
	if (rcP.bottom > rcMe.bottom)
		rcMe.bottom = rcP.bottom;

	return rcMe;
}
