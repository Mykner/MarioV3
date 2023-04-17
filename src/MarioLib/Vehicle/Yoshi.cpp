#include "Yoshi.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "FModManager.h"
#include "Stage.h"
#include "YoshiTongue.h"
#include "SoundManager.h"

#include "EnemyObjectBase.h"

BEGIN_IMPL_PROPERTYMAP(Yoshi)
	PROP_BOOL("Hatched", VT_BOOL, false, nullptr, "Yoshi"),
END_IMPL_PROPERTYMAP()

Yoshi::Yoshi(Game *pGame, Stage *pStage) :
	VehicleObjectBase(pGame, pStage)
{
	m_nType = VEHICLE_YOSHI;
	m_nState = STATE_HATCH;

	m_nRideType = STATE_RIDE;

	m_nShape = YOSHISHAPE_EGG;
	m_nWalkFrame = 0;
	m_nJumpHold = 0;

	m_nKeepInMouthType = 0;
	m_nKeepInMouthCooltime = 0;
	m_nGulpCooltime = 0;
	m_nBodyFrame = 0;
	m_nHeadFrame = -1;

	m_bHatched = false;
	m_nHatchFrame = 0;
}

Yoshi::~Yoshi()
{
}

void Yoshi::CheckCollisionEnemy()
{
	if (!IsAlive())
		return;

	m_fX += m_fXS;
	m_fY += m_fYS;

	NaRect rcMe = GetRect();

	m_vecPressedEnemy.clear();
	m_vecOverlappedEnemy.clear();
	std::vector<GameObjectBase*>::iterator it = CUR_STAGE->m_vecEnemy.begin();
	for (; it != CUR_STAGE->m_vecEnemy.end(); ++it)
	{
		EnemyObjectBase *pEnemy = (EnemyObjectBase*)*it;
		if ((GameObjectBase*)pEnemy == this)
			continue;
		if (!pEnemy->IsAlive())
			continue;
		if (pEnemy->m_nState == STATE_IDLE && pEnemy->m_nStateFrame == 0)
			continue;
		if (abs(m_fX - pEnemy->m_fX) > GameDefaults::nPageWidth)
			continue;

		if (IsOverlapped(pEnemy))
		{
			NaRect rcEnemy = pEnemy->GetRect();
			if (!IsOpponent(pEnemy))
			{
				if (CanDamageFriend())
				{
					m_vecOverlappedEnemy.push_back(pEnemy);
				}
				else if ((m_fXS > 0 && m_fX < pEnemy->m_fX) || (m_fXS < 0 && m_fX > pEnemy->m_fX))
				{
					m_bReachFriend = true;
				}
			}
			else
			{
				if (m_nInvinsibleTime > 0)
				{
					m_vecOverlappedEnemy.push_back(pEnemy);
				}
				else if (CanPressEnemy() &&
					pEnemy->CanPressed(POWER_PRESS_YOSHI) &&
					m_fY - rcEnemy.top < rcEnemy.Height() / 2)
				{
					m_vecPressedEnemy.push_back(pEnemy);
				}
				else
				{
					m_vecOverlappedEnemy.push_back(pEnemy);
				}
			}
		}
	}

	m_fX -= m_fXS;
	m_fY -= m_fYS;
}

void Yoshi::ChangeState(int nState, ChangeStateInfo * pInfo)
{
	switch (nState)
	{
	case STATE_WALK:
		if (m_pPassenger == nullptr)
		{
			if (m_bFlip)
				m_fXS = -1.0f;
			else
				m_fXS = 1.0f;
		}
		m_nNextState = nState;
		return;
		break;
	default:
		VehicleObjectBase::ChangeState(nState, pInfo);
		break;
	}
}

