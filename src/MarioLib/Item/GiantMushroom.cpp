#include "GiantMushroom.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "FModManager.h"
#include "SoundManager.h"

GiantMushroom::GiantMushroom(Game *pGame, Stage *pStage) :
	Mushroom(pGame, pStage)
{
	m_nType = ITEM_GIANTMUSHROOM;
	m_nState = STATE_ITEMPOP;

	m_fScaleX = 2.0f;
	m_fScaleY = 2.0f;
}

GiantMushroom::~GiantMushroom()
{
}

void GiantMushroom::Process()
{
	switch (m_nState)
	{
	case STATE_WALK:
		if (m_nStateFrame == 0)
		{
			if (!m_bFlip)
				m_fXS = 0.5f;
			else
				m_fXS = -0.5f;
		}
		break;
	case STATE_ITEMPOP:
		if (m_nStateFrame == 0)
		{
			SOUND_MANAGER->PlaySoundEffect(L"ItemPop");
		}

		OnItemPop();
		break;
	case STATE_ITEMPOPDOWN:
		if (m_nStateFrame == 0)
		{
			SOUND_MANAGER->PlaySoundEffect(L"ItemPop");
		}

		OnItemPopDown();
		break;
	}
	ProcessBase();

	if (CanApplied() && m_bCollisionPlayer)
	{
		CUR_PLAYER_OBJ->OnApplyItem(this);
		SetState(STATE_DEAD);
	}
}

void GiantMushroom::OnItemPop()
{
	if (m_nStateFrame == 30)
		m_fYS = -0.5f;

	if (m_nStateFrame == 62)
	{
		ChangeState(m_nBaseState);
	}
}

int GiantMushroom::GetSpriteIndex()
{
	if (m_nState == STATE_ITEMPOP && m_nStateFrame < 30)
		return -1;

	if (m_nState == STATE_ITEMPOPDOWN && m_nStateFrame < 4)
		return -1;

	return SPRIDX_MUSHROOM;
}

void GiantMushroom::GetScale(float & fScaleX, float & fScaleY)
{
	fScaleX = m_fScaleX;
	fScaleY = m_fScaleY;

	// We need easing function.
	// from: 1;
	// to: m_fScaleX;
	// Math.easeOutSine = function(t, b, c, d) {
	//    return c * Math.sin(t / d * (Math.PI / 2)) + b;
	// };

	float b = 0, d = 0, t = 0, c = 0;
	if (m_nState == STATE_ITEMPOP && m_nStateFrame >= 30)
	{
		SIZE s = GetSize();
		if (s.cx > 16)
		{
			b = 1.0f; // start
			d = 32; // duration
			t = m_nStateFrame - 32; // cur time
			c = (m_fScaleX - 1.0f); // increase per time
		}
	}

	if (m_nState == STATE_THROWN && m_nStateFrame < 32)
	{
		SIZE s = GetSize();
		if (s.cx > 16)
		{
			b = 1.0f; // start
			d = 32; // duration
			t = m_nStateFrame; // cur time
			c = (m_fScaleX - 1.0f); // increase per time
		}
	}

	if (m_nState == STATE_ITEMPOPDOWN && m_nStateFrame >= 4)
	{
		SIZE s = GetSize();
		if (s.cx > 16)
		{
			b = 1.0f; // start
			d = 4; // duration
			t = m_nStateFrame - 4; // cur time
			c = (m_fScaleX - 1.0f); // increase per time
		}
	}

	if (b != 0 && d != 0 && c != 0)
	{
		float fVal = c * sin(t / d * (PI / 2)) + b;

		fScaleX = fScaleY = fVal;
	}
}
