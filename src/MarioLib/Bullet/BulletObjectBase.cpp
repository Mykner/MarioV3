#include "BulletObjectBase.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "SoundManager.h"

#include "FireBall.h"
#include "BowserFire.h"
#include "ThrowingHammer.h"
#include "YoshiTongue.h"
#include "YoshiFire.h"
#include "SpinySpike.h"
#include "Boomerang.h"
#include "Wrench.h"

BulletObjectBase::BulletObjectBase(Game *pGame, Stage *pStage) :
	GameObjectBase(pGame, pStage)
{
	m_nType = BULLET_NONE;
	m_nState = STATE_IDLE;

	RequestSprites(pStage);

	m_pOwner = nullptr;
	m_nLifeFrame = 0;
}

BulletObjectBase::~BulletObjectBase()
{
}

void BulletObjectBase::RequestSprites(Stage * pStage)
{
	m_pSprite = SPR_MANAGER->Request(L"Bullet");
}

void BulletObjectBase::ParseParameter(CreateParameter * pParam)
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

	if (pParam->fXS != 0)
		m_fXS = pParam->fXS;
	if (pParam->fYS != 0)
		m_fYS = pParam->fYS;

	if (pParam->fScaleX != 0)
		m_fScaleX = pParam->fScaleX;
	if (pParam->fScaleY != 0)
		m_fScaleY = pParam->fScaleY;

	if (m_bFlip)
		m_fXS *= -1;
}

void BulletObjectBase::Process()
{
	if (m_nState == STATE_DEAD)
		return;

	if (m_vecOverlappedEnemy.size() > 0)
		OnDamageEnemy();
	if (m_bCollisionPlayer)
		OnDamagePlayer();

	ProcessBase();

	if (IsOutOfCamera())
		ChangeState(STATE_DEAD);

	m_nLifeFrame++;
}

void BulletObjectBase::OnBeforeProcess()
{
	if (m_nDamageGroup == DAMAGEGROUP_PLAYER)
		CheckCollisionEnemy();
	else
		CheckCollisionPlayer();
	
	CheckCollision();
}

void BulletObjectBase::OnDamageEnemy()
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

void BulletObjectBase::OnDamagePlayer()
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

bool BulletObjectBase::CanEat()
{
	return false;
}

bool BulletObjectBase::CanSpit()
{
	return false;
}

void BulletObjectBase::Render(int nColor, int nZOrder)
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

int BulletObjectBase::GetSpriteIndex()
{
	return -1;
}

GameObjectBase * BulletObjectBase::CreateInstance(Game * pGame, Stage * pStage, int nType)
{
	GameObjectBase *pObj = nullptr;
	switch (nType)
	{
	case BULLET_FIREBALL:				pObj = new FireBall(pGame, pStage);				break;
	case BULLET_BOWSERFIRE:				pObj = new BowserFire(pGame, pStage);			break;
	case BULLET_THROWINGHAMMER:			pObj = new ThrowingHammer(pGame, pStage);		break;
	case BULLET_YOSHITONGUE:			pObj = new YoshiTongue(pGame, pStage);			break;
	case BULLET_YOSHIFIRE:				pObj = new YoshiFire(pGame, pStage);			break;
	case BULLET_SPINYSPIKE:				pObj = new SpinySpike(pGame, pStage);			break;
	case BULLET_BOOMERANG:				pObj = new Boomerang(pGame, pStage);			break;
	case BULLET_WRENCH:					pObj = new Wrench(pGame, pStage);				break;
	}

	return pObj;
}
