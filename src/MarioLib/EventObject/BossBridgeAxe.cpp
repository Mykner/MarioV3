#include "BossBridgeAxe.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "FModManager.h"
#include "Stage.h"
#include "SoundManager.h"

BossBridgeAxe::BossBridgeAxe(Game *pGame, Stage *pStage) :
	EventObjectBase(pGame, pStage)
{
	m_nType = EVENTOBJECT_BOSSBRIDGE_AXE;
	m_nState = STATE_IDLE;
}

BossBridgeAxe::~BossBridgeAxe()
{
}

void BossBridgeAxe::Process()
{
	ProcessBase();

	if (CanApplied() && m_bCollisionPlayer)
	{
		CUR_PLAYER_OBJ->OnApplyEvent(this);
		if (m_pGame->m_bModernStyle)
			ChangeState(STATE_EVENTDEAD);
		else
			ChangeState(STATE_DEAD);
	}
}

void BossBridgeAxe::OnBeforeProcess()
{
	CheckCollision();
	CheckCollisionPlayer();
}

void BossBridgeAxe::Render(int nColor, int nZOrder)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	float _x = x - pt.x;
	float _y = y - pt.y;
	bool bShadow = (m_pGame->m_bModernStyle);

	int nFrame = GetSpriteIndex();
	if (m_pGame->m_bModernStyle)
	{
		float fAngle = 0;
		if (m_nState == STATE_EVENTDEAD)
		{
			fAngle = -(m_nStateFrame * 10);
			if (fAngle < -90.0f)
				fAngle = -90.0f;
		}
		else if (m_bEventDead)
		{
			fAngle = -90.0f;
		}

		(*m_ppSprite)->RenderToQueue(_x, _y, nFrame,
			false, false, nColor, 1.0f, 1.0f, Z_ITEM, bShadow, fAngle, 1);
	}
	else
	{
		(*m_ppSprite)->RenderToQueue(_x, _y, nFrame,
			false, false, nColor, 1.0f, 1.0f, Z_ITEM, bShadow);
	}	
}

void BossBridgeAxe::OnEventDead()
{
	if (m_nStateFrame == 0)
		m_bEventDead = true;

	if (!IS_CLASSIC) 
	{
		if (m_nStateFrame == 9 && m_bCollision[COLLISION_BOTTOM])
		{
			CUR_STAGE->MakeEarthquake(POWER_EARTHQUAKE_UNKNOWN);

			SOUND_MANAGER->PlaySoundEffect(L"Cannon");
		}
		else if (m_nStateFrame > 9)
		{
			if (m_bCollision[COLLISION_BOTTOM])
			{
				if (m_fYS != 0)
				{
					m_fYS = 0;
					//ChangeState(STATE_BURNT);
				}
			}
			else
			{
				OnApplyGravity(0.5f);
			}
		}
	}
}

int BossBridgeAxe::GetSpriteIndex()
{
	int nVal = (m_pGame->m_nGameFrame % 48);
	if (IS_CLASSIC)
	{
		if (nVal < 24)
			return SPRIDX_MAPOBJ_BOSSBRIDGE_AXE1;
		else if (nVal < 32)
			return SPRIDX_MAPOBJ_BOSSBRIDGE_AXE1 + 1;
		else if (nVal < 40)
			return SPRIDX_MAPOBJ_BOSSBRIDGE_AXE1 + 2;
		else if (nVal < 48)
			return SPRIDX_MAPOBJ_BOSSBRIDGE_AXE1 + 1;
	}
	else
	{
		if (nVal < 24)
			return SPRIDX_MAPOBJ_BOSSBRIDGE_LARGE_AXE1;
		else if (nVal < 32)
			return SPRIDX_MAPOBJ_BOSSBRIDGE_LARGE_AXE2;
		else if (nVal < 40)
			return SPRIDX_MAPOBJ_BOSSBRIDGE_LARGE_AXE3;
		else if (nVal < 48)
			return SPRIDX_MAPOBJ_BOSSBRIDGE_LARGE_AXE2;
	}

	return SPRIDX_MAPOBJ_BOSSBRIDGE_AXE1;
}

bool BossBridgeAxe::CanApplied()
{
	if (m_nState == STATE_EVENTDEAD)
		return false;

	if (m_bEventDead)
		return false;

	return true;
}

SIZE BossBridgeAxe::GetSize()
{
	SIZE s = { 16, 16 };
	if (!IS_CLASSIC)
		s.cy = 46;

	return s;
}
