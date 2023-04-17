#include "GoombaShoe.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "FModManager.h"
#include "Stage.h"
#include "SoundManager.h"

#include "TypeNames.h"

BEGIN_IMPL_PROPERTYMAP(GoombaShoe)
	PROP_BOOL("PowerUp",	VT_BOOL,	false,	nullptr,		"GoombaShoe"),
	PROP_BOOL("Winged",		VT_BOOL,	false,	nullptr,		"GoombaShoe"),
	PROP_INT("ShoeType",	VT_I4,		0,		g_szShoeTypes,	"GoombaShoe"),
END_IMPL_PROPERTYMAP()

GoombaShoe::GoombaShoe(Game *pGame, Stage *pStage) :
	VehicleObjectBase(pGame, pStage)
{
	m_nType = VEHICLE_GOOMBASHOE;
	m_nState = STATE_STAND;

	m_nRideType = STATE_CROUCH;

	m_bPowerUp = false;

	m_nBounceCooltime = 0;
}

GoombaShoe::~GoombaShoe()
{
}

void GoombaShoe::ChangeState(int nState, ChangeStateInfo * pInfo)
{
	switch (nState)
	{
	case STATE_STAND:
		{
			if (m_nDamageGroup == DAMAGEGROUP_ENEMY)
				m_fXS = 0;
		}
		break;
	case STATE_BURNT:
		{
			ReleaseWing();

			if (m_pGame->m_bModernStyle)
				m_bFlipV = false;
			else
				m_bFlipV = true;

			m_fYS = -3.0f;
			
			if (m_bFlip)
				m_fXS = 2.0f;
			else
				m_fXS = -2.0f;

			SOUND_MANAGER->PlaySoundEffect(L"Kicked");
		}
		break;
	default:
		VehicleObjectBase::ChangeState(nState);
		return;
		break;
	}

	m_nNextState = nState;

	if (pInfo)
	{
		m_ChangeStateInfo = *pInfo;
		m_ChangeStateInfo.nNextState = nState;
	}
	else
	{
		m_ChangeStateInfo.nNextState = -1;
	}
}

void GoombaShoe::OnChangeState(int nState)
{
	ChangeStateInfo *pInfo = nullptr;
	if (m_ChangeStateInfo.nNextState != -1)
		pInfo = &m_ChangeStateInfo;

	switch (nState)
	{
	case STATE_JUMP:
		if (m_nDamageGroup == DAMAGEGROUP_ENEMY)
		{
			if (m_bFlip)
				m_fXS = -2.5f;
			else
				m_fXS = 2.5f;
			m_fYS = -5.0f;
		}
		else
		{
			VehicleObjectBase::OnChangeState(nState);
			return;
		}
		break;
	case STATE_GROUNDPOUND:
		m_fXS = 0;
		m_fYS = -3.0f;
		break;
	default:
		VehicleObjectBase::OnChangeState(nState);
		break;
	}
}

void GoombaShoe::OnBeforeProcess()
{
	VehicleObjectBase::OnBeforeProcess();

	BEGIN_VECTOR_LOOP_NOINC(GameObjectBase*, m_vecOverlappedEnemy) {
		switch (pObj->m_nType)
		{
		case ENEMY_MUNCHER:
			it = vecCurrent.erase(it);
			break;
		default:
			it++;
			break;
		}
	} END_VECTOR_LOOP();
}

void GoombaShoe::OnAfterProcess()
{
	m_fX += m_fXS;
	m_fY += m_fYS;

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

	if (IsAlive() && m_pPassenger)
	{
		PositionPassenger();
	}

    ProcessState();
}

void GoombaShoe::OnStand()
{
	if (m_pPassenger)
	{
		ControllableObject::OnStand();
	}
	else
	{
		GameObjectBase::OnStand();
	}
}

