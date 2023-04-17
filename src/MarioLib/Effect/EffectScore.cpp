#include "EffectScore.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"

EffectScore::EffectScore(Game *pGame, Stage *pStage) :
	EffectObjectBase(pGame, pStage)
{
	m_nType = EFFECT_SCORE;
	m_nState = STATE_IDLE;

	m_nScore = 0;
	m_pPlayer = nullptr;
	m_pFlagPole = nullptr; 
	
	m_fYS = -0.5f;	
}

EffectScore::~EffectScore()
{
}

void EffectScore::ParseParameter(CreateParameter * pParam)
{
	if (pParam == nullptr)
		return;

	m_nScore = pParam->nScore;
	m_pPlayer = pParam->pFromPlayer;
	m_fYS = pParam->fYS;
	if (pParam->pFromObject && pParam->pFromObject->m_nType == EVENTOBJECT_FLAGPOLE)
	{
		m_pFlagPole = pParam->pFromObject;
		m_fYS = 0;
	}
}

void EffectScore::Process()
{
	if (m_nState == STATE_DEAD)
		return;

	if (m_nStateFrame == 0)
	{
		if (m_pPlayer)
		{
			if (m_nScore < 10000)
				m_pPlayer->AddScore(m_nScore);
			else
				m_pPlayer->AddLife();
		}
	}

	if (m_pFlagPole)
	{
		if (m_pGame->m_nStagePackType != STAGEPACK_CLASSIC)
		{
			if (m_nStateFrame == 1)
				m_fYS = -2.0f;
		}
		else if (m_nStateFrame == 32)
			m_fYS = -2.0f; 

		// #TODO improvement
		if (m_fY < m_pFlagPole->GetRect().top + 8)
			m_fYS = 0;
	}
	else
	{
		if (m_pGame->m_bModernStyle && m_nStateFrame > 20)
		{
			m_fYS = 0;
		}
			
		if (m_nStateFrame == 48)
			ChangeState(STATE_DEAD);
	}
}

void EffectScore::Render(int nColor, int nZOrder)
{
	if (m_nState == STATE_DEAD)
		return;

	// 8: 1
	// 9: 2
	// 10: 4
	// 11: 5
	// 12: 8
	// 13: 0
	// 14: 1UP

	// number width: 4px
	// 1up width: 16px
	int _x;
	if (m_pFlagPole)
		_x = m_pFlagPole->m_fX + 11;
	else
		_x = x;
	int _y = y;
	
	if (m_nScore < 1000)
		_x -= 6;
	else
		_x -= 8;

	NaPointT<float> ptCamera = CAMERA->GetPosition();
	_x -= ptCamera.x;
	_y -= ptCamera.y;

	if (m_pFlagPole == nullptr && m_pGame->m_bModernStyle)
	{
		if (m_nStateFrame > 38)
		{
			DWORD dwPercent = (m_nStateFrame - 38) * 0x19000000;
			nColor -= dwPercent;
		}
	}

	int nNumberIndex[] = { 13, 8, 9, -1, 10, 11, -1, -1, 12, -1 };
	if (m_nScore < 1000)
	{
		(*m_ppSprite)->RenderToQueue(_x + 0, _y, nNumberIndex[m_nScore / 100], false, false, nColor, 1.0f, 1.0f, Z_EFFECT);
		(*m_ppSprite)->RenderToQueue(_x + 4, _y, nNumberIndex[0], false, false, nColor, 1.0f, 1.0f, Z_EFFECT);
		(*m_ppSprite)->RenderToQueue(_x + 8, _y, nNumberIndex[0], false, false, nColor, 1.0f, 1.0f, Z_EFFECT);
	}
	else if (m_nScore < 10000)
	{
		(*m_ppSprite)->RenderToQueue(_x +  0, _y, nNumberIndex[m_nScore / 1000], false, false, nColor, 1.0f, 1.0f, Z_EFFECT);
		(*m_ppSprite)->RenderToQueue(_x +  4, _y, nNumberIndex[0], false, false, nColor, 1.0f, 1.0f, Z_EFFECT);
		(*m_ppSprite)->RenderToQueue(_x +  8, _y, nNumberIndex[0], false, false, nColor, 1.0f, 1.0f, Z_EFFECT);
		(*m_ppSprite)->RenderToQueue(_x + 12, _y, nNumberIndex[0], false, false, nColor, 1.0f, 1.0f, Z_EFFECT);
	}
	else
	{
		(*m_ppSprite)->RenderToQueue(_x, _y, 14, false, false, nColor, 1.0f, 1.0f, Z_EFFECT);
	}
}

