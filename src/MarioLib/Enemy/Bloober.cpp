#include "Bloober.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "SoundManager.h"

BEGIN_IMPL_PROPERTYMAP(Bloober)
	PROP_BOOL("PowerUp", VT_BOOL, false, nullptr, "Bloober"),
	PROP_BOOL("Winged", VT_BOOL, false, nullptr, "Bloober")
END_IMPL_PROPERTYMAP()

Bloober::Bloober(Game *pGame, Stage *pStage) :
	EnemyObjectBase(pGame, pStage)
{
	m_nType = ENEMY_BLOOBER;
	m_nState = STATE_IDLE;

	m_bFlip = true;

	m_nBaseState = STATE_SWIM;
}

Bloober::~Bloober()
{
}

void Bloober::ChangeState(int nState, ChangeStateInfo *pInfo)
{
	switch (nState)
	{
	case STATE_PRESSED:
		if (m_bWinged)
		{
			m_fXS = 0;
			m_fYS = 0.5f;

			ReleaseWing();
			nState = STATE_SWIM;

			SOUND_MANAGER->PlaySoundEffect(L"Pressed");
			return;
		}
		else
		{
			EnemyObjectBase::ChangeState(nState, pInfo);
			return;
		}
		break;
	case STATE_PUSHEDUP:
	case STATE_BURNT:
		// Use Default
	default:
		EnemyObjectBase::ChangeState(nState, pInfo);
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

void Bloober::OnChangeState(int nState)
{
	ChangeStateInfo *pInfo = nullptr;
	if (m_ChangeStateInfo.nNextState != -1)
		pInfo = &m_ChangeStateInfo;

	switch (nState)
	{
	case STATE_SWIM:
		m_fXS = 0;
		m_fYS = 0.75f;
		break;
	case STATE_SWIMJUMP:
		m_fXS = 0;
		m_fYS = 0;
		break;
	default:
		EnemyObjectBase::OnChangeState(nState);
		return;
		break;
	}
}

void Bloober::OnIdle()
{
	ChangeState(STATE_SWIM);
}

void Bloober::OnSwim()
{
	if (m_fY > 208 - 20 ||
		(CUR_PLAYER_OBJ->m_fY < m_fY && rand() % 50 == 0))
		ChangeState(STATE_SWIMJUMP);
}

void Bloober::OnSwimJump()
{
	if (m_nStateFrame < 8)
	{
		// wait
	}

	if (m_nStateFrame == 8)
	{
		if (rand() % 2 == 0)
		{
			m_bFlip = false;
			m_fXS = 1.5f;
		}
		else
		{
			m_bFlip = true;
			m_fXS = -1.5f;
		}
		
		m_fYS = -1.5f;

		if (m_bWinged)
		{
			m_fXS *= 2.0f;
			//m_fYS *= 2.0f;
		}		
	}

	if (!m_bWinged)
	{
		if (m_nStateFrame == 32)
		{
			m_fXS = 0;
			m_fYS = 0.5f;
		}

		// 30~56: Jump Charge
		if (m_nStateFrame == 56)
			ChangeState(STATE_SWIM);
	}
	else
	{
		if (m_nStateFrame > 8 && m_nStateFrame < 32 && m_fYS > -4.0f)
			m_fYS += -0.25f;

		if (m_nStateFrame == 32)
		{
			m_fXS = 0;
			m_fYS = 0.5f;
		}

		if (m_nStateFrame == 56)
		{
			if (m_fY > 36 && rand() % 2 == 0)
				ChangeState(STATE_SWIMJUMP);
			else
				ChangeState(STATE_SWIM);
		}
	}
}

void Bloober::OnPressed()
{
	if (m_nStateFrame > 40)
		ChangeState(STATE_DEAD);
}

bool Bloober::IsAlive()
{
	return EnemyObjectBase::IsAlive();
}

bool Bloober::CanPressed(int nPower)
{
	if (m_pGame->m_nStagePackType == STAGEPACK_CLASSIC)
		return false;

	return true;
}

bool Bloober::CanBounceTrampolinMiniHorz()
{
	return false;
}

std::vector<POINT> Bloober::GetWingPosition()
{
	POINT pt;
	SIZE s = GetSize();
	pt.x = -7;
	pt.y = -5;

	std::vector<POINT> vec;
	vec.push_back(pt);

	pt.x = +7;
	vec.push_back(pt);

	return vec;
}

int Bloober::GetWingSpriteIndex()
{
	int nFrame = SPRIDX_WING1;
	if (m_fYS < 0)
	{
		if (m_nStateFrame % 16 >= 8)
			nFrame = SPRIDX_WING2;
	}

	return nFrame;
}

int Bloober::GetSpriteIndex()
{
	if (EnemyObjectBase::GetSpriteIndex() == -1)
		return -1;

	int nFrame;
	switch (m_nState)
	{
	case STATE_PRESSED:
	case STATE_PUSHEDUP:
	case STATE_BURNT:
		nFrame = SPRIDX_BLOOBER1;
		break;
	case STATE_SWIMJUMP:
		if (m_nStateFrame >= 36)
			nFrame = SPRIDX_BLOOBER2; // <8<
		else
			nFrame = SPRIDX_BLOOBER1; // <8===== ~~
		break;
	case STATE_SWIM:
		if (m_fYS > 0)
			nFrame = SPRIDX_BLOOBER2;
		else
			nFrame = SPRITE_INDEX_CYCLE(SPRIDX_BLOOBER1, 2, 2);
		break;
	case STATE_PIPEUP:
		nFrame = SPRIDX_BLOOBER1;
		break;
	case STATE_STACKED:
		nFrame = SPRITE_INDEX_CYCLE(SPRIDX_BLOOBER1, 2, 16);
		break;
	default:
		if (m_fYS <= 0)
			nFrame = SPRITE_INDEX_CYCLE(SPRIDX_BLOOBER1, 2, 2);
		else if (m_fYS > 0)
			nFrame = SPRIDX_BLOOBER2;
		break;
	}

	return nFrame;
}
