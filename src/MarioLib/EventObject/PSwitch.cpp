#include "PSwitch.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "FModManager.h"
#include "Stage.h"
#include "TypeNames.h"
#include "SoundManager.h"

#include "UIBase.h"
#include "SpeechBubble.h"

PSwitch::PSwitch(Game *pGame, Stage *pStage) :
	EventObjectBase(pGame, pStage)
{
	m_nType = EVENTOBJECT_P_SWITCH;
	m_nState = STATE_IDLE;

	m_bCanLand = true;
	m_bCanCollide = true;

	m_nPressedTimer = 0;
}

PSwitch::~PSwitch()
{
}

void PSwitch::Process()
{
	ProcessBase();

	if (m_nPressedTimer > 0)
	{
		m_nPressedTimer++;
		if (m_nPressedTimer == 48)
			ChangeState(STATE_DEAD);
	}
}

void PSwitch::OnBeforeProcess()
{
	CheckCollision();
}

void PSwitch::OnAfterProcess()
{
	m_fX += m_fXS;
	m_fY += m_fYS;

    ProcessState();
}

void PSwitch::Render(int nColor, int nZOrder)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	float _x = x - pt.x;
	float _y = y - pt.y;

	int nFrame = GetSpriteIndex();
	bool bShadow = m_pGame->m_bModernStyle;
	if (nZOrder == -1)
		nZOrder = GetZOrder();
	
	(*m_ppSprite)->RenderToQueue(_x, _y, nFrame,
		false, false, nColor, 1.0f, 1.0f, nZOrder, bShadow);
}

void PSwitch::ChangeState(int nState, ChangeStateInfo * pInfo)
{
	switch (nState)
	{
	case STATE_PRESSED:
		{
			if (m_nPressedTimer == 0)
			{
				m_nPressedTimer = 1;

				CUR_STAGE->MakeEarthquake();

				m_pGame->SwapCoinAndBrick(true);
				CUR_PLAYER->m_nPSwitchTime = 60 * 12;

				SOUND_MANAGER->PlayPSwitchBGM();
				SOUND_MANAGER->PlaySoundEffect(L"Cannon");
			}
		}
		break;
	default:
		EventObjectBase::ChangeState(nState, pInfo);
		return;
		break;
	}

	m_nNextState = nState;
}

void PSwitch::OnIdle()
{
	CheckFalling();
}

void PSwitch::OnReachWall(bool bIsLeftWall)
{
	SIZE s = GetSize();

	if (bIsLeftWall)
		m_fX = (m_ptCollision[COLLISION_LEFT].x + 1) + s.cx / 2 + 1;
	else
		m_fX = (m_ptCollision[COLLISION_RIGHT].x) - s.cx / 2;

	m_fXS *= -0.5f;
}

void PSwitch::OnPressed()
{
	if (m_bCollision[COLLISION_BOTTOM])
	{
		if (m_fY > m_ptCollision[COLLISION_BOTTOM].y)
		{
			m_fY = m_ptCollision[COLLISION_BOTTOM].y;
		}

		m_fYS = 0;
	}
	else 
	{
		OnApplyGravity(1, 4.0f);
	}
}

void PSwitch::OnLanding()
{
	m_fY = m_ptCollision[COLLISION_BOTTOM].y;
	
	if (abs(m_fYS) > 1.0f)
	{
		m_fXS *= 0.7f;
		m_fYS = -0.5f * m_fYS;
	}
	else
	{
		m_fXS = 0;
		m_fYS = 0;
	}

	if (m_nPressedTimer == 0)
	{
		if (m_bStandOnJumpBlock)
			ChangeState(STATE_BLOCKJUMP);
		else
			ChangeState(STATE_STAND);
	}
}

bool PSwitch::IsAlive()
{
	return (m_nState != STATE_DEAD);
}

bool PSwitch::CanBornFromBlock()
{
	return true;
}

bool PSwitch::CanCarried()
{
	switch (m_nState)
	{
	case STATE_ITEMPOP:
	case STATE_ITEMPOPDOWN:
		return false;
		break;
	}

	if (m_nPressedTimer > 0)
		return false;

	return true;
}

void PSwitch::AddPassenger(GameObjectBase * pNewObj)
{
	if (pNewObj->m_nType >= BULLET_BEGIN && pNewObj->m_nType < BULLET_END)
		return;

	switch (m_nState)
	{
	case STATE_ITEMPOP:
	case STATE_ITEMPOPDOWN:
	case STATE_PIPEUP:
	case STATE_PIPEDOWN:
	case STATE_PIPELEFT:
	case STATE_PIPERIGHT:
		return;
		break;
	}

	if (m_nPressedTimer == 0 && IsAlive())
		ChangeState(STATE_PRESSED);
}

int PSwitch::GetSpriteIndex()
{
	if (EventObjectBase::GetSpriteIndex() == -1)
		return -1;

	int nFrame = SPRIDX_MAPOBJ_P_SWITCH1;

	if (m_nPressedTimer > 0)
		nFrame = SPRIDX_MAPOBJ_P_SWITCH_PRESSED;
	else
	{
		nFrame = (SPRIDX_MAPOBJ_P_SWITCH1 + (m_pGame->m_nGameFrame % 24) / 12);
	}

	return nFrame;
}

int PSwitch::GetZOrder()
{
	int nZOrder = GameObjectBase::GetZOrder();
	if (nZOrder != 0)
		return nZOrder;

	if (m_nState == STATE_ITEMPOP || m_nState == STATE_ITEMPOPDOWN)
		return Z_BEHIND_MAP;

	return Z_COMMON_OBJ;
}

SIZE PSwitch::GetSize()
{
	SIZE s = { 16, 14 };

	if (m_nPressedTimer > 0)
		s.cy = 7;

	return s;
}
