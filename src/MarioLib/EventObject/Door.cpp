#include "Door.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "FModManager.h"
#include "Stage.h"
#include "TypeNames.h"
#include "SoundManager.h"

#include "SpeechBubble.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(Door)
	PROP_INT("PairIndex", VT_I4, 0, nullptr, "Door"),
	PROP_BOOL("Locked", VT_BOOL, false, nullptr, "Door"),
END_IMPL_PROPERTYMAP()

Door::Door(Game *pGame, Stage *pStage) :
	EventObjectBase(pGame, pStage)
{
	m_nType = EVENTOBJECT_DOOR;
	m_nState = STATE_IDLE;

	m_nPairIndex = 0;
	m_bLocked = false;
	m_nSubType = 0;
}

Door::~Door()
{
}

void Door::ChangeState(int nState, ChangeStateInfo * pInfo)
{
	switch (nState)
	{
	case STATE_OPEN:
		break;
	case STATE_DENY:
		SOUND_MANAGER->PlaySoundEffect(L"DoorLocked");
		break;
	case STATE_UNLOCK:
		SOUND_MANAGER->PlaySoundEffect(L"DoorUnlock");
		break;
	case STATE_PUSHEDUP:
		return;
		break;
	}

	m_nNextState = nState;
}

void Door::Process()
{
	switch(m_nState)
	{
	case STATE_OPEN:
		break;
	case STATE_CLOSE:
		break;
	case STATE_DENY:
		if (m_nStateFrame == 24)
			ChangeState(STATE_IDLE);
		break;
	case STATE_UNLOCK:
		m_bLocked = false;
		if (m_nStateFrame == 1)
		{
			m_nSubType = 1;
			Door *pPairDoor = (Door*)m_pGame->m_pGameState->FindPairDoor(this);
			if (pPairDoor)
			{
				pPairDoor->m_bLocked = false;
				pPairDoor->m_nSubType = 1;
			}

			// Effects
			NaRect rc = GetRect();
			CreateParameter param;
			param.fXS = -0.3f;
			param.fYS = -0.3f;
			CUR_STAGE->CreateEffect(
				m_fX - rc.Width() / 2, 
				m_fY - rc.Height(), EFFECT_STARDUST, &param);

			param.fXS = 0.3f;
			param.fYS = -0.3f;
			CUR_STAGE->CreateEffect(
				m_fX + rc.Width() / 2, 
				m_fY - rc.Height(), EFFECT_STARDUST, &param);
		}

		if (m_nStateFrame == 24)
			ChangeState(STATE_OPEN);

		break;
	}
}

void Door::OnBeforeProcess()
{
}

void Door::OnAfterProcess()
{	
	EventObjectBase::OnAfterProcess();
}

void Door::Render(int nColor, int nZOrder)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	float _x = x - pt.x;
	float _y = y - pt.y;
	bool bShadow = IS_MODERN;

	float fAngle = 0.0f;
	if (m_nState == STATE_DENY)
	{
		fAngle = (rand() % 10) - 5;
	}

	int nFrame = GetSpriteIndex();
	float fScaleX, fScaleY;
	GetScale(fScaleX, fScaleY);

	(*m_ppSprite)->RenderToQueue(_x, _y, nFrame,
		false, false, nColor, fScaleX, fScaleY, Z_DOOR, bShadow, fAngle);
}

int Door::GetSpriteIndex()
{
	int nFrame = SPRIDX_MAPOBJ_DOOR1;

	switch (m_nState)
	{
	case STATE_IDLE:
	default:
		if (m_bLocked)
			nFrame = SPRIDX_MAPOBJ_KEYDOOR_LOCKED;
		else
		{
			if (m_nSubType == 0)
				nFrame = SPRIDX_MAPOBJ_DOOR1;
			else
				nFrame = SPRIDX_MAPOBJ_KEYDOOR1;
		}
		break;
	case STATE_OPEN:
		if (m_nSubType == 0)
		{
			if (m_nStateFrame < 6)
				nFrame = SPRIDX_MAPOBJ_DOOR2;
			else
				nFrame = SPRIDX_MAPOBJ_DOOR3;
		}
		else
		{
			if (m_nStateFrame < 6)
				nFrame = SPRIDX_MAPOBJ_KEYDOOR2;
			else
				nFrame = SPRIDX_MAPOBJ_KEYDOOR3;
		}
		break;
	case STATE_CLOSE:
		if (m_nSubType == 0)
		{
			if (m_nStateFrame < 6)
				nFrame = SPRIDX_MAPOBJ_DOOR3;
			else if (m_nStateFrame < 12)
				nFrame = SPRIDX_MAPOBJ_DOOR2;
			else
				nFrame = SPRIDX_MAPOBJ_DOOR1;
		}
		else
		{
			if (m_nStateFrame < 6)
				nFrame = SPRIDX_MAPOBJ_KEYDOOR3;
			else if (m_nStateFrame < 12)
				nFrame = SPRIDX_MAPOBJ_KEYDOOR2;
			else
				nFrame = SPRIDX_MAPOBJ_KEYDOOR1;
		}
		break;
	}

	return nFrame;
}

void Door::GetScale(float & fScaleX, float & fScaleY)
{
	fScaleX = m_fScaleX;
	fScaleY = m_fScaleY;

	if (m_nState == STATE_UNLOCK)
	{
		float fScale = 1.0f;
		switch (m_nStateFrame)
		{
		case 1:	fScale = 1.1f; break;
		case 2:	fScale = 1.2f; break;
		case 3:	fScale = 1.3f; break;
		case 4:	fScale = 1.4f; break;

		case 5:	fScale = 1.4f; break;
		case 6:	fScale = 1.3f; break;
		case 7:	fScale = 1.2f; break;
		case 8:	fScale = 1.1f; break;
		}

		fScaleX = fScale;
		fScaleY = fScale;
	}
}

SIZE Door::GetSize()
{
	SIZE s = { 16, 32 };
	return s;
}
