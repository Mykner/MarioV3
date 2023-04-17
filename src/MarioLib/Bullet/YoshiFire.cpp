#include "YoshiFire.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"

YoshiFire::YoshiFire(Game *pGame, Stage *pStage) :
	BulletObjectBase(pGame, pStage)
{
	m_nType = BULLET_YOSHIFIRE;
	m_nState = STATE_WALK;

	m_fXS = 1.0f;
	m_fYS = 0;
	m_pOwner = nullptr;
	m_nLifeFrame = 0;
	m_nTargetY = 0;
}

YoshiFire::~YoshiFire()
{
}

void YoshiFire::ParseParameter(CreateParameter * pParam)
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
}

void YoshiFire::Process()
{
	if (m_nState == STATE_DEAD)
		return;

	if (m_pOwner && m_pOwner->m_nState == STATE_DEAD)
		m_pOwner = nullptr;

	OnWalk();

	if (IsOutOfStage() || IsOutOfCamera())
		ChangeState(STATE_DEAD);

	if (m_vecOverlappedEnemy.size() > 0)
		OnDamage();
	if (m_bCollisionPlayer)
		OnDamagePlayer();

	m_nLifeFrame++;
}

void YoshiFire::OnBeforeProcess()
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

void YoshiFire::ChangeState(int nState, ChangeStateInfo *pInfo)
{
	switch (nState)
	{
 	case STATE_SPITTED:
 	case STATE_SPITTED_SAFE:
		if (pInfo && pInfo->pFromObject)
		{
			m_bFlip = pInfo->pFromObject->m_bFlip;
			m_fXS = 1.5f;
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

void YoshiFire::OnWalk()
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

void YoshiFire::OnDamage()
{
	std::vector<GameObjectBase*>::iterator it = m_vecOverlappedEnemy.begin();
	for (; it != m_vecOverlappedEnemy.end(); ++it)
	{
		GameObjectBase *pEnemy = *it;
        
		if (pEnemy->IsAlive() && pEnemy->CanBurnt(BURN_YOSHIFIRE))
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

			CUR_STAGE->CreateItem(pEnemy->m_fX, pEnemy->m_fY, ITEM_THROWNCOIN);
		}
	}
}

void YoshiFire::OnDamagePlayer()
{
	GameObjectBase *pEnemy = CUR_PLAYER_OBJ;
	if (pEnemy->IsAlive())
    {        
        if (pEnemy->CanBurnt(BURN_YOSHIFIRE))
        {
            ChangeStateInfo info;
            info.nNextState = STATE_BURNT;
            info.pFromObject = this;
            pEnemy->ChangeState(STATE_BURNT, &info);
        }
        else
        {
			CreateParameter param;
			CUR_STAGE->CreateEffect(m_fX, m_fY, EFFECT_EXPLOSION, &param);
		
            ChangeState(STATE_DEAD);
            return;
        }
    }
}

bool YoshiFire::CanEat()
{
	if (m_nDamageGroup == DAMAGEGROUP_ENEMY)
		return true;

	return false;
}

bool YoshiFire::CanSpit()
{
	return true;
}

SIZE YoshiFire::GetSize()
{
	SIZE s = { 24, 8 };
	return s;
}

void YoshiFire::Render(int nColor, int nZOrder)
{
	if (m_nState == STATE_DEAD)
		return;

	NaPointT<float> pt = CAMERA->GetPosition();
	int nFrame = GetSpriteIndex();
	m_pSprite->RenderToQueue(x - pt.x, y - pt.y, nFrame, m_bFlip, m_bFlipV, nColor,
		1.0f, 1.0f, Z_BULLET_BEHIND_ENEMY);
}

int YoshiFire::GetSpriteIndex()
{
	int nFrame = SPRITE_INDEX_CYCLE(SPRIDX_BULLET_YOSHIFIRE1, 2, 4);
	return nFrame;
}
