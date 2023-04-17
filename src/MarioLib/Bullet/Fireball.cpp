#include "Fireball.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "SoundManager.h"

FireBall::FireBall(Game *pGame, Stage *pStage) :
	BulletObjectBase(pGame, pStage)
{
	m_nType = BULLET_FIREBALL;
	m_nState = STATE_FIREDOWN;

	m_fXS = 4.0f;
	m_fYS = 4.0f;
	m_pOwner = nullptr;
	m_nLifeFrame = 0;
}

FireBall::~FireBall()
{
}

void FireBall::ParseParameter(CreateParameter * pParam)
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
		m_nState = STATE_FLY;
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

void FireBall::Process()
{
	if (m_nState == STATE_DEAD)
		return;

	if (m_vecOverlappedEnemy.size() > 0)
		OnDamage();
	if (m_bCollisionPlayer)
		OnDamagePlayer();

	switch (m_nState)
	{
	case STATE_FIREDOWN:
		OnFireDown();
		break;
	case STATE_FIREUP:
		OnFireUp();
		break;
	case STATE_FLY:
		OnFly();
		break;
	case STATE_TRAMPOLINPRESS:
		OnTrampolinPress();
		break;
	case STATE_TRAMPOLINJUMP:
		OnFireUp();
		break;
	}

	if (IsOutOfCamera())
		ChangeState(STATE_DEAD);

	m_nLifeFrame++;
}

void FireBall::OnBeforeProcess()
{
	if (m_nDamageGroup == DAMAGEGROUP_PLAYER)
		CheckCollisionEnemy();
	else
		CheckCollisionPlayer();
	
	CheckCollision();
}

void FireBall::ChangeState(int nState, ChangeStateInfo *pInfo)
{
	switch (nState)
	{
	case STATE_FIREDOWN:
		m_fYS = 2.0f;
		break;
	case STATE_FIREUP:
		m_fYS = -4.0f;
		break;
	}

	GameObjectBase::ChangeState(nState);
}

void FireBall::OnFireDown()
{
	CheckReachWall();

	if (IsAlive())
	{
		int nS = m_vecOverlappedEnemy.size();
		int nP = m_vecPressedEnemy.size();

		if (m_bCollision[COLLISION_BOTTOM])
			ChangeState(STATE_FIREUP);
		else if (m_fYS < 4.0f)
			m_fYS += 0.6f;
	}
}

void FireBall::OnFireUp()
{
	CheckReachWall();
	
	m_fYS += 0.7f;

	if (IsAlive() && m_nStateFrame > 0 && m_fYS >= 1)
		ChangeState(STATE_FIREDOWN);
}

void FireBall::OnFly()
{
	if (m_bCollision[COLLISION_TOP] ||
		m_bCollision[COLLISION_LEFT] ||
		m_bCollision[COLLISION_RIGHT] ||
		m_bCollision[COLLISION_BOTTOM])
	{
		CreateParameter param;
		param.nType = EFFECT_DUST;
		param.pFromObject = this;
		param.fScaleX = m_fScaleX;
		param.fScaleY = m_fScaleY;
		CUR_STAGE->CreateEffect(
			m_fX + (m_fXS * 4), 
			m_fY + (m_fYS * 4) - 4, EFFECT_DUST, &param);

		ChangeState(STATE_DEAD);

		SOUND_MANAGER->PlaySoundEffect(L"HitBlock");
	}
}

void FireBall::OnDamage()
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

	CreateParameter param;
	param.nType = EFFECT_EXPLOSION;
	param.pFromObject = this;
	CUR_STAGE->CreateEffect(m_fX, m_fY - 4, EFFECT_EXPLOSION, &param);

	// Dead immediately
	m_nState = STATE_DEAD;
	m_nNextState = -1;
}

void FireBall::OnDamagePlayer()
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

bool FireBall::CanEat()
{
	return false;
}

void FireBall::OnReachWall(bool bIsLeftWall)
{
	if (IS_MODERN)
	{
		CreateParameter param;
		param.nType = EFFECT_DUST;
		param.pFromObject = this;
		CUR_STAGE->CreateEffect(m_fX, m_fY - 4, EFFECT_DUST, &param);
	}
	else
	{
		CreateParameter param;
		param.nType = EFFECT_EXPLOSION;
		param.pFromObject = this;
		CUR_STAGE->CreateEffect(m_fX, m_fY - 4, EFFECT_EXPLOSION, &param);
	}

	ChangeState(STATE_DEAD);

	SOUND_MANAGER->PlaySoundEffect(L"HitBlock");
}

void FireBall::GetScale(float & fScaleX, float & fScaleY)
{
	fScaleX = m_fScaleX;
	fScaleY = m_fScaleY;

	if (m_nState == STATE_FLY && m_nStateFrame < 8)
	{
		fScaleX *= ((m_nStateFrame / 16.0f) + 0.5f);
		fScaleY *= ((m_nStateFrame / 16.0f) + 0.5f);
	}
}

SIZE FireBall::GetSize()
{
	SIZE s = { 8, 8 };
	return s;
}

void FireBall::Render(int nColor, int nZOrder)
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
		float fAngle = m_nStateFrame * 30;
		m_pSprite->RenderToQueue(x - pt.x, y - pt.y, nFrame, m_bFlip, m_bFlipV, nColor, 
			fScaleX, fScaleY, Z_BULLET, bShadow, fAngle);
	}
	else
	{
		m_pSprite->RenderToQueue(x - pt.x, y - pt.y, nFrame, m_bFlip, m_bFlipV, nColor, 
			fScaleX, fScaleY, Z_BULLET);
	}	
}

int FireBall::GetSpriteIndex()
{
	if (m_pGame->m_bModernStyle)
		return SPRIDX_BULLET_FIREBALL1;

#define FIREBALL_SPRITE_INDEX_CYCLE(from, cnt, delay) \
    (from + (m_nLifeFrame % (cnt * delay)) / delay)
    
	int nFrame = FIREBALL_SPRITE_INDEX_CYCLE(SPRIDX_BULLET_FIREBALL1, 4, 3);
	return nFrame;
}
