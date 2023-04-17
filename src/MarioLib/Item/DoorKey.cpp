#include "DoorKey.h"

#include "Game.h"
#include "Stage.h"
#include "Player.h"
#include "SoundManager.h"
#include "Camera.h"

DoorKey::DoorKey(Game *pGame, Stage *pStage) :
	ItemObjectBase(pGame, pStage)
{
	m_nType = ITEM_DOORKEY;
	m_nState = STATE_ITEMPOP;

	m_nBaseState = STATE_FLY;
	m_pFollow = nullptr;
	m_fTargetDistance = 0;
	m_fCurrentDistance = 0;

	m_fWalkingBounceOffset = 0;
	m_fWalkingBounceSpeed = 0;
}

DoorKey::~DoorKey()
{
}

void DoorKey::Process()
{
	//ItemObjectBase::Process();

	switch (m_nState)
	{
	case STATE_WALK:
		if (m_nStateFrame == 0)
		{
			if (!m_bFlip)
				m_fXS = 1.0f;
			else
				m_fXS = -1.0f;
		}

		OnWalk();
		break;
	case STATE_ITEMPOP:
		if (m_nStateFrame == 0)
		{
			SOUND_MANAGER->PlaySoundEffect(L"ItemPop");
		}

		OnItemPop();
		break;
	case STATE_ITEMPOPDOWN:
		if (m_nStateFrame == 0)
		{
			SOUND_MANAGER->PlaySoundEffect(L"ItemPop");
		}

		OnItemPopDown();
		break;
	default:
		ProcessBase();
		break;
	}

	if (m_bCollisionPlayer && m_pFollow == nullptr)
	{
		if (CanApplied())
		{
			CUR_PLAYER_OBJ->OnApplyItem(this);
			SetState(STATE_DEAD);

			CUR_PLAYER->AddDoorKey(this);
		}
	}
}