void Yoshi::OnAfterProcess()
{
	m_fX += m_fXS;
	m_fY += m_fYS;

	if (m_nDamageCooltime > 0)
	{
		m_nDamageCooltime--;
	}
	
	if (m_nThrowFireFrame > 0)
	{
		m_nThrowFireFrame--;
	}

	if (m_nKeepInMouthCooltime > 0)
	{
		m_nKeepInMouthCooltime--;

		if (m_nKeepInMouthCooltime == 0)
			OnSwallow();
	}

	if (m_nGulpCooltime > 0)
	{
		m_nGulpCooltime--;
	}

	if (!m_bHatched)
	{
		ProcessHatch();
	}

	if (IsAlive() && m_pPassenger)
	{
		PositionPassenger();
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

void Yoshi::ProcessHatch()
{
	if (m_bHatched)
		return;

	if (m_nState == STATE_ITEMPOP ||
		m_nState == STATE_ITEMPOPDOWN)
		return;

	if (m_nShape == YOSHISHAPE_EGG && m_nHatchFrame >= 28)
	{
		m_nShape = YOSHISHAPE_BEGINHATCH;

		// Effect
		CreateParameter param;
		param.fXS = -0.5f;
		param.fYS = -3.0f;
		param.nType = 0;
		CUR_STAGE->CreateEffect(m_fX, m_fY, EFFECT_EGGDEBRIS, &param);
		param.fXS = 0.5f;
		param.fYS = -3.0f;
		param.nType = 1;
		CUR_STAGE->CreateEffect(m_fX, m_fY, EFFECT_EGGDEBRIS, &param);
		param.fXS = -0.5f;
		param.fYS = -2.0f;
		param.nType = 2;
		CUR_STAGE->CreateEffect(m_fX, m_fY, EFFECT_EGGDEBRIS, &param);
		param.fXS = 0.5f;
		param.fYS = -2.0f;
		param.nType = 3;
		CUR_STAGE->CreateEffect(m_fX, m_fY, EFFECT_EGGDEBRIS, &param);

		SOUND_MANAGER->PlaySoundEffect(L"YoshiEggBreak");
	}

	if (m_nShape == YOSHISHAPE_BEGINHATCH && m_nHatchFrame >= 80)
	{
		m_bHatched = true;
		m_nShape = YOSHISHAPE_HATCHED;
		ChangeState(STATE_IDLE);
	}

	m_nHatchFrame++;
}

void Yoshi::OnHatch()
{
	if (m_bHatched)
	{
		ChangeState(STATE_IDLE);
		return;
	}

	if (CheckFalling())
		return;	
}

void Yoshi::OnIdle()
{
	if (m_bCollision[COLLISION_BOTTOM])
	{
		if (!CheckFalling())
		{
			if (m_pPassenger == nullptr)
			{
				m_fYS = -1.5f;
			}
			else
				m_fYS = 0;
		}
	}
	else
	{
		m_fYS += 0.3f;
	}
}

void Yoshi::PositionPassenger()
{
	if (m_pPassenger == nullptr)
		return;

	SIZE s = GetSize();

	float fOffsetX = -1;
	float fOffsetY = -13;

	int nFrame = GetBodySpriteIndex();
	switch (nFrame)
	{
	case SPRIDX_VEHICLE_YOSHI_BODY_WALK1:
	case SPRIDX_VEHICLE_YOSHI_BODY_WALK2:
		fOffsetY = -12;
		break;
	case SPRIDX_VEHICLE_YOSHI_BODY_SIT:
	case SPRIDX_VEHICLE_YOSHI_BODY_EAT:
		fOffsetY = -8;
		break;
	}

	switch (m_nState)
	{
	case STATE_PIPERIGHT:
		fOffsetY = -6;
		break;
	case STATE_PIPEDOWN:
	case STATE_PIPEUP:
		fOffsetY = -8;
		break;
	}

	if (m_bFlip)
		fOffsetX *= -1.0f;

	m_pPassenger->m_fX = m_fX + fOffsetX;
	m_pPassenger->m_fY = m_fY + fOffsetY;
	m_pPassenger->m_bFlip = m_bFlip;
}

void Yoshi::OnRidePassenger(GameObjectBase * pPassenger)
{
	VehicleObjectBase::OnRidePassenger(pPassenger);

	m_fXS = 0;
	m_fYS = 0;
	pPassenger->m_bFlip = m_bFlip;

	ChangeState(STATE_STAND);

	SOUND_MANAGER->PlaySoundEffect(L"YoshiRide");
}

void Yoshi::OnGetOffPassenger()
{
	m_fXS = 0;
	m_fYS = 0;

	VehicleObjectBase::OnGetOffPassenger();

	ChangeState(STATE_IDLE);
}

void Yoshi::OnDamagedPassenger()
{
	VehicleObjectBase::OnDamagedPassenger();

	m_nDamageCooltime = (GameDefaults::nDamageCooltime / 4);

	ChangeState(STATE_WALK);

	SOUND_MANAGER->PlaySoundEffect(L"YoshiDamaged");
}

void Yoshi::OnStand()
{
	if (m_pPassenger == nullptr)
	{
		if (!m_bHatched)
			ChangeState(STATE_HATCH);
		else
			ChangeState(STATE_IDLE);
	}
	else
		ControllableObject::OnStand();
}

void Yoshi::OnWalk()
{
	if (m_pPassenger)
	{
		ControllableObject::OnWalk();
	}
	else
	{
		//GameObjectBase::OnWalk();
		if (CheckFalling())
			return;

		CheckReachWall();
	}
}

void Yoshi::OnFall()
{
	VehicleObjectBase::OnFall();
}

void Yoshi::OnReachWall(bool bIsLeftWall)
{
	GameObjectBase::OnReachWall(bIsLeftWall);

	if (m_pPassenger == nullptr)
	{
		m_fXS *= -1;
		m_bFlip = !m_bFlip;
	}
	else
		m_fXS = 0;

}

void Yoshi::OnItemPop()
{
	if (m_nStateFrame == 30)
		m_fYS = -1.5f;

	if (m_nStateFrame == 42)
	{
		ChangeState(STATE_HATCH);
	}
}

void Yoshi::OnItemPopDown()
{
	if (m_nStateFrame == 4)
		m_fYS = 2.0f;

	if (m_nStateFrame == 4 + 4)
		ChangeState(STATE_HATCH);
}

void Yoshi::OnKeepInMouth(GameObjectBase *pObj)
{
	if (pObj->CanSpit())
	{
		pObj->ChangeState(STATE_DEAD);

		m_nKeepInMouthType = pObj->m_nType;
		m_nKeepInMouthCooltime = 360;
	}
	else
	{
		OnSwallow(pObj);
	
		pObj->ChangeState(STATE_DEAD);
	}
}

void Yoshi::OnSwallow(GameObjectBase *pObj)
{
	if (m_pPassenger && m_pPassenger->m_pController)
	{
		if (pObj && pObj->m_nType >= ITEM_BEGIN && pObj->m_nType <= ITEM_END)
		{
			m_pPassenger->OnApplyItem(pObj);
		}
		else
		{
			m_pPassenger->m_pController->AddScoreWithEffect(100, this);
		}
	}

	m_nKeepInMouthType = 0;
	m_nGulpCooltime = 20;

	SOUND_MANAGER->PlaySoundEffect(L"YoshiSwallow");
}

void Yoshi::OnDamaged()
{
	if (m_nInvinsibleTime > 0)
		return;

	if (m_pPassenger)
	{
		OnDamagedPassenger();
	}
}

void Yoshi::ConvertSpitType()
{
	switch (m_nKeepInMouthType)
	{
	case ENEMY_REDTURTLE:
	case ENEMY_REDTURTLE_WINGED_PATROL:
	case ENEMY_PODOBOO:
	case BULLET_FIREBALL:
	case BULLET_THROWINGHAMMER:
	case BULLET_BOWSERFIRE:	
		m_nKeepInMouthType = BULLET_YOSHIFIRE;
		break;
	}
}

bool Yoshi::CanFire()
{
	if (m_nKeepInMouthCooltime > 0)
		return true;

	if (m_nGulpCooltime > 0)
		return false;

	if (m_nState != STATE_HATCH)
		return true;

	return false;
}

void Yoshi::ThrowFireball()
{
	// Mouth Position
	int nOffsetX = 19;
	if (m_bFlip)
		nOffsetX *= -1;

	if (m_nKeepInMouthCooltime == 0)
	{
		int nCnt = 0;
		BEGIN_VECTOR_LOOP(GameObjectBase*, CUR_STAGE->m_vecBullet) {
			if (pObj->m_nType == BULLET_YOSHITONGUE)
			{
				if (((YoshiTongue*)pObj)->m_pOwner == this)
					nCnt++;
			}
		} END_VECTOR_LOOP();

		if (nCnt > 0)
			return;

		CreateParameter param;
		param.pFromPlayer = m_pController;
		param.pFromObject = this;
		param.nType = BULLET_YOSHITONGUE;
		CUR_STAGE->CreateBullet(m_fX + nOffsetX, m_fY - 10, BULLET_YOSHITONGUE, &param);

		m_nThrowFireFrame = 22;

		SOUND_MANAGER->PlaySoundEffect(L"YoshiTongue");
	}
	else
	{
		ConvertSpitType();

		float fX = m_fX + nOffsetX;
		float fY = m_fY - 10;

		GameObjectBase *pObj = nullptr;
		if (m_nKeepInMouthType >= ENEMY_BEGIN && m_nKeepInMouthType <= ENEMY_END)
			pObj = CUR_STAGE->AddEnemy(fX, fY, m_nKeepInMouthType);
		else if (m_nKeepInMouthType >= BULLET_BEGIN && m_nKeepInMouthType <= BULLET_END)
			pObj = CUR_STAGE->CreateBullet(fX, fY, m_nKeepInMouthType);
		if (pObj)
		{
			if (m_pPassenger)
				pObj->m_nDamageGroup = m_pPassenger->m_nDamageGroup;
			else
				pObj->m_nDamageGroup = m_nDamageGroup;
			pObj->SetState(STATE_HELDBYTONGUE);

			ChangeStateInfo info;
			info.pFromObject = this;
			if (IsInputHold(INPUT_D))
				pObj->ChangeState(STATE_SPITTED_SAFE, &info);
			else
				pObj->ChangeState(STATE_SPITTED, &info);
		}

		if (m_nKeepInMouthType == BULLET_YOSHIFIRE)
		{
			SOUND_MANAGER->PlaySoundEffect(L"YoshiFire");
		}
		else
		{
			SOUND_MANAGER->PlaySoundEffect(L"YoshiSpit");
		}

		m_nKeepInMouthCooltime = 0;
		m_nKeepInMouthType = 0;
		m_nThrowFireFrame = 12;
	}
}

bool Yoshi::CanPressEnemy()
{
	if (m_pPassenger)
	{
		return true;
	}
	return false;
}

bool Yoshi::CanRided(GameObjectBase * pByWho)
{
	if (m_nState == STATE_ITEMPOP)
		return false;
	if (m_bHatched == false)
		return false;
	if (m_nState == STATE_HATCH)
		return false;

	if (m_pPassenger)
		return false;

	switch (m_nState)
	{
	case STATE_PIPEUP:
		return false;
		break;
	}

	return true;
}

bool Yoshi::CanStandOnSpike()
{
	return true;
}

int Yoshi::GetPower(int nPowerForWhat)
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
		return POWER_PRESS_YOSHI;
		break;
	}
	
	return 0;
}

