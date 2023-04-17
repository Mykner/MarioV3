#include "Wrench.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "SoundManager.h"

#include "RockyWrench.h"

Wrench::Wrench(Game *pGame, Stage *pStage) :
	BulletObjectBase(pGame, pStage)
{
	m_nType = BULLET_WRENCH;
	m_nState = STATE_IDLE;

	m_fXS = 0;
	m_fYS = 0;
	m_pOwner = nullptr;
	m_nLifeFrame = 0;
	m_nTargetY = 0;
}

Wrench::~Wrench()
{
}

void Wrench::ParseParameter(CreateParameter * pParam)
{
	if (pParam == nullptr)
		return;
	
	if (pParam->pFromPlayer != nullptr)
		m_nDamageGroup = DAMAGEGROUP_PLAYER;
	else
		m_nDamageGroup = DAMAGEGROUP_ENEMY;

	if (pParam->pFromObject)
	{
		m_pOwner = pParam->pFromObject;
		m_pController = pParam->pFromObject->m_pController;
		m_bFlip = pParam->pFromObject->m_bFlip;
	}

	if (pParam->fScaleX != 0)
		m_fScaleX = pParam->fScaleX;
	if (pParam->fScaleY != 0)
		m_fScaleY = pParam->fScaleY;

	if (pParam->nType == -1)
	{
		m_nState = STATE_IDLE;
		m_fXS = pParam->fXS;
		m_fYS = pParam->fYS;
	}
	else
	{
		if (pParam->fXS != 0)
			m_fXS = pParam->fXS;
		if (pParam->fYS != 0)
			m_fYS = pParam->fYS;
	}

	if (m_bFlip)
		m_fXS *= -1;
}

void Wrench::Process()
{
	if (m_nState == STATE_DEAD)
		return;

	if (m_vecOverlappedEnemy.size() > 0)
		OnDamage();
	if (m_bCollisionPlayer)
		OnDamagePlayer();

	switch (m_nState)
	{
	case STATE_WALK:
		OnWalk();
		break;
	}

	if (IsOutOfCamera())
		ChangeState(STATE_DEAD);

	m_nLifeFrame++;
}

void Wrench::OnBeforeProcess()
{
	if (m_nDamageGroup == DAMAGEGROUP_PLAYER)
		CheckCollisionEnemy();
	else
		CheckCollisionPlayer();
	
	CheckCollision();
}

void Wrench::ChangeState(int nState, ChangeStateInfo *pInfo)
{
	switch (nState)
	{
	case STATE_WALK:
		if (m_bFlip)
			m_fXS = -1.0f;
		else
			m_fXS = 1.0f;
		break;
	case STATE_DEAD:
		if (m_pOwner)
		{
			switch (m_pOwner->m_nType)
			{
			case ENEMY_ROCKYWRENCH:
				((RockyWrench*)m_pOwner)->m_pWrench = nullptr;
				m_pOwner = nullptr;
				break;
			}
		}
		break;
	}

	GameObjectBase::ChangeState(nState);
}

void Wrench::OnWalk()
{
	;
}

void Wrench::OnDamage()
{
	std::vector<GameObjectBase*>::iterator it = m_vecOverlappedEnemy.begin();
	for (; it != m_vecOverlappedEnemy.end(); ++it)
	{
		GameObjectBase *pEnemy = *it;
		if (pEnemy->IsAlive())
		{
			// Damage Only One Target
			if (pEnemy->CanBurnt())
			{
				ChangeStateInfo info;
				info.nNextState = STATE_BURNT;
				info.pFromObject = this;
				pEnemy->ChangeState(STATE_BURNT, &info);
			}
			else
			{
				SOUND_MANAGER->PlaySoundEffect(L"HitBlock");
			}
			break;
		}
	}
}

void Wrench::OnDamagePlayer()
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
	}
}

bool Wrench::CanEat()
{
	return false;
}

void Wrench::GetScale(float & fScaleX, float & fScaleY)
{
	fScaleX = m_fScaleX;
	fScaleY = m_fScaleY;

	if (m_nState == STATE_FLY && m_nStateFrame < 8)
	{
		fScaleX *= ((m_nStateFrame / 16.0f) + 0.5f);
		fScaleY *= ((m_nStateFrame / 16.0f) + 0.5f);
	}
}

SIZE Wrench::GetSize()
{
	SIZE s = { 8, 8 };
	return s;
}

void Wrench::Render(int nColor, int nZOrder)
{
	if (m_nState == STATE_DEAD)
		return;

	NaPointT<float> pt = CAMERA->GetPosition();
	int nFrame = GetSpriteIndex();

	float fScaleX, fScaleY;
	GetScale(fScaleX, fScaleY);

	if (m_pGame->m_bModernStyle)
	{
		bool bShadow = true;
		float fAngle = 0;
		if (m_fXS != 0 || m_fYS != 0)
			fAngle = m_nStateFrame * 15;
		m_pSprite->RenderToQueue(x - pt.x, y - pt.y, nFrame, m_bFlip, m_bFlipV, nColor, 
			fScaleX, fScaleY, Z_BULLET, bShadow, fAngle);
	}
	else
	{
		m_pSprite->RenderToQueue(x - pt.x, y - pt.y, nFrame, m_bFlip, m_bFlipV, nColor, 
			fScaleX, fScaleY, Z_BULLET);
	}	
}

int Wrench::GetSpriteIndex()
{
	if (m_pGame->m_bModernStyle)
		return SPRIDX_BULLET_WRENCH1;

#define WRENCH_SPRITE_INDEX_CYCLE(from, cnt, delay) \
    (from + (m_nLifeFrame % (cnt * delay)) / delay)

	if (m_fXS != 0 || m_fYS != 0)
		return SPRIDX_BULLET_WRENCH1;

	return WRENCH_SPRITE_INDEX_CYCLE(SPRIDX_BULLET_WRENCH1, 4, 3);
}
