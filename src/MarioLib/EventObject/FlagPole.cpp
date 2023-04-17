#include "FlagPole.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "FModManager.h"
#include "Stage.h"
#include "SoundManager.h"

FlagPole::FlagPole(Game *pGame, Stage *pStage) :
	EventObjectBase(pGame, pStage)
{
	m_nType = EVENTOBJECT_FLAGPOLE;
	m_nState = STATE_IDLE;

	m_nFlagHeight = 0;
}

FlagPole::~FlagPole()
{
}

void FlagPole::Process()
{
	switch (m_nState)
	{
	case STATE_FLAGDOWN:
		OnFlagDown();
		break;
	}
	
	if (CanApplied() && m_bCollisionPlayer)
	{
		CUR_PLAYER_OBJ->OnApplyEvent(this);
		ChangeState(STATE_FLAGDOWN);

		CreateParameter param;

		int nFlagTop = GetRect().top;
		if (CUR_PLAYER_OBJ->m_nShape == SHAPE_GIANT)
			param.nScore = 10000;
		else
		{
			int nY = (CUR_PLAYER_OBJ->m_fY - nFlagTop) / 16;
			if (nY < 1)
				param.nScore = 5000;
			else if (nY < 3)
				param.nScore = 2000;
			else if (nY < 4)
				param.nScore = 800;
			else if (nY < 6)
				param.nScore = 400;
			else if (nY < 8)
				param.nScore = 200;
			else
				param.nScore = 100;
		}

		param.nType = EFFECT_SCORE;
		param.pFromObject = this;
		param.pFromPlayer = CUR_PLAYER;
		param.fYS = -2.0f;
		m_pStage->CreateEffect(m_fX + 12, m_fY - 16, EFFECT_SCORE, &param);
	}
}

void FlagPole::OnBeforeProcess()
{
	//CheckCollision();
	CheckCollisionPlayer();
}

void FlagPole::Render(int nColor, int nZOrder)
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
			_x,
			_y - (i * TILE_YS),
			SPRIDX_MAPOBJ_FLAGPOLE_BODY,
			false, false, nColor, 1.0f, 1.0f, Z_BEHIND_MAP, bShadow
		);
	}

	// Head
	(*m_ppSprite)->RenderToQueue(
		_x,
		_y - (9 * TILE_YS),
		SPRIDX_MAPOBJ_FLAGPOLE_HEAD,
		false, false, nColor, 1.0f, 1.0f, Z_BEHIND_MAP + 1, bShadow
	);

	// Flag
	int nFlagIndex = SPRITE_INDEX_CYCLE(SPRIDX_MAPOBJ_FLAGPOLE_FLAG, 3, 6);
	(*m_ppSprite)->RenderToQueue(
		_x,
		_y - (8 * TILE_YS) + m_nFlagHeight,
		nFlagIndex,
		false, false, nColor, 1.0f, 1.0f, Z_BEHIND_MAP, bShadow
	);
}

void FlagPole::OnChangeState(int nState)
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
		EventObjectBase::OnChangeState(nState);
		return;
		break;
	}
}

void FlagPole::OnFlagDown()
{
	if (m_pGame->m_nStagePackType != STAGEPACK_CLASSIC)
	{
		if (m_nStateFrame == 0)
			SOUND_MANAGER->PlaySoundEffect(L"FlagDown");

		if (m_nStateFrame > 0)
			m_nFlagHeight += 2;

		if (m_nFlagHeight >= 128)
		{
			ChangeState(STATE_FLAGSTOP);
		}
	}
	else
	{
		if (m_nStateFrame == 30)
			SOUND_MANAGER->PlaySoundEffect(L"FlagDown");

		if (m_nStateFrame > 30)
			m_nFlagHeight += 2;

		if (m_nFlagHeight >= 128)
		{
			ChangeState(STATE_FLAGSTOP);
		}
	}
}

bool FlagPole::CanApplied()
{
	if (m_nState != STATE_IDLE)
		return false;

	return true;
}

SIZE FlagPole::GetSize()
{
	SIZE s = {
		2,
		9 * TILE_YS
	};

	if (m_pStage->m_bIsDesignStage)
		s.cx = 16;

	return s;
}