void Yoshi::Render(int nColor, int nZOrder)
{
	if (m_nDamageCooltime > 0)
	{
		if (m_pGame->m_nGameFrame % 2 == 1)
			return;
	}

	if (m_nGiantTime > 0)
	{
		if (m_nGiantTime < 8 * 24)
		{
			if (m_pGame->m_nGameFrame % 4 < 2)
				return;
		}
		else if (m_nGiantTime < 4 * 24)
		{
			if (m_pGame->m_nGameFrame % 2 < 1)
				return;
		}
	}

	float _x = x, _y;
	if (m_pClimb)
	{
		if (!m_bFlip)
			_x = m_pClimb->m_fX - 6;
		else
			_x = m_pClimb->m_fX + 6;
	}

	NaPointT<float> ptCamera = CAMERA->GetPosition();
	_x = _x - ptCamera.x;
	_y = y - ptCamera.y;

	float fScaleX, fScaleY;
	GetScale(fScaleX, fScaleY);

	if (nZOrder == -1)
		nZOrder = GetZOrder();
	bool bShadow = (m_pGame->m_bModernStyle);

	int nBodyFrame = GetBodySpriteIndex();
	if (nBodyFrame != -1)
		(*m_ppSprite)->RenderToQueue(_x, _y, nBodyFrame, m_bFlip, m_bFlipV, -1L, fScaleX, fScaleY, nZOrder, bShadow);

	int nHeadFrame = GetHeadSpriteIndex();
	if (nHeadFrame != -1)
	{
		POINT ptOffset = GetHeadSpriteOffset();
		_x += ptOffset.x;
		_y += ptOffset.y;
		(*m_ppSprite)->RenderToQueue(_x, _y, nHeadFrame, m_bFlip, m_bFlipV, -1L, fScaleX, fScaleY, nZOrder + 1, bShadow);
	}
}

