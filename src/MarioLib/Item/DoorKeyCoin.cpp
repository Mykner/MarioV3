#include "DoorKeyCoin.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "FModManager.h"
#include "Stage.h"
#include "SoundManager.h"

DoorKeyCoin::DoorKeyCoin(Game *pGame, Stage *pStage) :
	ItemObjectBase(pGame, pStage)
{
	m_nType = ITEM_DOORKEYCOIN;
	m_nState = STATE_IDLE;

	m_bIsLastKey = false;
}

DoorKeyCoin::~DoorKeyCoin()
{
}

void DoorKeyCoin::ChangeState(int nState, ChangeStateInfo * pInfo)
{
	switch(nState)
	{
	case STATE_PUSHEDUP:
		nState = STATE_JUMP;
		
		if (pInfo && pInfo->pFromObject)
		{
			pInfo->pFromObject->OnApplyItem(m_nType);
		}
		break;
	default:
		ItemObjectBase::ChangeState(nState, pInfo);
		return;
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

void DoorKeyCoin::OnChangeState(int nState)
{
	ChangeStateInfo *pInfo = nullptr;
	if (m_ChangeStateInfo.nNextState != -1)
		pInfo = &m_ChangeStateInfo;

	switch (nState)
	{
	case STATE_JUMP:
		{
			m_fYS = -4.0f;
		}
		break;
	}
}

void DoorKeyCoin::Process()
{
	switch (m_nState)
	{
	case STATE_PUSHEDUP:
		OnPushedUp();
		break;
	case STATE_JUMP:
		OnJump();
		break;
	}

	if (rand() % 48 == 0)
	{
		NaRect rc = GetRect();
		rc.left -= 8;
		rc.top -= 8;
		rc.right += 8;
		rc.bottom += 8;
		int nX = rc.left + (rand() % rc.Width());
		int nY = rc.top + (rand() % rc.Height());
		CUR_STAGE->CreateEffect(nX, nY, EFFECT_TWINKLE);
	}

	if (CanApplied() && m_bCollisionPlayer)
	{
		CUR_PLAYER_OBJ->OnApplyItem(this);
		
		Stage *pTopStage = CUR_STAGE->GetTopStage();
		NaDebugOut(L"CurKeyCoin: %d / Max: %d\n", CUR_PLAYER->m_nDoorKeyCoin, pTopStage->m_nMaxDoorKey);

		if (CUR_PLAYER->m_nDoorKeyCoin == pTopStage->m_nMaxDoorKey)
		{
			m_bIsLastKey = true;
		}
		ChangeState(STATE_JUMP);
	}
}

void DoorKeyCoin::OnBeforeProcess()
{
	CheckCollisionPlayer();
}

void DoorKeyCoin::OnJump()
{
	m_fYS += 0.2f;

	if (m_nStateFrame == 38)
	{
		if (m_bIsLastKey)
		{
			GameObjectBase *pKey = CUR_STAGE->ReserveCreateGameObject(m_fX, m_fY, ITEM_DOORKEY);
			if (pKey)
			{
				ChangeStateInfo info;
				info.pFromObject = CUR_PLAYER_OBJ;
				pKey->SetState(STATE_FOLLOWPREPARE);
				pKey->ChangeState(STATE_FOLLOWPREPARE, &info);

				SOUND_MANAGER->PlaySoundEffect(L"DoorKeyAppear");
			}
		}
		ChangeState(STATE_DEAD);
	}

	if (m_bIsLastKey)
	{
		POINT pt = m_ptEffectBase;
		switch (m_nStateFrame)
		{
		case 0:
			pt = m_ptEffectBase = { (int)m_fX, (int)m_fY };

			CUR_STAGE->CreateEffect(pt.x + 0, pt.y + 0, EFFECT_TWINKLE);
			CUR_STAGE->CreateEffect(pt.x - 8, pt.y - 4, EFFECT_TWINKLE);
			CUR_STAGE->CreateEffect(pt.x + 8, pt.y - 4, EFFECT_TWINKLE);
			CUR_STAGE->CreateEffect(pt.x - 8, pt.y - 12, EFFECT_TWINKLE);
			CUR_STAGE->CreateEffect(pt.x + 8, pt.y - 12, EFFECT_TWINKLE);
			CUR_STAGE->CreateEffect(pt.x + 0, pt.y - 16, EFFECT_TWINKLE);
			break;
		case 8:
			CUR_STAGE->CreateEffect(pt.x + 0, pt.y + 4, EFFECT_TWINKLE);
			CUR_STAGE->CreateEffect(pt.x - 11, pt.y - 1, EFFECT_TWINKLE);
			CUR_STAGE->CreateEffect(pt.x + 11, pt.y - 1, EFFECT_TWINKLE);
			CUR_STAGE->CreateEffect(pt.x - 11, pt.y - 15, EFFECT_TWINKLE);
			CUR_STAGE->CreateEffect(pt.x + 11, pt.y - 15, EFFECT_TWINKLE);
			CUR_STAGE->CreateEffect(pt.x + 0, pt.y - 20, EFFECT_TWINKLE);
			break;
		case 16:
			CUR_STAGE->CreateEffect(pt.x + 0, pt.y + 8, EFFECT_TWINKLE);
			CUR_STAGE->CreateEffect(pt.x - 14, pt.y + 2, EFFECT_TWINKLE);
			CUR_STAGE->CreateEffect(pt.x + 14, pt.y + 2, EFFECT_TWINKLE);
			CUR_STAGE->CreateEffect(pt.x - 14, pt.y - 18, EFFECT_TWINKLE);
			CUR_STAGE->CreateEffect(pt.x + 14, pt.y - 18, EFFECT_TWINKLE);
			CUR_STAGE->CreateEffect(pt.x + 0, pt.y - 24, EFFECT_TWINKLE);
			break;
		}
	}
}

void DoorKeyCoin::OnPushedUp()
{
	CUR_PLAYER_OBJ->OnApplyItem(this);
	SetState(STATE_DEAD);
}

bool DoorKeyCoin::IsOutOfCamera()
{
	switch (m_nState)
	{
	case STATE_PUSHEDUP:
	case STATE_JUMP:
		return false;
		break;
	}

	return CAMERA->IsOutOfCamera(this);
}

bool DoorKeyCoin::CanApplied()
{
	if (m_nState == STATE_DEAD)
		return false;

	if (m_nState == STATE_HELDBYTONGUE)
		return false;

	if (m_nState == STATE_JUMP)
		return false;

	return true;
}

int DoorKeyCoin::GetSpriteIndex()
{
	int nMax, nFrame;
	if (m_nState == STATE_JUMP)
	{
		nMax = (m_pGame->m_nGameFrame % 16);
		nFrame = (nMax / 4);
	}
	else
	{
		nMax = (m_pGame->m_nGameFrame % 32);
		nFrame = (nMax / 8);
	}

	int nFrames[] = { 0, 1, 2, 1 };
	return SPRIDX_KEY_COIN + nFrames[nFrame];
}
