#include "ClownCar.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "SoundManager.h"
#include "Stage.h"
#include "RenderManager.h"

ClownCar::ClownCar(Game *pGame, Stage *pStage) :
	VehicleObjectBase(pGame, pStage)
{
	m_nType = VEHICLE_CLOWNCAR;
	m_nState = STATE_FLY;

	m_nRideType = STATE_STAND;

	m_nBounceCooltime = 0;
}

ClownCar::~ClownCar()
{
}

void ClownCar::OnAfterProcess()
{
	m_fX += m_fXS;
	m_fY += m_fYS;

	PositionPassenger();

	if (m_nDamageCooltime > 0)
	{
		m_nDamageCooltime--;
	}

	if (m_nBounceCooltime > 0)
		m_nBounceCooltime--;

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

void ClownCar::OnFly()
{
	bool bFront = m_bFlip ? IsInputHold(INPUT_L) : IsInputHold(INPUT_R);
	bool bBack = m_bFlip ? IsInputHold(INPUT_R) : IsInputHold(INPUT_L);
	bool bUp = IsInputHold(INPUT_U);
	bool bDown = IsInputHold(INPUT_D);
	float fMark = m_bFlip ? (-1) : 1;

	float fAccel = 0.1f;
	float fMaxSpeed = 1.5f;
	float fMaxGravity = 1.0f;
	if (m_pPassenger && m_pPassenger->m_nDamageGroup == DAMAGEGROUP_ENEMY)
	{
		//fAccel = 0.025f;
		fMaxSpeed = 0.8f;
		fMaxGravity = 0.7f;
	}

	if (bFront && !bBack)
	{
		if ((fMark > 0 && m_fXS < fMaxSpeed) ||
			(fMark < 0 && m_fXS > -fMaxSpeed))
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
			m_fYS += (fAccel / 5);
	}
	else if (!bUp && !bDown)
	{
		StopCombo();

		// StopPress MoveKey -> SlowDown
		if (m_fYS < 0)
			m_fYS += (fAccel / 5);
		else if (m_fYS > 0)
			m_fYS -= (fAccel / 5);
	}

	OnApplyGravity(0.3f, fMaxGravity);

	if (m_fYS > 0 && m_bCollision[COLLISION_BOTTOM])
	{
		OnBounce();
		m_fYS *= -2.5f;
		m_fY = m_ptCollision[COLLISION_BOTTOM].y;
	}
	if (m_fYS < 0 && m_pPassenger && m_bCollision[COLLISION_TOP])
	{
		NaRect rc = GetRect();
		SIZE size;
		size.cy = rc.Height();

		if (!m_bCollision[COLLISION_BOTTOM])
		{
			OnBounce();
			m_fYS *= -2.5f;
		}

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
	{
		OnReachWall(true);
		
		m_fXS *= -1.2f;
		OnBounce();
	}
	else if (m_fXS > 0 && m_bCollision[COLLISION_RIGHT])
	{
		OnReachWall(false);
		
		m_fXS *= -1.2f;
		OnBounce();
	}

	if (CheckPressEnemy(POWER_PRESS_NORMAL))
	{
		if (!m_bCollision[COLLISION_TOP])
		{
			OnBounce();
			if (m_fYS > 0)
				m_fYS *= -2.5f;
			else
				m_fYS = -3.0f;
		}
	}

	if (m_fY < 36)
		m_fY = 36;
}

void ClownCar::PositionPassenger()
{
	if (m_pPassenger == nullptr || !IsAlive())
		return;

	SIZE s = GetSize();

	auto sizePassenger = m_pPassenger->GetSize();
	if (sizePassenger.cy > 16)
	{
		m_pPassenger->m_fX = m_fX;
		m_pPassenger->m_fY = m_fY - (s.cy * 0.5f);
	}
	else
	{
		m_pPassenger->m_fX = m_fX;
		m_pPassenger->m_fY = m_fY - (s.cy * 0.7f);
	}
}

void ClownCar::OnRidePassenger(GameObjectBase * pPassenger)
{
	VehicleObjectBase::OnRidePassenger(pPassenger);

	m_bFlip = pPassenger->m_bFlip;

	if (!m_bCollision[COLLISION_BOTTOM])
		m_fYS = 0.5f;

	SOUND_MANAGER->PlaySoundEffect(L"Pressed", this);
}

void ClownCar::OnGetOffPassenger()
{
	VehicleObjectBase::OnGetOffPassenger();
}

void ClownCar::OnDamagedPassenger()
{
	if (m_pPassenger && m_pPassenger->m_nDamageGroup != DAMAGEGROUP_ENEMY)
	{
		if (m_pPassenger->m_nShape == SHAPE_MINI)
		{
			m_nDamageCooltime = GameDefaults::nDamageCooltime;
			OnGetOffPassenger();
		}
		else
		{
			m_pPassenger->m_nDamageCooltime = GameDefaults::nDamageCooltime;
			//m_pPassenger->ChangeState(STATE_BURNT);

			m_pPassenger->m_pVehicle = nullptr;
			((ControllableObject*)m_pPassenger)->OnDamaged();

			m_pPassenger->m_pVehicle = this;
		}
	}
	else
	{
		OnGetOffPassenger();
	}
}

void ClownCar::OnBounce()
{
	m_nBounceCooltime = 8;

	SOUND_MANAGER->PlaySoundEffect(L"HitBlock", this);
}

void ClownCar::OnReachWall(bool bIsLeftWall)
{
	GameObjectBase::OnReachWall(bIsLeftWall);
}

void ClownCar::OnTrampolinJump()
{
	if (CheckHitBlock(1))
		return;

	if (m_fYS >= -2)
		ChangeState(STATE_FLY);

	OnFall();
}

void ClownCar::OnControlBy(GameObjectBase *pRider)
{
	if (m_nState == STATE_FLY)
	{
		if (pRider->m_pController && m_nStateFrame % 100 == 99)
		{
			if (m_fX < CUR_PLAYER_OBJ->m_fX)
			{
				pRider->m_pController->UnholdInput(INPUT_L);
				pRider->m_pController->HoldInput(INPUT_R);
			}
			if (m_fX > CUR_PLAYER_OBJ->m_fX)
			{
				pRider->m_pController->UnholdInput(INPUT_R);
				pRider->m_pController->HoldInput(INPUT_L);
			}
			if (m_fY < CUR_PLAYER_OBJ->m_fY)
			{
				pRider->m_pController->UnholdInput(INPUT_U);
				pRider->m_pController->HoldInput(INPUT_D);
			}
			if (m_fY > CUR_PLAYER_OBJ->m_fY)
			{
				pRider->m_pController->UnholdInput(INPUT_D);
				pRider->m_pController->HoldInput(INPUT_U);
			}

		}		
	}
}

bool ClownCar::CanPressEnemy()
{
	if (m_pPassenger)
	{
		return true;
	}
	return false;
}

void ClownCar::Render(int nColor, int nZOrder)
{
	float _x, _y;

	NaPointT<float> ptCamera = CAMERA->GetPosition();
	_x = x - ptCamera.x;
	_y = y - ptCamera.y;

	float fScaleX, fScaleY;
	GetScale(fScaleX, fScaleY);

	if (nZOrder == -1)
		nZOrder = GetZOrder();
	bool bShadow = (m_pGame->m_bModernStyle);

	int nBodyFrame = GetBodySpriteIndex();
	if (nBodyFrame != -1)
		(*m_ppSprite)->RenderToQueue(_x, _y, nBodyFrame, m_bFlip, m_bFlipV, -1L, fScaleX, fScaleY, nZOrder, bShadow);

	int nHeadFrame = GetWingSpriteIndex();
	if (nHeadFrame != -1)
	{
		POINT ptOffset = GetWingSpriteOffset();
		_x += ptOffset.x;
		_y += ptOffset.y;
		(*m_ppSprite)->RenderToQueue(_x, _y, nHeadFrame, m_bFlip, m_bFlipV, -1L, fScaleX, fScaleY, nZOrder + 1, bShadow);
	}

#if !defined(NDEBUG)
// 	{
// 		NaString str;
// 		str.Format(L"Ovl.Enemy: %d", m_vecOverlappedEnemy.size());
// 		RENDER_MANAGER->Text(_x, _y, str);
// 	}
#endif
}

int ClownCar::GetSpriteIndex()
{
	if (m_nState == STATE_ITEMPOP && m_nStateFrame < 30)
		return -1;

	return SPRIDX_VEHICLE_CLOWNCAR1;
}

int ClownCar::GetBodySpriteIndex()
{
	if (m_nDamageGroup != DAMAGEGROUP_ENEMY)
	{
		// #TODO ClownCar Body ..
		if (m_nStateFrame % 80 > 70)
			return SPRIDX_VEHICLE_CLOWNCAR2;

		return SPRIDX_VEHICLE_CLOWNCAR1;
	}
	else
	{
		if (m_nDamageCooltime > 70)
			return SPRIDX_VEHICLE_CLOWNCAR4;

		return SPRIDX_VEHICLE_CLOWNCAR3;
	}

	return SPRIDX_VEHICLE_CLOWNCAR1;
}

int ClownCar::GetWingSpriteIndex()
{
	if (m_fYS < 0)
	{
		return SPRITE_INDEX_CYCLE(SPRIDX_VEHICLE_CLOWNCAR_PROP1, 3, 2);
	}
	else
	{
		return SPRITE_INDEX_CYCLE(SPRIDX_VEHICLE_CLOWNCAR_PROP1, 3, 4);
	}

	return SPRIDX_VEHICLE_CLOWNCAR_PROP1;
}

POINT ClownCar::GetWingSpriteOffset()
{
	POINT pt = { 0, 4 };
	if (m_bFlip)
		pt.x *= -1;

	return pt;
}

void ClownCar::GetScale(float & fScaleX, float & fScaleY)
{
	fScaleX = m_fScaleX;
	fScaleY = m_fScaleY;

	if (m_nBounceCooltime > 0)
	{
		if (m_nBounceCooltime >= 4)
		{
			float fFactor = 1.0f + ((8 - m_nBounceCooltime) / 20.0f);
			fScaleX = m_fScaleX * fFactor;
			fScaleY = m_fScaleY / fFactor;
		}
		else if (m_nBounceCooltime < 4)
		{
			float fFactor = 1.0f + (m_nBounceCooltime / 20.0f);
			fScaleX = m_fScaleX * fFactor;
			fScaleY = m_fScaleY / fFactor;
		}
	}
}

NaRect ClownCar::GetRect()
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

SIZE ClownCar::GetSize()
{
	SIZE s;
	s.cx = 32 * m_fScaleX;
	s.cy = 32 * m_fScaleY;
	return s;
}