int Yoshi::GetSpriteIndex()
{
	if (m_nState == STATE_ITEMPOP && m_nStateFrame < 30)
		return -1;
	if (m_nState == STATE_ITEMPOPDOWN && m_nStateFrame < 4)
		return -1;

	return -1;
}

int Yoshi::GetBodySpriteIndex()
{
	if (m_nState == STATE_ITEMPOP && m_nStateFrame < 30)
		return -1;
	if (m_nState == STATE_ITEMPOPDOWN && m_nStateFrame < 4)
		return -1;

	int nFrame = m_nBodyFrame;
	if (m_bHatched == false)
	{
		if (m_nHatchFrame < 8)
			nFrame = SPRIDX_VEHICLE_YOSHIEGG;
		else if (m_nHatchFrame < 28)
			nFrame = SPRIDX_VEHICLE_YOSHIEGG2;
		else if (m_nHatchFrame < 36)
			nFrame = SPRIDX_VEHICLE_YOSHIBABY2;
		else if (m_nHatchFrame < 44)
			nFrame = SPRIDX_VEHICLE_YOSHIBABY;
		else if (m_nHatchFrame < 80)
			nFrame = SPRIDX_VEHICLE_YOSHI_BODY_SIT;
		
		m_nBodyFrame = nFrame;
		return nFrame;
	}
	else if (nFrame == 0)
	{
		nFrame = SPRIDX_VEHICLE_YOSHI_BODY_STAND;
	}

	switch (m_nState)
	{
	case STATE_ITEMPOP:
		if (m_nStateFrame < 30)
			nFrame = -1;
		else
			nFrame = SPRIDX_VEHICLE_YOSHIEGG;
		break;
	case STATE_ITEMPOPDOWN:
		if (m_nStateFrame < 4)
			nFrame = -1;
		else
			nFrame = SPRIDX_VEHICLE_YOSHIEGG;
		break;
	case STATE_HATCH:
		if (m_bHatched)
			nFrame = SPRIDX_VEHICLE_YOSHI_BODY_STAND;

		/*
		if (m_nStateFrame < 8)
			nFrame = SPRIDX_VEHICLE_YOSHIEGG;
		else if (m_nStateFrame < 28)
			nFrame = SPRIDX_VEHICLE_YOSHIEGG2;
		else if (m_nStateFrame < 36)
			nFrame = SPRIDX_VEHICLE_YOSHIBABY2;
		else if (m_nStateFrame < 44)
			nFrame = SPRIDX_VEHICLE_YOSHIBABY;
		else if (m_nStateFrame < 80)
			nFrame = SPRIDX_VEHICLE_YOSHI_BODY_SIT;
			*/
		break;
	case STATE_IDLE:
		nFrame = SPRIDX_VEHICLE_YOSHI_BODY_SIT;
		break;
	case STATE_STAND:
		nFrame = SPRIDX_VEHICLE_YOSHI_BODY_STAND;
		break;
	case STATE_WALK:
		{
			int nRef;
			if (m_pPassenger)
			{
				nRef = (m_nWalkFrame % 3);
			}
			else
				nRef = SPRITE_INDEX_CYCLE(0, 3, 2);

			switch (nRef)
			{
			case 0:
				nFrame = SPRIDX_VEHICLE_YOSHI_BODY_WALK1;
				break;
			case 1:
				nFrame = SPRIDX_VEHICLE_YOSHI_BODY_WALK2;
				break;
			case 2:
				nFrame = SPRIDX_VEHICLE_YOSHI_BODY_STAND;
				break;
			}
		}
		break;
	case STATE_SIT:
		nFrame = SPRIDX_VEHICLE_YOSHI_BODY_SIT;
		break;
	case STATE_PIPERIGHT:
	case STATE_PIPELEFT:
		if (m_nStateFrame % 6 < 3)
			nFrame = SPRIDX_VEHICLE_YOSHI_BODY_SIT;
		else
			nFrame = SPRIDX_VEHICLE_YOSHI_BODY_SITWALK1;
		break;
	case STATE_PIPEDOWN:
	case STATE_PIPEUP:
		nFrame = SPRIDX_VEHICLE_YOSHI_BODY_STAND;
		break;
	case STATE_JUMP:
		nFrame = SPRIDX_VEHICLE_YOSHI_BODY_JUMP;
		break;
	case STATE_FALL:
	case STATE_BLOCKJUMP:
	case STATE_TRAMPOLINJUMP:
		nFrame = SPRIDX_VEHICLE_YOSHI_BODY_WALK1;
		break;
	}

	// Throw Fireball ignore action frames
	switch (m_nState)
	{
	default:
		if (m_nThrowFireFrame > 0)
		{
			nFrame = SPRIDX_VEHICLE_YOSHI_BODY_EAT;
		}
		break;
	}

	m_nBodyFrame = nFrame;

	return nFrame;
}