void GoombaShoe::OnWalk()
{
	if (m_pPassenger)
	{
		//ControllableObject::OnWalk();
		bool bSwim = CanSwim();
		bool bBoost = bSwim ? false : IsInputHold(INPUT_F);
		bool bFront = m_bFlip ? IsInputHold(INPUT_L) : IsInputHold(INPUT_R);
		bool bBack = m_bFlip ? IsInputHold(INPUT_R) : IsInputHold(INPUT_L);
		float fMark = m_bFlip ? (-1) : 1;

		float fWalkSpeed = m_PhysicalProps.fAccelWalk; // 0.05f;
		float fMaxSpeed = m_PhysicalProps.fMaxWalkSpeed; // 1.5f;
		if (bSwim)
		{
			if (m_nShape != SHAPE_SHELL)
				fMaxSpeed = m_PhysicalProps.fMaxWalkSpeed / 1.5f; // 1.0f;
			else
				fMaxSpeed = m_PhysicalProps.fMaxWalkSpeed; // 1.5f;
		}

		if (bBoost)
		{
			fWalkSpeed = m_PhysicalProps.fAccelRun; // 0.1f;
			if (m_pGame->m_nStagePackType != STAGEPACK_CLASSIC)
			{
				if (m_nInvinsibleTime > 0)
					fMaxSpeed = m_PhysicalProps.fMaxRunSpeedInvinsible; // 4.0f;
				else
					fMaxSpeed = m_PhysicalProps.fMaxRunSpeedNonClassic; // 3.5f;
			}
			else
				fMaxSpeed = m_PhysicalProps.fMaxRunSpeedClassic; // 3.0f;
		}

		if (bFront && !bBack)
		{
			// Normal Move
			if (m_fXS * fMark < fMaxSpeed)
				m_fXS += (fMark) * (fWalkSpeed);

			// StopPress AccelKey -> SlowDown
			if (!bBoost && (m_fXS * fMark > fMaxSpeed))
				m_fXS += (-fMark) * (fWalkSpeed);

			if (!m_bFlip && m_bCollision[COLLISION_RIGHT])
			{
				PipeInfo *pPipeInfo = FindPipeNearMe(COLLISION_RIGHT);
				if (pPipeInfo)
				{
					EnterPipe(pPipeInfo);
					return;
				}
			}
			else if (m_bFlip && m_bCollision[COLLISION_LEFT])
			{
				PipeInfo *pPipeInfo = FindPipeNearMe(COLLISION_LEFT);
				if (pPipeInfo)
				{
					EnterPipe(pPipeInfo);
					return;
				}
			}
		}
		else
		{
			// StopPress MoveKey -> SlowDown
			// 1.2f: to make fix slippery move
			m_fXS += (-fMark) * (m_PhysicalProps.fDecelWalk);

			if (m_bFlip ? (m_fXS > 0) : (m_fXS < 0))
			{
				m_fXS = 0;
				ChangeState(STATE_STAND);
			}
		}

		if (bBack && !bFront)
		{
			m_bFlip = !m_bFlip;
			ChangeState(STATE_BREAK);
		}

		if (IsInputHold(INPUT_D) && m_bCollision[COLLISION_BOTTOM])
		{
			PipeInfo *pPipeUnderMe = FindPipeNearMe(COLLISION_BOTTOM);
			if (pPipeUnderMe)
			{
				EnterPipe(pPipeUnderMe);
				return;
			}
			else if (CanSit())
				ChangeState(STATE_SIT);
		}
		else if (IsInputPressed(INPUT_U))
		{
			GameObjectBase *pDoor = FindDoorNearMe();
			if (pDoor)
			{
				EnterDoor(pDoor);
				return;
			}
		}

		// Walking Jump
		if (CheckJump())
		{
			;
		}
		else if (CheckPressEnemy())
		{
			ChangeState(STATE_PRESSJUMP);
		}

		// Fall Down
		//GameObjectBase::OnWalk();
		{
			auto size = GetSize();
			//if (CheckFalling())
			if (m_bStandOnJumpBlock)
			{
				ChangeState(STATE_BLOCKJUMP);
				return;
			}
			else if (!m_bCollision[COLLISION_BOTTOM])
			{
				OnApplyGravity(1.0f);
			}
			else if (m_fY > m_ptCollision[COLLISION_BOTTOM].y)
			{
				m_fY = m_ptCollision[COLLISION_BOTTOM].y;

				if (m_fYS > 0)
				{
					m_fYS = 0;
					if (m_nShoeType == 1)
					{
						NaString strSound;
						strSound.Format(L"ShoeStiletto%d", 1 + rand() % 3);
						SOUND_MANAGER->PlaySoundEffect(strSound, this);
					}
				}
			}

			if (m_bReachFriend)
				OnReachFriend();
			else
				CheckReachWall();
		}

		// Process WalkFrame
		int nFriction;
		if (abs(m_fXS) < 1.5f)
			nFriction = 5;
		else
			nFriction = MAX(2, (5 - abs(m_fXS)));

		if (m_nShape == SHAPE_GIANT)
			nFriction *= 3;

		if (m_nStateFrame % (nFriction) == 0 && m_nNextShape == -1)
			m_nWalkFrame++;
	}
	else
	{
		GameObjectBase::OnWalk();
	}

	if (m_bCollision[COLLISION_BOTTOM])
	{
		if (m_nStateFrame % 10 == 9)
			m_fYS = -2.5f;
	}
	
	if (IsInputPressed(INPUT_J))
	{
		if (!IsInputHold(INPUT_U))
			ChangeState(STATE_JUMP);
	}
}

