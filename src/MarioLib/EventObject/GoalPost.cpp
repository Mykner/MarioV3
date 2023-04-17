#include "GoalPost.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "FModManager.h"
#include "Stage.h"
#include "SoundManager.h"

GoalPost::GoalPost(Game *pGame, Stage *pStage) :
	EventObjectBase(pGame, pStage)
{
	m_nType = EVENTOBJECT_GOALPOST;
	m_nState = STATE_IDLE;

	m_fLineHeight = 0;
	m_fLineSpeed = -2.0f;
}

GoalPost::~GoalPost()
{
}

void GoalPost::CheckCollisionPlayer()
{
	m_bCollisionPlayer = false;
	if ((GameObjectBase*)CUR_PLAYER_OBJ == this)
		return;

	NaRect rcLine = GetRect();
	rcLine.bottom = m_fY + m_fLineHeight;
	rcLine.top = rcLine.bottom - 6;

	NaRect rcPlayer = CUR_PLAYER_OBJ->GetRect();
	if (rcLine.IsOverlapped(rcPlayer))
	{
		m_bCollisionPlayer = true;
	}
}

void GoalPost::Process()
{
	switch (m_nState)
	{
	case STATE_IDLE:
		OnIdle();
		break;
	case STATE_FLAGDOWN:
		OnFlagDown();
		break;
	}
	
	if (CanApplied() && m_bCollisionPlayer)
	{
		CUR_PLAYER_OBJ->OnApplyEvent(this);
		ChangeState(STATE_FLAGDOWN);

		CreateParameter param;

		int nPostHeight = GetRect().Height();
		int nPercent = (-m_fLineHeight * 100.0f) / nPostHeight;
		
		param.nScore = nPercent * 100;
		param.nType = EFFECT_SCORE;
		param.pFromObject = this;
		param.pFromPlayer = CUR_PLAYER;
		param.fYS = -1.0f;
		m_pStage->CreateEffect(m_fX, m_fY + m_fLineHeight, EFFECT_SCORE, &param);
	}
}

void GoalPost::OnBeforeProcess()
{
	//CheckCollision();
	CheckCollisionPlayer();
}

void GoalPost::Render(int nColor, int nZOrder)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	float _x = x - pt.x;
	float _y = y - pt.y;

	bool bShadow = false;
	if (m_pGame->m_bModernStyle)
		bShadow = true;

	// Body
	for (int i = 0; i < 9; i++)
	{
		(*m_ppSprite)->RenderToQueue(
			_x - 16, 
			_y - (i * TILE_YS),
			SPRIDX_MAPOBJ_GOALPOST_L,
			false, false, nColor, 1.0f, 1.0f, Z_BEHIND_MAP, bShadow
		);

		(*m_ppSprite)->RenderToQueue(
			_x + 16,
			_y - (i * TILE_YS),
			SPRIDX_MAPOBJ_GOALPOST_R,
			false, false, nColor, 1.0f, 1.0f, Z_MAPOBJECT, bShadow
		);
	}

	// Head
	(*m_ppSprite)->RenderToQueue(
		_x - 16,
		_y - (9 * TILE_YS),
		SPRIDX_MAPOBJ_GOALPOST_LT,
		false, false, nColor, 1.0f, 1.0f, Z_BEHIND_MAP, bShadow
	);

	(*m_ppSprite)->RenderToQueue(
		_x + 16,
		_y - (9 * TILE_YS),
		SPRIDX_MAPOBJ_GOALPOST_RT,
		false, false, nColor, 1.0f, 1.0f, Z_MAPOBJECT, bShadow
	);

	// Line
	if (m_nState == STATE_IDLE)
	{
		(*m_ppSprite)->RenderToQueue(
			_x,
			_y + m_fLineHeight,
			SPRIDX_MAPOBJ_GOALPOST_LINE,
			false, false, nColor, 1.0f, 1.0f, Z_ITEM, bShadow
		);
	}
}

void GoalPost::OnChangeState(int nState)
{
	ChangeStateInfo *pInfo = nullptr;
	if (m_ChangeStateInfo.nNextState != -1)
		pInfo = &m_ChangeStateInfo;

	switch (nState)
	{
	case STATE_TRAMPOLINPRESS:
	case STATE_TRAMPOLINJUMP:
		return;
		break;
	default:
		EventObjectBase::OnChangeShape(nState);
		return;
		break;
	}
}

void GoalPost::OnIdle()
{
	if (m_fLineSpeed < 0 && m_fLineHeight < -8 * TILE_YS)
	{
		m_fLineSpeed *= -1;
	}
	
	if (m_fLineSpeed > 0 && m_fLineHeight > 0)
	{
		m_fLineSpeed *= -1;
	}

	m_fLineHeight += m_fLineSpeed;
}

void GoalPost::OnFlagDown()
{
	if (m_nStateFrame > 0)
		m_fLineHeight += 2;

	if (m_fLineHeight >= 128)
	{
		ChangeState(STATE_FLAGSTOP);
	}
}

bool GoalPost::CanApplied()
{
	if (m_nState != STATE_IDLE)
		return false;

	return true;
}

SIZE GoalPost::GetSize()
{
	SIZE s = {
		48,
		9 * TILE_YS
	};

	return s;
}