int Yoshi::GetHeadSpriteIndex()
{
	if (m_nState == STATE_ITEMPOP && m_nStateFrame < 30)
		return -1;
	if (m_nState == STATE_ITEMPOPDOWN && m_nStateFrame < 4)
		return -1;

	int nFrame = m_nHeadFrame;
	if (m_bHatched == false)
	{
		if (m_nHatchFrame < 44)
			nFrame = -1;
		else if (m_nHatchFrame >= 44 && m_nHatchFrame < 80)
			nFrame = SPRIDX_VEHICLE_YOSHI_HEAD_IDLE;

		m_nHeadFrame = nFrame;
		return nFrame;
	}
	else if (nFrame)
	{
		nFrame = SPRIDX_VEHICLE_YOSHI_HEAD_IDLE;
	}

	switch (m_nState)
	{
	case STATE_IDLE:
		if (m_nStateFrame % 24 < 12)
			nFrame = SPRIDX_VEHICLE_YOSHI_HEAD_IDLE;
		else
			nFrame = SPRIDX_VEHICLE_YOSHI_HEAD_IDLE2;
		break;
	case STATE_STAND:
		nFrame = SPRIDX_VEHICLE_YOSHI_HEAD_IDLE;
		break;
	case STATE_WALK:
		if (m_pPassenger)
			nFrame = SPRIDX_VEHICLE_YOSHI_HEAD_IDLE;
		else
			nFrame = SPRIDX_VEHICLE_YOSHI_HEAD_ANGRY;
		break;
	case STATE_SIT:
		nFrame = SPRIDX_VEHICLE_YOSHI_HEAD_IDLE;
		break;
	case STATE_PIPERIGHT:
	case STATE_PIPELEFT:
		nFrame = SPRIDX_VEHICLE_YOSHI_HEAD_IDLE;
		break;
	case STATE_PIPEDOWN:
	case STATE_PIPEUP:
		nFrame = SPRIDX_VEHICLE_YOSHI_HEAD_TURN;
		break;
	case STATE_JUMP:
	case STATE_BLOCKJUMP:
	case STATE_TRAMPOLINJUMP:
		if (m_pPassenger)
			nFrame = SPRIDX_VEHICLE_YOSHI_HEAD_ANGRY;
		else
			nFrame = SPRIDX_VEHICLE_YOSHI_HEAD_IDLE;
		break;
	}

	// Throw Fireball ignore action frames
	switch (m_nState)
	{
	default:
		if (m_nThrowFireFrame > 0)
		{
			nFrame = SPRIDX_VEHICLE_YOSHI_HEAD_EAT;
		}
		break;
	}

	if (m_nKeepInMouthCooltime > 0)
	{
		nFrame = SPRIDX_VEHICLE_YOSHI_HEAD_SOMETHING_IN_MOUTH;
		if (m_nKeepInMouthCooltime < 24*3 &&
			m_nKeepInMouthCooltime % 16 < 8)
		{
			nFrame = SPRIDX_VEHICLE_YOSHI_HEAD_GULP2;
		}
	}

	if (m_nGulpCooltime > 0)
	{
		if (m_nGulpCooltime > 12)
			nFrame = SPRIDX_VEHICLE_YOSHI_HEAD_GULP1;
		else if (m_nGulpCooltime > 9)
			nFrame = SPRIDX_VEHICLE_YOSHI_HEAD_GULP2;
		else if (m_nGulpCooltime > 6)
			nFrame = SPRIDX_VEHICLE_YOSHI_HEAD_GULP3;
		else if (m_nGulpCooltime > 3)
			nFrame = SPRIDX_VEHICLE_YOSHI_HEAD_GULP4;
		else 
			nFrame = SPRIDX_VEHICLE_YOSHI_HEAD_GULP5;
	}

	m_nHeadFrame = nFrame;

	return nFrame;
}

