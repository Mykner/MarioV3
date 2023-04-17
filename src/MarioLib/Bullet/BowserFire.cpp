#include "BowserFire.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"

BowserFire::BowserFire(Game *pGame, Stage *pStage) :
	BulletObjectBase(pGame, pStage)
{
	m_nType = BULLET_BOWSERFIRE;
	m_nState = STATE_WALK;

	m_fXS = 1.0f;
	m_fYS = 0;
	m_pOwner = nullptr;
	m_nLifeFrame = 0;
	m_nTargetY = 0;
}

BowserFire::~BowserFire()
{
}

void BowserFire::ParseParameter(CreateParameter * pParam)
{
	if (pParam == nullptr)
		return;
	
	if (pParam->pFromPlayer != nullptr)
		m_nDamageGroup = DAMAGEGROUP_PLAYER;

	if (pParam->pFromObject)
	{
		m_pOwner = pParam->pFromObject;
		m_bFlip = pParam->pFromObject->m_bFlip;
	}

	if (m_bFlip)
		m_fXS *= -1;
	
	if (pParam->fScaleX != 0)
	{
		m_fScaleX = pParam->fScaleX;
		m_fXS *= m_fScaleX;
	}
	
	if (pParam->fScaleY != 0)
		m_fScaleY = pParam->fScaleY;
}

void BowserFire::Process()
{
	if (m_nState == STATE_DEAD)
		return;

	OnWalk();

	if (IsOutOfStage())
		ChangeState(STATE_DEAD);

	if (m_vecOverlappedEnemy.size() > 0)
		OnDamage();
	if (m_bCollisionPlayer)
		OnDamagePlayer();

	m_nLifeFrame++;
}

void BowserFire::OnBeforeProcess()
{
	if (m_nNextState != -1)
	{
		// Affected by other
		m_nState = m_nNextState;
		m_nNextState = -1;
		m_nStateFrame = 0;
	}

	/*
	// Will be used from ClownCar
	if (m_fScaleX > 1 || m_fScaleY > 1)
		CheckCollision();
		*/

	if (m_nDamageGroup == DAMAGEGROUP_PLAYER)
		CheckCollisionEnemy();
	else
		CheckCollisionPlayer();
}

void BowserFire::ChangeState(int nState, ChangeStateInfo *pInfo)
{
	switch (nState)
	{
 	case STATE_SPITTED:
 	case STATE_SPITTED_SAFE:
		if (pInfo && pInfo->pFromObject)
		{
			m_bFlip = pInfo->pFromObject->m_bFlip;
			m_fXS = 2.0f;
			if (m_bFlip)
				m_fXS *= -1;

			m_pOwner = pInfo->pFromObject;

			nState = STATE_WALK;
		}
 		break;
	default:
		GameObjectBase::ChangeState(nState);
		return;
		break;
	}

	m_nNextState = nState;
}

void BowserFire::OnWalk()
{
	if (m_nTargetY != 0)
	{
		if (m_nTargetY < m_fY)
			m_fYS = -1.0f;
		else if (m_nTargetY > m_fY)
			m_fYS = 1.0f;
		else if (m_nTargetY == m_fY)
			m_fYS = 0;
	}
}

void BowserFire::OnDamage()
{
	std::vector<GameObjectBase*>::iterator it = m_vecOverlappedEnemy.begin();
	for (; it != m_vecOverlappedEnemy.end(); ++it)
	{
		GameObjectBase *pEnemy = *it;
        
        // #TODO Fix
        if (!pEnemy->CanBurnt())
        {
            CUR_STAGE->CreateEffect(m_fX, m_fY, EFFECT_DUST);
            ChangeState(STATE_DEAD);
            return;
        }
		if (pEnemy->IsAlive() && pEnemy->CanBurnt())
		{
			// Damage All Target
			ChangeStateInfo info;
			info.nNextState = STATE_BURNT;
			info.pFromObject = this;
			pEnemy->ChangeState(STATE_BURNT, &info);

			if (m_pOwner && GET_CONTROLLER_IF_PLAYER(m_pOwner))
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

void BowserFire::OnDamagePlayer()
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
        else if (pEnemy->m_nDamageCooltime == 0)
        {
			CreateParameter param;
			param.bIsQuick = true;
			CUR_STAGE->CreateEffect(m_fX, m_fY, EFFECT_EXPLOSION, &param);

			/*
			param.bIsQuick = false;
			param.nType = EFFECT_SMALLDUST;
            CUR_STAGE->CreateEffect(m_fX, m_fY, EFFECT_DUST);
			*/
            ChangeState(STATE_DEAD);
            return;
        }
    }
}

bool BowserFire::CanEat()
{
	if (DAMAGEGROUP_ENEMY)
		return true;

	return false;
}

bool BowserFire::CanSpit()
{
	return true;
}

SIZE BowserFire::GetSize()
{
	SIZE s = 
	{
		24 * m_fScaleX, 
		8 * m_fScaleY 
	};
	return s;
}

void BowserFire::Render(int nColor, int nZOrder)
{
	if (m_nState == STATE_DEAD)
		return;

	int nFrame = GetSpriteIndex();
	if (m_nStateFrame < 16)
		nZOrder = Z_BULLET_BEHIND_ENEMY;
	else
		nZOrder = Z_BULLET;

	bool bShadow = IS_MODERN;

	NaPointT<float> pt = CAMERA->GetPosition();

	m_pSprite->RenderToQueue(x - pt.x, y - pt.y, nFrame, m_bFlip, m_bFlipV, nColor,
		m_fScaleX, m_fScaleY, nZOrder, bShadow);
}

int BowserFire::GetSpriteIndex()
{
	// #TODO improvement
	int nFrame = SPRIDX_BULLET_BOWSERFIRE1 + (m_nLifeFrame % 4) / 2;
	return nFrame;
}