void GoombaShoe::OnJump()
{
	if (m_bCollision[COLLISION_TOP] && IsInputHold(INPUT_U))
	{
		PipeInfo *pPipeInfo = FindPipeNearMe(COLLISION_TOP);
		if (pPipeInfo)
		{
			EnterPipe(pPipeInfo);
			return;
		}
	}

	int nPower = GetPower(POWERTYPE_HITBLOCK);
	if (CheckHitBlock(nPower))
		return;

	if (m_pPassenger == nullptr)
	{
		ChangeState(STATE_FALL);
		return;
	}

	OnAir();

	if (m_bPowerUp && IsInputPressed(INPUT_D))
	{
		ChangeState(STATE_GROUNDPOUND);
		return;
	}

	if (m_fYS >= 0 || m_nStateFrame >= 24)
	{
		if (CheckPressEnemy())
		{
			ChangeState(STATE_PRESSJUMP);
			return;
		}
	}

	if (m_fYS <= 0)
	{
		if (m_nJumpHold > 0)
		{
			if (IsInputHold(INPUT_J))
			{
				// more higher
				m_nJumpHold--;
			}
			else
			{
				m_fYS = -1.0f;
				m_nJumpHold = -27;
			}
		}

		int nJumpFrame = m_nStateFrame;
		if (m_nJumpHold < 0)
			nJumpFrame = m_nStateFrame - m_nJumpHold;

		switch (nJumpFrame)
		{
		case 5:  // 4 -> 3
		case 12: // 3 -> 2
		case 21: // 2 -> 1
		case 28: // 1 -> 0
			m_fYS += 1.0f;
			break;
		}

		if (nJumpFrame > 33)
		{
			ChangeState(STATE_FALL);
		}
	}
}