POINT Yoshi::GetHeadSpriteOffset()
{
	POINT pt = { 6, -16 };

	int nBodyFrame = GetBodySpriteIndex();
	switch (nBodyFrame)
	{
	case SPRIDX_VEHICLE_YOSHI_BODY_SIT:
	case SPRIDX_VEHICLE_YOSHI_BODY_SITWALK1:
		pt.x = 6;

		if (m_nState == STATE_SIT ||
			m_nState == STATE_PIPERIGHT)
			pt.y = -7;
		else
			pt.y = -12;
		break;
	case SPRIDX_VEHICLE_YOSHI_BODY_WALK1:
		pt.y = -14;
		break;
	case SPRIDX_VEHICLE_YOSHI_BODY_WALK2:
		pt.y = -15;
		break;
	case SPRIDX_VEHICLE_YOSHI_BODY_STAND:
	case SPRIDX_VEHICLE_YOSHI_BODY_JUMP:
		pt.y = -16;
		break;
	case SPRIDX_VEHICLE_YOSHI_BODY_EAT:
		pt.x = 8;
		pt.y = -8;
		break;	
	default:
		break;
	}

	int nHeadFrame = GetHeadSpriteIndex();
	if (nHeadFrame == SPRIDX_VEHICLE_YOSHI_HEAD_TURN)
	{
		pt.x = 7;
		pt.y = -14;
	}

	if (m_bFlip)
		pt.x *= -1;

	return pt;
}

