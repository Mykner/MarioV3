#include "SpinySpike.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"

SpinySpike::SpinySpike(Game *pGame, Stage *pStage) :
	BulletObjectBase(pGame, pStage)
{
	m_nType = BULLET_SPINYSPIKE;
	m_nState = STATE_WALK;

	m_fXS = 0;
	m_fYS = 0;
	m_pOwner = nullptr;
	m_nLifeFrame = 0;
	m_nTargetY = 0;
}

SpinySpike::~SpinySpike()
{
}

void SpinySpike::ParseParameter(CreateParameter * pParam)
{
	if (pParam == nullptr)
		return;
	
	if (pParam->pFromPlayer != nullptr)
		m_nDamageGroup = DAMAGEGROUP_PLAYER;

	if (pParam->pFromObject)
	{
		m_pOwner = pParam->pFromObject;
		//m_bFlip = pParam->pFromObject->m_bFlip;
	}

	m_fXS = pParam->fXS;
	m_fYS = pParam->fYS;

	if (m_bFlip)
		m_fXS *= -1;
	
	if (pParam->fScaleX != 0)
		m_fScaleX = pParam->fScaleX;
	if (pParam->fScaleY != 0)
		m_fScaleY = pParam->fScaleY;
}

void SpinySpike::Process()
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

void SpinySpike::OnBeforeProcess()
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

void SpinySpike::OnWalk()
{
}

void SpinySpike::OnDamage()
{
	std::vector<GameObjectBase*>::iterator it = m_vecOverlappedEnemy.begin();
	for (; it != m_vecOverlappedEnemy.end(); ++it)
	{
		GameObjectBase *pEnemy = *it;
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

void SpinySpike::OnDamagePlayer()
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
            ChangeState(STATE_DEAD);
            return;
        }
    }
}

bool SpinySpike::CanEat()
{
	if (m_nDamageGroup == DAMAGEGROUP_ENEMY)
		return true;

	return false;
}

bool SpinySpike::CanSpit()
{
	return false;
}

SIZE SpinySpike::GetSize()
{
	SIZE s = 
	{
		8 * m_fScaleX, 
		8 * m_fScaleY 
	};
	return s;
}

void SpinySpike::Render(int nColor, int nZOrder)
{
	if (m_nState == STATE_DEAD)
		return;

	int nFrame = GetSpriteIndex();

	if (m_nStateFrame < 16)
		nZOrder = Z_BULLET_BEHIND_ENEMY;
	else
		nZOrder = Z_BULLET;

	bool bShadow = IS_MODERN;
	float fAngle = 0;
	if (m_fXS == 0 && m_fYS < 0)
		fAngle = 0;
	else if (m_fXS > 0 && m_fYS < 0)
		fAngle = 45;
	else if (m_fXS > 0 && m_fYS == 0)
		fAngle = 90;
	else if (m_fXS > 0 && m_fYS > 0)
		fAngle = 135;
	else if (m_fXS == 0 && m_fYS > 0)
		fAngle = 180;
	else if (m_fXS < 0 && m_fYS > 0)
		fAngle = 225;
	else if (m_fXS < 0 && m_fYS == 0)
		fAngle = 270;
	else if (m_fXS < 0 && m_fYS < 0)
		fAngle = 315;

	NaPointT<float> pt = CAMERA->GetPosition();

	m_pSprite->RenderToQueue(x - pt.x, y - pt.y, nFrame, m_bFlip, m_bFlipV, nColor,
		m_fScaleX, m_fScaleY, nZOrder, bShadow, fAngle);
}

int SpinySpike::GetSpriteIndex()
{
	return SPRIDX_BULLET_SPINYSPIKE;
}
