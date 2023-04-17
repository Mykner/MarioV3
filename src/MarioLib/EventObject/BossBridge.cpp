#include "BossBridge.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "FModManager.h"
#include "Stage.h"
#include "SoundManager.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(BossBridge)
	PROP_INT("Length", VT_I4, 5, nullptr, ""),
END_IMPL_PROPERTYMAP()

BossBridge::BossBridge(Game *pGame, Stage *pStage) :
	EventObjectBase(pGame, pStage)
{
	m_nType = EVENTOBJECT_BOSSBRIDGE;
	m_nState = STATE_IDLE;

	m_nLength = 5;
	m_bAnchor = true;

	m_bCanCollide = true;
	m_bCanLand = true;
}

BossBridge::~BossBridge()
{
	m_vecPassenger.clear();
}

void BossBridge::Process()
{
	switch (m_nState)
	{
	case STATE_DIE:
		OnDie();
		break;
	}
}

void BossBridge::OnBeforeProcess()
{
	//CheckCollision();
	//CheckCollisionPlayer();
}

void BossBridge::OnAfterProcess()
{
	m_fX += m_fXS;
	m_fY += m_fYS;

	if (IsAlive())
	{
		// Move with passengers 
		BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecPassenger)
			pObj->m_fX += m_fXS;
		pObj->m_fY += m_fYS;
		END_VECTOR_LOOP();
		m_vecPassenger.clear();
	}

	m_nStateFrame++;
	if (m_nNextState != -1)
	{
		m_nState = m_nNextState;
		m_nNextState = -1;
		m_nStateFrame = 0;

		OnChangeState(m_nState);
	}
}

void BossBridge::Render(int nColor, int nZOrder)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	float _x = x - pt.x;
	float _y = y - pt.y;
	bool bShadow = (m_pGame->m_bModernStyle);

	SIZE s = GetSize();
	
	for (int i = 0; i < m_nLength; i++)
	{
		(*m_ppSprite)->RenderToQueue(_x + (i * 16), _y, SPRIDX_MAPOBJ_BOSSBRIDGE,
			false, false, nColor, 1.0f, 1.0f, Z_COMMON_OBJ, bShadow);
	}

	if (m_bAnchor)
	{
		if (IS_CLASSIC)
		{
			(*m_ppSprite)->RenderToQueue(_x + ((m_nLength - 1) * 16), _y - 16, SPRIDX_MAPOBJ_BOSSBRIDGE_ANCHOR,
				false, false, nColor, 1.0f, 1.0f, Z_COMMON_OBJ, bShadow);
		}
		else
		{
			(*m_ppSprite)->RenderToQueue(_x + ((m_nLength - 1) * 16), _y - 16, SPRIDX_MAPOBJ_BOSSBRIDGE_LARGE_ANCHOR,
				false, false, nColor, 1.0f, 1.0f, Z_MAP, bShadow);

			float _x2 = _x + (m_nLength - 1) * 16 + 2;
			float _y2 = _y - 16 - 10;
			for (int i = 0; i < 5; i++)
			{
				(*m_ppSprite)->RenderToQueue(_x2, _y2, SPRIDX_MAPOBJ_BOSSBRIDGE_LARGE_ANCHOR_CHAIN,
					false, false, nColor, 1.0f, 1.0f, Z_AFTER_MAP, false);

				_x2 -= 4;
				_y2 += 2;
			}
		}
	}
}

void BossBridge::OnDie()
{
	if (m_nStateFrame == 0)
	{
		if (IS_MODERN)
		{
			float _x2 = m_fX + (m_nLength - 1) * 16 + 2;
			float _y2 = m_fY - 16 - 10;

			for (int i = 0; i < 5; i++)
			{
				CreateParameter param;
				param.fXS = (rand() % 3) - 1;
				param.fYS = -1.0f;
				param.pSprite = m_pSprite;
				param.nType = SPRIDX_MAPOBJ_BOSSBRIDGE_LARGE_ANCHOR_CHAIN;
				CUR_STAGE->CreateEffect(
					_x2,
					_y2, EFFECT_DEBRISBASE, &param);

				_x2 -= 4;
				_y2 += 2;
			}
		}

		SOUND_MANAGER->PlaySoundEffect(L"BreakBridge");
	}
	else if (m_nStateFrame % 4 == 0 && m_nLength > 0)
	{
		m_nLength--;

		if (IS_MODERN)
		{
			CreateParameter param;
			param.fXS = 0;
			param.fYS = 0.1f;
			param.pSprite = m_pSprite;
			param.nType = SPRIDX_MAPOBJ_BOSSBRIDGE;
			CUR_STAGE->CreateEffect(
				m_fX + (m_nLength * 16),
				m_fY, EFFECT_DEBRISBASE, &param);
		}
	}
}

void BossBridge::AddPassenger(GameObjectBase * pObj)
{
	m_vecPassenger.push_back(pObj);
}

NaRect BossBridge::GetRect()
{
	NaRect rc;
	SIZE s = GetSize();
	rc.left = m_fX - 8;
	rc.right = rc.left + s.cx;
	rc.top = m_fY - s.cy + 1;
	rc.bottom = m_fY;

	return rc;
}

NaRectT<float> BossBridge::GetRectFloat()
{
	NaRectT<float> rc;
	SIZE s = GetSize();
	rc.left = m_fX - 8;
	rc.right = rc.left + s.cx;
	rc.top = m_fY - s.cy + 1;
	rc.bottom = m_fY;

	return rc;
}

SIZE BossBridge::GetSize()
{
	SIZE s = {
		16 * m_nLength,
		16
	};
	return s;
}