int Yoshi::GetZOrder()
{
	switch (m_nState)
	{
	case STATE_PIPEDOWN:
	case STATE_PIPERIGHT:
	case STATE_PIPEUP:
		return Z_IN_PIPE_VEHICLE_BEHIND_OBJ;
		break;
	case STATE_ITEMPOP:
	case STATE_ITEMPOPDOWN:
		return Z_BEHIND_MAP;
		break;
	}

	return Z_VEHICLE_BEHIND_OBJ;
}

NaRect Yoshi::GetRect()
{
	NaRect rcMe = GameObjectBase::GetRect();
	/*
	if (m_pPassenger == nullptr)
		return rcMe;

	if (m_pPassenger != this)
	{
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
	}
	*/
	return rcMe;
}

SIZE Yoshi::GetSize()
{
	SIZE s = { 16, 16 };

	if (m_pPassenger)
	{
		s.cy = 32;

		if (m_nState == STATE_SIT)
			s.cy = 24;
	}

	s.cx *= m_fScaleX;
	s.cy *= m_fScaleY;
	return s;
}

void Yoshi::GetScale(float & fScaleX, float & fScaleY)
{
	fScaleX = m_fScaleX;
	fScaleY = m_fScaleY;

	// #TODO Implementation
	/*
	// ScaleWithPassenger
	float fScalePX = m_fScaleX;
	float fScalePY = m_fScaleY;

	NaRect rcMe = GameObjectBase::GetRect();
	NaRect rcBoth = rcMe;
	if (m_pPassenger)
	{
		NaRect rcP = m_pPassenger->GetRect();

		// Add two rect.
		if (rcP.left < rcBoth.left)
			rcBoth.left = rcP.left;
		if (rcP.right > rcBoth.right)
			rcBoth.right = rcP.right;
		if (rcP.top < rcBoth.top)
			rcBoth.top = rcP.top;
		if (rcP.bottom > rcBoth.bottom)
			rcBoth.bottom = rcP.bottom;

		fScalePX = (float)rcBoth.Width() / (float)rcMe.Width();
		fScalePY = (float)rcBoth.Height() / (float)rcMe.Height();
	}

	fScaleX = m_fScaleX;
	fScaleY = m_fScaleY;

	// #TODO tuning minimum Scale value.

	switch (m_nState)
	{
	case STATE_PIPEDOWN:
		{
			if (rcBoth.Width() > 32)
			{
				float fMinScale = 1.0f;
				fScaleX = fScalePX - (m_nStateFrame * 0.1f);
				fScaleY = fScalePY - (m_nStateFrame * 0.1f);

				if (fScaleX < fMinScale)
					fScaleX = fScaleY = fMinScale;
			}
		}
		break;
	case STATE_PIPERIGHT:
		{
			if (rcBoth.Height() > 32)
			{
				float fMinScale = 1.0f;
				fScaleX = fScalePX - (m_nStateFrame * 0.1f);
				fScaleY = fScalePY - (m_nStateFrame * 0.1f);

				if (fScaleX < fMinScale)
					fScaleX = fScaleY = fMinScale;

				// Total 32frame
				m_fX += ((32.0f / 16.0f) / fScalePX) - m_fXS;
			}
		}
		break;
	case STATE_PIPEUP:
		{
			if (rcBoth.Width() > 32)
			{
				float fMinScale = 1.0f;
				fScaleX = fScalePX - ((30 - m_nStateFrame) * 0.1f);
				fScaleY = fScalePY - ((30 - m_nStateFrame) * 0.1f);

				if (fScaleX < fMinScale)
					fScaleX = fScaleY = fMinScale;
			}
		}
		break;
	}
	*/
}