void GoombaShoe::OnFall()
{
	SIZE size = GetSize();
	if (m_fYS < 0)
	{
		if (m_bCollision[COLLISION_TOP])
		{
			// Hit the block
			
			m_fYS = 0;
			m_fY = ((m_ptCollision[COLLISION_TOP].y + 1)) + size.cy;

			return;
		}
	}

	if (CanSwim())
	{
		ChangeState(STATE_SWIM);

		CreateParameter param;
		param.fScaleX = m_fScaleX;
		param.fScaleY = m_fScaleY;
		CUR_STAGE->CreateEffect(m_fX, m_fY, EFFECT_SPLASH, &param);
		return;
	}

	if (CheckPressEnemy())
	{
		ChangeState(STATE_PRESSJUMP);
	}
	else if (!m_bCollision[COLLISION_BOTTOM])
	{
		OnApplyGravity(1, 4.0f);
	}
	else if ((int)m_fY - (size.cy / 4) <= m_ptCollision[COLLISION_BOTTOM].y)
	{
		m_fYS = 0;
		m_fY = m_ptCollision[COLLISION_BOTTOM].y;

		StopCombo();

		POINT ptCollidableOffset = FindCollidableOffset(COLLISION_TOP, 8);
		if (m_bStandOnJumpBlock)
			ChangeState(STATE_BLOCKJUMP);
		else if (m_fXS != 0)
		{
			if (m_nDamageGroup == DAMAGEGROUP_ENEMY)
				ChangeState(STATE_STAND); 
			else
				ChangeState(STATE_WALK);
		}
		else if (m_fXS == 0)
			ChangeState(STATE_STAND);
	}
	else if (CanSwim())
	{
		ChangeState(STATE_SWIM);
	}

	if (m_bWinged)
	{
		if (m_fYS >= 1.5f && IsInputHold(INPUT_J))
		{
			ChangeState(STATE_FLY);
			return;
		}
	}

	if (m_bPowerUp && IsInputPressed(INPUT_D))
	{
		ChangeState(STATE_GROUNDPOUND);
		return;
	}

	if (m_nNextState == -1)
		OnAir();
}

