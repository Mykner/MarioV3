#include "CheckPointFlag.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "FModManager.h"
#include "Stage.h"
#include "TypeNames.h"
#include "SoundManager.h"

#include "SpeechBubble.h"

CheckPointFlag::CheckPointFlag(Game *pGame, Stage *pStage) :
	EventObjectBase(pGame, pStage)
{
	m_nType = EVENTOBJECT_CHECKPOINT_FLAG;
	m_nState = STATE_IDLE;

	m_bChecked = false;
	m_nShape = 0;
}

CheckPointFlag::~CheckPointFlag()
{
}

void CheckPointFlag::Process()
{
	if (m_nShape == 0 && m_bCollisionPlayer)
	{
		m_nShape = 1;
		m_nStateFrame = 0;

		CUR_PLAYER_OBJ->OnApplyEvent(this);

		if (!IS_CLASSIC)
		{
			SOUND_MANAGER->PlaySoundEffect(L"CheckPoint");
		}
	}
	else if (m_nShape == 1)
	{
		if (m_nStateFrame == 5)
			m_nShape = 2;
	}
}

void CheckPointFlag::OnBeforeProcess()
{
	CheckCollisionPlayer();
}

void CheckPointFlag::OnAfterProcess()
{
	m_nStateFrame++;
}

void CheckPointFlag::Render(int nColor, int nZOrder)
{
	if (IS_CLASSIC)
		return;

	NaPointT<float> pt = CAMERA->GetPosition();
	float _x = x - pt.x;
	float _y = y - pt.y;
	bool bShadow = IS_MODERN;
	int nFrame = GetSpriteIndex();

	(*m_ppSprite)->RenderToQueue(_x, _y, nFrame,
		false, false, nColor, 1.0f, 1.0f, Z_MAP, bShadow);
}

SIZE CheckPointFlag::GetSize()
{
	SIZE s = { 16, 32 };
	if (IS_CLASSIC)
	{
		s.cy = GameDefaults::nPageHeight;
	}

	return s;
}

int CheckPointFlag::GetSpriteIndex()
{
	switch (m_nShape)
	{
	case 0:
		return SPRITE_INDEX_CYCLE(SPRIDX_MAPOBJ_CHECKPOINT1, 3, 4);
		break;
	case 1:
		return SPRITE_INDEX_CYCLE(SPRIDX_MAPOBJ_CHECKPOINT_HIT1, 3, 1);
		break;
	case 2:
		return SPRITE_INDEX_CYCLE(SPRIDX_MAPOBJ_CHECKPOINT_CHECKED1, 4, 4);
		break;
	}

	return -1;
}
