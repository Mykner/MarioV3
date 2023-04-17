#include "ThrowingHammer.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "SoundManager.h"

ThrowingHammer::ThrowingHammer(Game *pGame, Stage *pStage) :
	BulletObjectBase(pGame, pStage)
{
	m_nType = BULLET_THROWINGHAMMER;

	m_fXS = 0;
	m_fYS = 0;
}

ThrowingHammer::~ThrowingHammer()
{
}

void ThrowingHammer::Process()
{
	if (m_nState == STATE_DEAD)
		return;

	switch(m_nState)
	{
	case STATE_IDLE:
		OnIdle();
		break;
	case STATE_WALK:
		OnWalk();
		break;
    case STATE_BURNT:
        OnBurnt();
        break;
	}

	if (IsOutOfStage() && m_pOwner == nullptr)
		ChangeState(STATE_DEAD);

	if (IsAlive())
	{
		if (m_vecOverlappedEnemy.size() > 0)
			OnDamageEnemy();
		if (m_bCollisionPlayer)
			OnDamagePlayer();
	}

	m_nLifeFrame++;
}

void ThrowingHammer::OnBeforeProcess()
{
	if (m_nNextState != -1)
	{
		// Affected by other
		m_nState = m_nNextState;
		m_nNextState = -1;
		m_nStateFrame = 0;
	}

	if (m_nDamageGroup == DAMAGEGROUP_PLAYER)
		CheckCollisionEnemy();
	else
		CheckCollisionPlayer();
}

void ThrowingHammer::ChangeState(int nState, ChangeStateInfo *pInfo)
{
	switch (nState)
	{
	case STATE_WALK:
		m_fXS = 1.5f;
		if (m_bFlip)
			m_fXS *= -1;
		m_fYS = -3.0f;

		if (m_fScaleX > 1.0f)
		{
			m_fXS *= (1.0f + ((m_fScaleX - 1.0f) / 5.0f));
		}
		m_pOwner = nullptr;
		break;
    case STATE_BURNT:
        if (m_pGame->m_bModernStyle)
            m_bFlipV = false;
        else
            m_bFlipV = true;
            
        m_fYS = -2.0f;
        if (pInfo && pInfo->pFromObject)
        {
			if (pInfo->pFromObject->m_fX < m_fX)
			{
				m_fXS = 1.0f;
				m_bFlip = !m_bFlip;
			}
			else
			{
				m_fXS = -1.0f;
				m_bFlip = !m_bFlip;
			}
        }

        break;
	}
	GameObjectBase::ChangeState(nState);
}

void ThrowingHammer::OnIdle()
{
	// Do Nothing
}

void ThrowingHammer::OnWalk()
{
	if (m_fYS < 4.0f || !m_bCollision[COLLISION_BOTTOM])
	{
		if (m_fYS < 2.0f)
			OnApplyGravity(0.3f, 4.0f);
		else
			OnApplyGravity(0.6f, 4.0f);
	}
}

void ThrowingHammer::OnBurnt()
{
    switch (m_nStateFrame)
    {
        case 2:
        case 7:
        case 11:
        case 15:
        case 19:
        case 23:
            m_fYS += 1.0f;
            break;
    }
    
    if (IsOutOfCamera())
        ChangeState(STATE_DEAD);

}

void ThrowingHammer::OnDamageEnemy()
{
	std::vector<GameObjectBase*>::iterator it = m_vecOverlappedEnemy.begin();
	for (; it != m_vecOverlappedEnemy.end(); ++it)
	{
		GameObjectBase *pEnemy = *it;
        
        // #TODO Fix
        if (!pEnemy->CanBurnt())
        {
            ChangeState(STATE_BURNT);

			SOUND_MANAGER->PlaySoundEffect(L"Kicked");
            return;
        }
		if (pEnemy->IsAlive() && pEnemy->CanBurnt())
		{
			// Damage All Target
			ChangeStateInfo info;
			info.nNextState = STATE_BURNT;
			info.pFromObject = this;
			pEnemy->ChangeState(STATE_BURNT, &info);

			if (m_pOwner && m_pOwner->m_pController)
			{
				CreateParameter param;
				param.nScore = 100;
				param.nType = EFFECT_SCORE;
				param.pFromObject = this;
				param.pFromPlayer = m_pOwner->m_pController;
				param.fYS = -1.0f;
				CUR_STAGE->CreateEffect(m_fX, m_fY - 16, EFFECT_SCORE, &param);
			}
		}
	}
}

void ThrowingHammer::OnDamagePlayer()
{
	GameObjectBase *pEnemy = CUR_PLAYER_OBJ;
	if (pEnemy->IsAlive())
	{
        if (pEnemy->CanBurnt())
        {
            ChangeStateInfo info;
            info.nNextState = STATE_BURNT;
            info.pFromObject = this;

			pEnemy->ChangeState(STATE_BURNT, &info);
        }
        else
        {
            // #TODO Burnt + invert flip
            ChangeStateInfo info;
            info.pFromObject = pEnemy;
            ChangeState(STATE_BURNT, &info);

			SOUND_MANAGER->PlaySoundEffect(L"Kicked");
            return;
        }
	}
}

bool ThrowingHammer::CanEat()
{
	return true;
}

bool ThrowingHammer::CanSpit()
{
	return true;
}

SIZE ThrowingHammer::GetSize()
{
	SIZE s = { 8, 8 };
	return s;
}

void ThrowingHammer::Render(int nColor, int nZOrder)
{
	if (m_nState == STATE_DEAD)
		return;

	NaPointT<float> pt = CAMERA->GetPosition();
	int nFrame = GetSpriteIndex();

	if (m_pGame->m_bModernStyle)
	{
		bool bShadow = true;
		float fAngle = 0;
		if (m_fXS != 0 && m_fYS != 0)
		{
			if (m_fScaleX > 1)
				fAngle = m_nStateFrame * 16.0f;
			else
				fAngle = m_nStateFrame * 22.5f;
		}

		m_pSprite->RenderToQueue(x - pt.x, y - pt.y, nFrame, m_bFlip, m_bFlipV, nColor,
			m_fScaleX, m_fScaleY, Z_BULLET, bShadow, fAngle);
	}
	else
	{
		m_pSprite->RenderToQueue(x - pt.x, y - pt.y, nFrame, m_bFlip, m_bFlipV, nColor,
			m_fScaleX, m_fScaleY, Z_BULLET);
	}	
}

int ThrowingHammer::GetSpriteIndex()
{
	if (m_fXS == 0 && m_fYS == 0)
		return SPRIDX_BULLET_THROWINGHAMMER1;

	if (IS_MODERN)
		return SPRIDX_BULLET_MODERN_THROWINGHAMMER;
	
	int nFrame = SPRITE_INDEX_CYCLE(SPRIDX_BULLET_THROWINGHAMMER1, 4, 4);
	return nFrame;
}