void DoorKey::ChangeState(int nState, ChangeStateInfo * pInfo)
{
	switch (nState)
	{
	case STATE_TRAMPOLINPRESS:
	case STATE_TRAMPOLINJUMP:
		return;
	default:
		ItemObjectBase::ChangeState(nState, pInfo);
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

void DoorKey::OnChangeState(int nState)
{
	ChangeStateInfo *pInfo = nullptr;
	if (m_ChangeStateInfo.nNextState != -1)
		pInfo = &m_ChangeStateInfo;
	
	switch (nState)
	{
	case STATE_FOLLOWPREPARE:
		if (pInfo && pInfo->pFromObject)
		{
			m_ptCenter.x = m_fX;
			m_ptCenter.y = m_fY;

			m_pFollow = pInfo->pFromObject;

			float fRangeX = abs(m_pFollow->m_fX - m_fX);
			float fRangeY = abs(m_pFollow->m_fY - m_fY);
			
			m_fCurrentDistance = 0;
			m_fTargetDistance = sqrt((fRangeX * fRangeX) + (fRangeY * fRangeY));
			NaDebugOut(L"DoorKey's range is: %.2f\n", m_fTargetDistance);
		}
		m_fXS = 0;
		m_fYS = 0;
		break;
	default:
		ItemObjectBase::OnChangeState(nState);
		break;
	}
}

void DoorKey::OnItemPop()
{
	ItemObjectBase::OnItemPop();

	MakeTwinkle();
}

void DoorKey::OnFly()
{
	int nDistance = 16;
	int nSpeed = m_nStateFrame * 4;

	float fCur = (nDistance) * sin((nSpeed) * fToRadian);
	float fNew = (nDistance) * sin(((nSpeed + 1)) * fToRadian);

	m_fYS = fNew - fCur;
}

void DoorKey::OnFollowPrepare()
{
	if (m_pFollow)
	{
		/*
		if (m_fX < m_pFollow->m_fX)
		{
			//  432
			// 5  1    
			//6  start
			//7          end
			//8           14
			// 9         13
			//  10 11 12

		}

		float fDX = m_pFollow->m_fX - m_ptCenter.x;
		float fDY = m_pFollow->m_fY - m_ptCenter.y;
		float rad = atan2(fDX, fDY);
		float degree = (rad * 180) / PI;
		NaDebugOut(L"Angle: %f\n", degree);

		float fNewX = m_ptCenter.x + m_fCurrentDistance * cos(rad) - m_fCurrentDistance * sin(rad);
		float fNewY = m_ptCenter.y + m_fCurrentDistance * cos(rad) + m_fCurrentDistance * cos(rad);

		m_fX = fNewX;
		m_fY = fNewY;

		m_fCurrentDistance += 0.1f;
		if (m_fCurrentDistance >= m_fTargetDistance)
			ChangeState(STATE_STAND);
			*/

		MakeTwinkle();

		int nStayFrame = 96;
		if (m_nStateFrame < nStayFrame)
		{
			m_fY -= 0.25f;
			return;
		}

		if (m_nStateFrame == nStayFrame)
		{
			m_ptCenter.x = m_fX;
			m_ptCenter.y = m_fY;
		}
		
		float fRangeX = (m_pFollow->m_fX - m_ptCenter.x);
		float fRangeY = (m_pFollow->m_fY - m_ptCenter.y);

		int nTime = m_nStateFrame - nStayFrame;
		m_fX = m_ptCenter.x + (fRangeX * ((float)nTime / 60.0f));
		m_fY = m_ptCenter.y + (fRangeY * ((float)nTime / 60.0f));

		if (nTime == 56)
		{
			CUR_PLAYER_OBJ->OnApplyItem(this);
			SetState(STATE_DEAD);

			CUR_PLAYER->AddDoorKey(this);
		}
	}
	else
	{
		ChangeState(STATE_BURNT);
	}
}

void DoorKey::OnFollow()
{
	if (m_pFollow == nullptr)
		return;

	if (m_pFollow->IsAlive() == false)
	{
		m_pFollow = nullptr;
		ChangeState(STATE_BURNT);
		return;
	}

	GameObjectBase *pFollow = m_pFollow;
	if (pFollow->m_pVehicle)
		pFollow = (GameObjectBase*)pFollow->m_pVehicle;

	POINT pt;
	pt.x = pFollow->m_fX;
	pt.y = pFollow->m_fY;

	float fGround = m_fY - m_fWalkingBounceOffset;

	if (m_fWalkingBounceOffset < 0 || m_fWalkingBounceSpeed == -2.0f)
	{
		m_fWalkingBounceOffset += m_fWalkingBounceSpeed;
		m_fWalkingBounceSpeed += 0.3f;
	}
	else
	{
		if (m_fWalkingBounceSpeed > 0)
		{
			m_fWalkingBounceOffset = 0;
			m_fWalkingBounceSpeed = 0;
		}
	}

	bool bStand = false;
	if (m_vecFollow.size() > 2)
	{
		POINT ptLast = m_vecFollow[m_vecFollow.size() - 1];
		if (ptLast.x == pt.x && ptLast.y == pt.y &&
			fGround == pt.y)
		{
			// Do not move
			return;
		}
	}
	m_vecFollow.push_back(pt);

	if (m_vecFollow.size() > 12)
	{
		m_vecFollow.erase(m_vecFollow.begin());
	}

	if (m_vecFollow.size() > 1)
	{
		if (m_fY - m_fWalkingBounceOffset == m_vecFollow[1].y)
		{
			// Walking.
			if (m_fWalkingBounceOffset == 0 && m_fWalkingBounceSpeed == 0 &&
				m_bCollision[COLLISION_BOTTOM])
			{
				// Small Jump
				m_fWalkingBounceSpeed = -2.0f;
			}
		}
		else if (fGround != m_vecFollow[1].y)
		{
			// else if not walking?
			m_fWalkingBounceOffset = 0;
			m_fWalkingBounceSpeed = 0;
		}

		m_fX = m_vecFollow[1].x;
		m_fY = m_vecFollow[1].y + m_fWalkingBounceOffset;
	}
}

void DoorKey::MakeTwinkle()
{
	if (rand() % 4 > 0)
	{
		for (int i = 0; i < (rand() % 4); i++)
		{
			NaRect rc = GetRect();
			rc.left -= 8;
			rc.top -= 8;
			rc.right += 8;
			rc.bottom += 8;
			int nX = rc.left + (rand() % rc.Width());
			int nY = rc.top + (rand() % rc.Height());
			CreateParameter param;
			param.nType = EFFECT_GOLDTWINKLE;
			CUR_STAGE->CreateEffect(nX, nY, EFFECT_TWINKLE, &param);
		}
	}
}

bool DoorKey::IsOutOfCamera()
{
	switch (m_nState)
	{
	case STATE_FOLLOWPREPARE:
	case STATE_FOLLOW:
		return false;
		break;
	}

	return CAMERA->IsOutOfCamera(this);
}

bool DoorKey::CanApplied()
{
	//
	// #TODO Must fix this
	//
	if (m_nStateFrame == 0)
		return false;

	if (m_pFollow == nullptr)
		return true;

	if (m_nState == STATE_DEAD)
		return false;
	
	if (m_nState == STATE_BURNT)
		return false;

	if (m_nState != STATE_FOLLOW)
		return true;

	return false;
}

bool DoorKey::CanEat()
{
	return false;
}

int DoorKey::GetSpriteIndex()
{
	if (m_nState == STATE_DEAD || m_nNextState == STATE_DEAD)
		return -1;

	if (m_nState == STATE_ITEMPOP && m_nStateFrame < 30)
		return -1;

	if (m_nState == STATE_ITEMPOPDOWN && m_nStateFrame < 4)
		return -1;

	int nFrames = (m_pGame->m_nGameFrame % 72);

	if (nFrames < 48)
		return SPRIDX_KEY;
	else if (nFrames < 48 + (8 * 1))
		return SPRIDX_KEY2;
	else if (nFrames < 48 + (8 * 2))
		return SPRIDX_KEY3;
	else if (nFrames < 48 + (8 * 3))
		return SPRIDX_KEY4;

	return SPRIDX_KEY;
}

int DoorKey::GetZOrder()
{
	switch (m_nState)
	{
	case STATE_FOLLOW:
		return Z_FOLLOWING_DOORKEY;
		break;
	}

	return ItemObjectBase::GetZOrder();
}
