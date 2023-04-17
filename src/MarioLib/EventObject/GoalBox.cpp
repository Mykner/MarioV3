#include "GoalBox.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "FModManager.h"
#include "Stage.h"
#include "SoundManager.h"

GoalBox::GoalBox(Game *pGame, Stage *pStage) :
	EventObjectBase(pGame, pStage)
{
	m_nType = EVENTOBJECT_GOALBOX;
	m_nState = STATE_IDLE;
	RequestSprites(pStage);

	m_nItemIdx = ITEM_MUSHROOM;
	m_fItemX = -1;
	m_fItemY = -1;
	m_fItemYS = 0;
}

GoalBox::~GoalBox()
{
}

void GoalBox::RequestSprites(Stage * pStage)
{
	EventObjectBase::RequestSprites(pStage);

	if (pStage == nullptr)
		pStage = CUR_STAGE;

	m_ppSpriteItem = &pStage->m_pSpriteItem;
}

void GoalBox::Process()
{
	switch (m_nState)
	{
	case STATE_IDLE:
		OnIdle();
		break;
	case STATE_FLY:
		OnFly();
		break;
	}
	
	if (CanApplied() && m_bCollisionPlayer)
	{
		CUR_PLAYER_OBJ->OnApplyEvent(this);
		ChangeState(STATE_FLY);

		CreateParameter param;
		switch (m_nItemIdx)
		{
		case ITEM_MUSHROOM:		param.nScore = 100; break;
		case ITEM_FIREFLOWER:	param.nScore = 1000; break;
		case ITEM_SUPERSTAR:	param.nScore = 4000; break;
		case ITEM_1UP:			param.nScore = 10000; break;
		}

		param.nType = EFFECT_SCORE;
		param.pFromObject = this;
		param.pFromPlayer = CUR_PLAYER;
		param.fYS = -0.5f;
		m_pStage->CreateEffect(m_fX + 12, m_fY - 16, EFFECT_SCORE, &param);
	}
}

void GoalBox::OnBeforeProcess()
{
	//CheckCollision();
	CheckCollisionPlayer();
}

void GoalBox::OnAfterProcess()
{
	m_fItemY += m_fItemYS;

	EventObjectBase::OnAfterProcess();
}

void GoalBox::Render(int nColor, int nZOrder)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	float _x = x - pt.x;
	float _y = y - pt.y;

	bool bShadow = false;
	if (m_pGame->m_bModernStyle)
		bShadow = true;

	// Box
	(*m_ppSprite)->RenderToQueue(
		_x,
		_y,
		SPRIDX_MAPOBJ_GOALBOX,
		false, false, nColor, 1.0f, 1.0f, Z_BEHIND_MAP, bShadow
	);

	// Item
	float fScaleX = 1.0f;
	float fScaleY = 1.0f;
	if (m_nState == STATE_FLY)
	{
		if (m_nStateFrame < 150)
		{
			switch (m_nStateFrame % 9)
			{
			case 1: fScaleX = 0.85f; break;
			case 2: fScaleX = 0.65f; break;
			case 3: fScaleX = 0.25f; break;
			case 4: fScaleX = 0.01f; break;
			case 5: fScaleX = 0.25f; break;
			case 6: fScaleX = 0.55f; break;
			case 7: fScaleX = 0.85f; break;
			}

			nColor = 0x70ffffff;
		}
		else
		{
			fScaleX = fScaleY = 2.0f;
		}
	}
	else if (m_nState == STATE_EVENTDEAD)
	{
		fScaleX = fScaleY = 2.0f;
	}

	int nItemFrame = SPRIDX_MUSHROOM;
	switch (m_nItemIdx)
	{
	case ITEM_MUSHROOM: nItemFrame = SPRIDX_MUSHROOM; break;
	case ITEM_FIREFLOWER: nItemFrame = SPRIDX_FLOWER; break;
	case ITEM_SUPERSTAR: nItemFrame = SPRIDX_STAR; break;
	case ITEM_1UP: nItemFrame = SPRIDX_MUSHROOM1UP; break;
	}

	float fItemX = m_fItemX;
	float fItemY = m_fItemY;
	if (m_fItemX == -1 && m_fItemY == -1)
	{
		fItemX = m_fX;
		fItemY = m_fY - 6;
	}

	(*m_ppSpriteItem)->RenderToQueue(
		fItemX - pt.x,
		fItemY - pt.y,
		nItemFrame,
		false, false, nColor, fScaleX, fScaleY, Z_EFFECT, bShadow
	);
}

void GoalBox::OnChangeState(int nState)
{
	ChangeStateInfo *pInfo = nullptr;
	if (m_ChangeStateInfo.nNextState != -1)
		pInfo = &m_ChangeStateInfo;

	switch (nState)
	{
	case STATE_FLY:
		m_fItemX = m_fX;
		m_fItemY = m_fY - 6;
		m_fItemYS = -1.0f;

		EventObjectBase::OnChangeState(nState);
		return;
		break;
	case STATE_EVENTDEAD:
		EventObjectBase::OnChangeState(nState);
		return;
		break;
	default:
		return;
		break;
	}
}

void GoalBox::OnIdle()
{
	m_fItemX = m_fX;
	m_fItemY = m_fY - 6;

	switch ((m_nStateFrame / 6) % 4)
	{
	case 0: m_nItemIdx = ITEM_MUSHROOM; break;
	case 1: m_nItemIdx = ITEM_FIREFLOWER; break;
	case 2: m_nItemIdx = ITEM_SUPERSTAR; break;
	case 3: m_nItemIdx = ITEM_1UP; break;
	}
}

void GoalBox::OnFly()
{
	if (m_fItemYS < 0)
	{
		if (rand() % 4 > 0)
		{
			for (int i = 0; i < (rand() % 4); i++)
			{
				NaRect rc = GetRect();
				rc.Offset(0, (m_fItemY - m_fY));
				rc.left -= 8;
				rc.top -= 8;
				rc.right += 8;
				rc.bottom += 8;
				int nX = rc.left + (rand() % rc.Width());
				int nY = rc.top + (rand() % rc.Height());
				CreateParameter param;
				param.nType = (rand() % 2 == 0) ? EFFECT_GOLDTWINKLE : EFFECT_TWINKLE;
				CUR_STAGE->CreateEffect(nX, nY, EFFECT_TWINKLE, &param);
			}
		}
	}

	if (m_nStateFrame >= 100)
	{
		m_fItemYS += 0.05f;
	}

	if (m_nStateFrame == 150)
	{
		m_fItemYS = 0;
		ChangeState(STATE_EVENTDEAD);
	}
}

bool GoalBox::CanApplied()
{
	if (m_nState != STATE_IDLE)
		return false;

	return true;
}

SIZE GoalBox::GetSize()
{
	SIZE s = { 24, 24 };

	return s;
}
