#include "Vine.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "FModManager.h"
#include "Stage.h"
#include "SoundManager.h"

Vine::Vine(Game *pGame, Stage *pStage) :
	ItemObjectBase(pGame, pStage)
{
	m_nType = ITEM_VINE;
	m_nState = STATE_ITEMPOP;

	m_nBaseState = STATE_VINEGROW;

	m_nLength = 0;
	m_nMaxLength = -1;
}

Vine::~Vine()
{
}

void Vine::Process()
{
	ProcessBase();

	if (CanApplied() && m_bCollisionPlayer)
	{
		CUR_PLAYER_OBJ->OnApplyItem(this);
	}
}

void Vine::ProcessBase()
{
	switch (m_nState)
	{
	case STATE_ITEMPOP:
		if (m_nStateFrame == 0)
			SOUND_MANAGER->PlaySoundEffect(L"Vine");

		OnItemPop();
		break;
	case STATE_ITEMPOPDOWN:
		if (m_nStateFrame == 0)
			SOUND_MANAGER->PlaySoundEffect(L"Vine");

		OnItemPopDown();
		break;
	case STATE_VINEGROW:
		OnVineGrow();
		break;
	default:
		ItemObjectBase::ProcessBase();
		return;
	}
}

void Vine::OnBeforeProcess()
{
	CheckCollision();
	CheckCollisionPlayer();
}

void Vine::Render(int nColor, int nZOrder)
{
	if (m_nState == STATE_ITEMPOP && m_nStateFrame < 30)
		return;

	if (m_nState == STATE_ITEMPOPDOWN && m_nStateFrame < 4)
		return;

	NaPointT<float> pt = CAMERA->GetPosition();
	float _x = x - pt.x;
	float _y = y - pt.y;

	bool bShadow = m_pGame->m_bModernStyle;

	// Head
	(*m_ppSprite)->RenderToQueue(
		_x,
		_y - m_nLength,
		SPRIDX_VINEHEAD,
		false, false, -1L, 1.0f, 1.0f, Z_BEHIND_MAP, bShadow
	);

	// Body
	for (int i = 0; i < m_nLength; i += 16)
	{
		(*m_ppSprite)->RenderToQueue(
			_x,
			_y - m_nLength + 16 + i,
			SPRIDX_VINEBODY,
			false, false, -1L, 1.0f, 1.0f, Z_BEHIND_MAP, bShadow
		);
	}
}

void Vine::ChangeState(int nState, ChangeStateInfo * pInfo)
{
	switch (nState)
	{
	case STATE_ITEMPOP:
		break;
	case STATE_VINEGROW:
		m_fXS = 0;
		m_fYS = 0;
		break;
	case STATE_TRAMPOLINPRESS:
	case STATE_TRAMPOLINJUMP:
		return;
		break;
	default:
		ItemObjectBase::ChangeState(nState, pInfo);
		return;
		break;
	}

	m_nNextState = nState;
}

void Vine::OnVineGrow()
{
	OnApplyGravity(1.0f, 4.0f);

	if (m_nStateFrame % 2 == 0)
		m_nLength++;

	if ((m_nMaxLength == -1 && m_nLength >= 64.0f) ||
		(m_nMaxLength != -1 && m_nLength >= m_nMaxLength))
		ChangeState(STATE_STAND);

	if (m_fYS > 0)
	{
		if (m_bCollision[COLLISION_BOTTOM] &&
			(int)m_fY <= m_ptCollision[COLLISION_BOTTOM].y)
		{
			m_fXS = 0;
			m_fYS = 0;
		}
	}
}

bool Vine::CanApplied()
{
	if (m_nState == STATE_ITEMPOP)
		return false;

	return true;
}

bool Vine::CanEat()
{
	return false;
}

SIZE Vine::GetSize()
{
	SIZE s = {
		2, 16 + m_nLength
	};

	if (m_nState == STATE_VINEGROW ||
		m_nState == STATE_STAND)
	{
		if (m_pGame->m_nStagePackType == STAGEPACK_CLASSIC)
		{
			s.cy = m_nLength;
		}
		else
		{
			s.cy = 16 + m_nLength;
		}
	}
	return s;
}
