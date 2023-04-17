#include "Grinder.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "SoundManager.h"

BEGIN_IMPL_PROPERTYMAP(Grinder)
	PROP_BOOL("PowerUp", VT_BOOL, false, nullptr, "Grinder")
END_IMPL_PROPERTYMAP()

Grinder::Grinder(Game *pGame, Stage *pStage) :
	EnemyObjectBase(pGame, pStage)
{
	m_nType = ENEMY_GRINDER;
	m_nState = STATE_IDLE;

	m_bFlip = true;
}

Grinder::~Grinder()
{
}

void Grinder::OnBeforeProcess()
{
	;
}

void Grinder::Render(int nColor, int nZOrder)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	int nFrame = GetSpriteIndex();

	if (nZOrder == -1)
		nZOrder = GetZOrder();

	if (nFrame != -1)
	{
		bool bShadow = m_pGame->m_bModernStyle && nZOrder != Z_MAP_SHADOW;
		float fAngle = (m_nStateFrame * 3.0f);

		if (IS_SPINNING)
		{
			fAngle = m_nStateFrame * -14;
			if (m_fXS < 0)
				fAngle *= -1;
			if (!m_bFlip)
				fAngle *= -1;
		}

		float fScaleX, fScaleY;
		GetScale(fScaleX, fScaleY);

		(*m_ppSprite)->RenderToQueue(x - pt.x, y - pt.y, nFrame, m_bFlip, m_bFlipV, nColor,
			fScaleX, fScaleY, nZOrder, bShadow, fAngle);
	}
}

void Grinder::ChangeState(int nState, ChangeStateInfo *pInfo)
{
	switch (nState)
	{
	case STATE_PRESSED:
		{
			int nPower = POWER_PRESS_NORMAL;
			if (pInfo && pInfo->nPower)
				nPower = pInfo->nPower;

			switch (nPower)
			{
			case POWER_PRESS_YOSHI:
				{
					OnStomped(false, pInfo->pFromObject);
					SOUND_MANAGER->PlaySoundEffect(L"YoshiStompImmune");
				}
				break;
			default:
				SOUND_MANAGER->PlaySoundEffect(L"Pressed");
				break;
			}

			// Cannot press!
			return;
		}
		break;
	}

	EnemyObjectBase::ChangeState(nState, pInfo);
}

void Grinder::OnIdle()
{
	
}

bool Grinder::IsAlive()
{
	return EnemyObjectBase::IsAlive();
}

bool Grinder::CanEat()
{
	return false;
}

bool Grinder::CanSpit()
{
	return false;
}

bool Grinder::CanPressed(int nPower)
{
	switch (nPower)
	{
	case POWER_PRESS_YOSHI:
		return true;
		break;
	}

	return false;
}

bool Grinder::CanBurnt(int nType)
{
	return false;
}

bool Grinder::CanBornFromBlock()
{
	return false;
}

bool Grinder::CanPushedUp()
{
	return false;
}

int Grinder::GetSpriteIndex()
{
	if (EnemyObjectBase::GetSpriteIndex() == -1)
		return -1;

	return SPRIDX_GRINDER;
}

int Grinder::GetZOrder()
{
	return Z_AFTER_MAP;
}

SIZE Grinder::GetSize()
{
	SIZE s;
	s.cx = 36 * m_fScaleX;
	s.cy = 36 * m_fScaleY;

	return s;
}