void GoombaShoe::OnFly()
{
	bool bFront = m_bFlip ? IsInputHold(INPUT_L) : IsInputHold(INPUT_R);
	bool bBack = m_bFlip ? IsInputHold(INPUT_R) : IsInputHold(INPUT_L);
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
	
	if (IsInputHold(INPUT_J) && m_nStateFrame < 70)
	{
		if (m_fYS > -2.0f)
			m_fYS -= 0.07f;
	}
	else
	{
		OnApplyGravity(0.5f, 4.0f);
	}

	if (m_fYS > 0 && m_bCollision[COLLISION_BOTTOM])
	{
		m_fYS = 0;
		m_fY = m_ptCollision[COLLISION_BOTTOM].y;

		OnLanding();
		return;
	}

	if (m_bPowerUp && IsInputPressed(INPUT_D))
	{
		ChangeState(STATE_GROUNDPOUND);
		return;
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

	if (CheckPressEnemy(POWER_PRESS_GOOMBASHOES))
	{
		if (!m_bCollision[COLLISION_TOP])
			m_fYS = -1.0f;
	}
}

void GoombaShoe::OnGroundPound()
{
	if (m_fYS > 1.0f)
		SmashEnemies();

	if (!m_bCollision[COLLISION_BOTTOM])
	{
		if (m_nStateFrame < 16)
			OnApplyGravity(2.5f, 5.0f);
		else
			OnApplyGravity(2.5f, 5.0f);
	}
	else
	{
		if ((int)m_fY <= m_ptCollision[COLLISION_BOTTOM].y)
		{
			m_fYS = 0;
			//OnLanding();
			{
				m_fY = m_ptCollision[COLLISION_BOTTOM].y;

				if (m_bStandOnJumpBlock)
					ChangeState(STATE_BLOCKJUMP);
				else
					ChangeState(STATE_STAND);

				SmashAll();
				m_nBounceCooltime = 8;

				// Create ShoeSmoke.
				// Initial speed = 4
				auto s = GetSize();

				CreateParameter param;
				param.pFromObject = this;
				param.fXS = -4.0f;
				param.nFlip = 1; 
				CUR_STAGE->CreateEventObject(m_fX - (s.cx / 1.5f), m_fY, EVENTOBJECT_SHOESMOKE, &param);

				param.fXS = 4.0f;				
				CUR_STAGE->CreateEventObject(m_fX + (s.cx / 1.5f), m_fY, EVENTOBJECT_SHOESMOKE, &param);
			}
		}
	}
}

int GoombaShoe::SmashAll()
{
	int nTX = m_fX / TILE_XS;
	int nTY = (m_fY + m_fYS + 2) / TILE_YS;

	int nRet = TILETYPE_EMPTY;

	// Destroy Blocks
	SIZE s = GetSize();
	int nLeftTX = (m_fX - (s.cx / 2)) / TILE_XS;
	int nRightTX = (m_fX + (s.cx / 2)) / TILE_XS;

	for (int _x = nLeftTX; _x <= nRightTX; _x++)
	{
		int _y = nTY;
		if (!CUR_STAGE->IsHardTile(_x, _y))
			continue;

		int nData = CUR_STAGE->GetTileData(_x, _y);
		int nType = CUR_STAGE->GetDataType(nData);
		if (m_nShoeType == 0)
		{
			if (nType == TILETYPE_SOFTHARD || nType == TILETYPE_ITEM)
			{
				CUR_STAGE->PressBlock(_x, _y, POWER_HITBLOCK_NORMAL, this);
			}
		}
		else if (m_nShoeType == 1)
		{
			if (nType == TILETYPE_ITEM)
			{
				CUR_STAGE->PressBlock(_x, _y, POWER_HITBLOCK_NORMAL, this);
			}
			else if (CUR_STAGE->CanCrush(_x, _y, POWER_HITBLOCK_BIGMARIO))
			{
				CUR_STAGE->CrushBlock(_x, _y, this);
			}
		}

		if (nType == TILETYPE_JUMP)
		{
			CUR_STAGE->PressJumpBlock(_x, _y);
		}
	}

	SOUND_MANAGER->PlaySoundEffect(L"Cannon");

	return nRet;
}

void GoombaShoe::SmashEnemies()
{
	NaRect rc = GetRect();
	rc.top += rc.Height() / 2;
	rc.bottom += 2;

	// Destroy Enemies
	BEGIN_VECTOR_LOOP(GameObjectBase*, CUR_STAGE->m_vecEnemy) {
		NaRect rcObj = pObj->GetRect();
		if (pObj == this)
			continue;
		if (!rc.IsOverlapped(rcObj))
			continue;

		if (pObj->IsAlive() && pObj->CanPressed(POWER_PRESS_GOOMBASHOES))
		{
			ChangeStateInfo info;
			info.nNextState = STATE_PRESSED;
			info.pFromObject = this;
			info.nPower = POWER_PRESS_GOOMBASHOES;
			pObj->ChangeState(STATE_PRESSED, &info);
		}
	} END_VECTOR_LOOP();

	// Destroy Player
	NaRect rcPlayer = CUR_PLAYER_OBJ->GetRect();
	if (rc.IsOverlapped(rcPlayer))
	{
		GameObjectBase *pObj = CUR_PLAYER_OBJ;
		if (pObj->IsAlive() && pObj->CanPressed(POWER_PRESS_GOOMBASHOES))
		{
			ChangeStateInfo info;
			info.nNextState = STATE_PRESSED;
			info.pFromObject = this;
			info.nPower = POWER_PRESS_GOOMBASHOES;
			pObj->ChangeState(STATE_PRESSED, &info);
		}
	}
}

void GoombaShoe::OnItemPop()
{
	if (m_nStateFrame == 30)
		m_fYS = -1.6f;

	if (m_nStateFrame == 42)
	{
		SetState(STATE_FLY);
		m_fYS = -1.0f;
	}
}

void GoombaShoe::OnItemPopDown()
{
	if (m_nStateFrame == 4)
		m_fYS = 2.0f;

	if (m_nStateFrame == 4 + 4)
	{
		SetState(STATE_FLY);
		m_fYS = 1.0f; 
	}
}

void GoombaShoe::OnPowerUp()
{
	if (m_bPowerUp)
	{
		m_fScaleX = m_fScaleY = 2.0f;
	}
	else
	{
		m_fScaleX = m_fScaleY = 1.0f;
	}
}

void GoombaShoe::OnBurnt()
{
	switch (m_nStateFrame)
	{
	case 2:
	case 7:
	case 11:
	case 15:
	case 19:
	case 23:
		m_fYS += 1.0f;
		break;
	}

	if (IsOutOfCamera())
		ChangeState(STATE_DEAD);
}

void GoombaShoe::OnControlBy(GameObjectBase *pRider)
{
	if (m_nState == STATE_STAND)
	{
		if (pRider->m_pController)
		{
			if (m_nStateFrame == 0)
				pRider->m_pController->UnholdInputAll();

			if (m_nStateFrame == 24)
			{
				if (rand() % 2 == 0)
					pRider->m_bFlip = false;
				else
					pRider->m_bFlip = true;
				m_bFlip = pRider->m_bFlip;
			}

			if (m_nStateFrame % 100 == 99)
			{
				ChangeState(STATE_JUMP);
			}
		}
	}

	if (m_nState == STATE_JUMP)
	{
		if (pRider->m_pController)
		{
			if (m_nStateFrame == 10)
				pRider->m_pController->HoldInput(INPUT_J);
			
			if (m_fYS >= -1 && m_bPowerUp)
			{
				//pRider->m_pController->HoldInput(INPUT_D);
				ChangeState(STATE_GROUNDPOUND);
			}
		}
	}
}

void GoombaShoe::PositionPassenger()
{
	if (m_pPassenger == nullptr)
		return;

	SIZE s = GetSize();
	float fScaleX, fScaleY;
	GetScale(fScaleX, fScaleY);

	m_pPassenger->m_fX = m_fX;
	m_pPassenger->m_fY = m_fY - ((16 * fScaleY) - 6);
	m_pPassenger->m_bFlip = m_bFlip;
}

void GoombaShoe::OnRidePassenger(GameObjectBase * pPassenger)
{
	VehicleObjectBase::OnRidePassenger(pPassenger);

	if (!m_bCollision[COLLISION_BOTTOM])
		m_fYS = 0.5f;
}

void GoombaShoe::OnGetOffPassenger()
{
	m_fXS = 0;
	m_fYS = 0;

	VehicleObjectBase::OnGetOffPassenger();
}

void GoombaShoe::OnDamagedPassenger()
{
	if (m_pPassenger && m_pPassenger->m_nDamageGroup != DAMAGEGROUP_ENEMY)
	{
		m_pPassenger->m_nDamageCooltime = GameDefaults::nDamageCooltime;
		m_pPassenger->ChangeState(STATE_GOTOFFVEHICLE);

		OnGetOffPassenger();

		ChangeState(STATE_BURNT);
	}
	else
	{
		OnGetOffPassenger();
	}
}

void GoombaShoe::OnReachWall(bool bIsLeftWall)
{
	switch (m_nState)
	{
	case STATE_JUMP:
	case STATE_FALL:
		if (m_nDamageGroup == DAMAGEGROUP_ENEMY)
		{
			GameObjectBase::OnReachWall(bIsLeftWall);
		}
		else
			VehicleObjectBase::OnReachWall(bIsLeftWall);
		break;
	default:
		VehicleObjectBase::OnReachWall(bIsLeftWall);
		break;
	}
}

bool GoombaShoe::IsAlive()
{
	switch (m_nState)
	{
	case STATE_BURNT:
		return false;
		break;
	default:
		return VehicleObjectBase::IsAlive();
		break;
	}
	return false;
}

bool GoombaShoe::CanPressEnemy()
{
	if (m_pPassenger)
	{
		return true;
	}
	return false;
}

bool GoombaShoe::CheckFalling()
{
	if (m_bStandOnJumpBlock)
	{
		ChangeState(STATE_BLOCKJUMP);
		return true;
	}
	else if (!m_bCollision[COLLISION_BOTTOM])
	{
		OnApplyGravity(1.0f, 4.0f);
		
		if (m_fYS >= 0)
		{
			ChangeState(STATE_FALL);
			return true;
		}
	}
	else if (m_fY > m_ptCollision[COLLISION_BOTTOM].y)
	{
		m_fY = m_ptCollision[COLLISION_BOTTOM].y;
	}

	return false;
}

bool GoombaShoe::CanRided(GameObjectBase * pByWho)
{
	switch (m_nState)
	{
	case STATE_PIPEUP:
		return false;
		break;
	}

	if (pByWho->m_nDamageGroup == DAMAGEGROUP_PLAYER)
		return true;
	else
	{
		switch (pByWho->m_nType)
		{
		case ENEMY_GOOMBA:
			return true;
			break;
		}
	}

	return false;
}

int GoombaShoe::GetPower(int nPowerForWhat)
{
	switch (nPowerForWhat)
	{
	case POWERTYPE_HITBLOCK:
		if (m_pPassenger)
		{
			return ((ControllableObject*)m_pPassenger)->GetPower(nPowerForWhat);
		}
		return POWER_HITBLOCK_BIGMARIO;
		break;
	case POWERTYPE_PRESS:
		return POWER_PRESS_GOOMBASHOES;
		break;
	}

	return 0;
}

int GoombaShoe::GetSpriteIndex()
{
	if (m_nState == STATE_ITEMPOP && m_nStateFrame < 30)
		return -1;

	switch (m_nShoeType)
	{
	case 0:
		return SPRITE_INDEX_CYCLE(SPRIDX_VEHICLE_GOOMBASHOE1, 2, 8);
		break;
	case 1:
		return SPRIDX_VEHICLE_STILETTO;
		break;
	}

	return SPRITE_INDEX_CYCLE(SPRIDX_VEHICLE_GOOMBASHOE1, 2, 8);
}

SIZE GoombaShoe::GetSpriteSize()
{
	SIZE s = { 16, 16 };
	int nIndex = GetSpriteIndex();
	if (nIndex != -1 && (*m_ppSprite) != nullptr)
	{
		RECT rc = (*m_ppSprite)->m_vecFrames[nIndex].m_rc;
		s.cx = (rc.right - rc.left) * m_fScaleX;
		s.cy = (rc.bottom - rc.top) * m_fScaleY;
	}

	return s;
}

std::vector<POINT> GoombaShoe::GetWingPosition()
{
	POINT pt;
	SIZE s = GetSize();
	pt.x = -4;
	pt.y = -4;

	std::vector<POINT> vec;
	vec.push_back(pt);

	return vec;
}

int GoombaShoe::GetWingSpriteIndex()
{
	int nFrame = SPRIDX_WING1;
	if (m_fYS != 0)
	{
		if (m_nState == STATE_FLY && m_fYS < 0)
		{
			if (m_nStateFrame % 8 >= 4)
				nFrame = SPRIDX_WING2;
		}
		else if (m_nStateFrame % 16 >= 8)
			nFrame = SPRIDX_WING2;
	}
	else
	{
		if (m_nStateFrame % 32 >= 16)
			nFrame = SPRIDX_WING2;
	}
	
	return nFrame;
}

void GoombaShoe::GetScale(float & fScaleX, float & fScaleY)
{
	fScaleX = m_fScaleX;
	fScaleY = m_fScaleY;

	if (m_nState == STATE_ITEMPOP && m_nStateFrame < 40)
	{
		float fFactorX = 1.0f, fFactorY = 1.0f;
		SIZE s = GetSpriteSize();
		if (s.cx > 16)
			fFactorX = (16.0f / (float)s.cx);

		if (s.cy > 16)
			fFactorY = (16.0f / (float)s.cy);

		float fFactor = fFactorX < fFactorY ? fFactorX : fFactorY;
		if (m_nStateFrame >= 36 && m_nStateFrame < 40)
		{
			// factor = 0.5

			// 0.5 === 1
			// 0.6 === f * 6/5
			// 0.7 === f * 7/5
			// ..
			// 1
			int nFrame = m_nStateFrame - 35;
			int nMaxFrame = 4;
			fFactor = fFactor * ((float)(nFrame + nMaxFrame) / (float)nMaxFrame);
			if (fFactor > 1)
				fFactor = 1.0f;
		}

		fScaleX *= fFactor;
		fScaleY *= fFactor;
	}

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

NaRect GoombaShoe::GetRect()
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
