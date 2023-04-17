#include "ShoeSmoke.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "FModManager.h"
#include "Stage.h"
#include "TypeNames.h"
#include "SoundManager.h"
#include "GoombaShoe.h"

#include "UIBase.h"
#include "SpeechBubble.h"

ShoeSmoke::ShoeSmoke(Game *pGame, Stage *pStage) :
	EventObjectBase(pGame, pStage)
{
	m_nType = EVENTOBJECT_SHOESMOKE;
	m_nState = STATE_IDLE;

	m_bCanLand = true;
	m_bCanCollide = false;

	RequestSprites(pStage);
}

ShoeSmoke::~ShoeSmoke()
{
}

void ShoeSmoke::RequestSprites(Stage * pStage)
{
	if (pStage == nullptr)
		pStage = CUR_STAGE;

	m_ppSprite = &pStage->m_pSpriteEffect;
}

void ShoeSmoke::ParseParameter(CreateParameter * pParam)
{
	if (pParam == nullptr)
		return;

	if (pParam->fXS != 0)
		m_fXS = pParam->fXS;
	if (pParam->fYS != 0)
		m_fYS = pParam->fYS;
	if (m_fXS < 0)
		m_bFlip = true;
	else
		m_bFlip = false;

	if (pParam->fScaleX != 0)
		m_fScaleX = pParam->fScaleX;
	if (pParam->fScaleY != 0)
		m_fScaleY = pParam->fScaleY;
	
	if (pParam->pFromObject)
	{
		m_nDamageGroup = pParam->pFromObject->m_nDamageGroup;
		if (pParam->pFromObject->m_nType == VEHICLE_GOOMBASHOE)
		{
			GoombaShoe *pShoe = (GoombaShoe*)pParam->pFromObject;
			if (pShoe->m_nShoeType == 1)
				m_bPowerUp = true;
		}
	}
}

void ShoeSmoke::Process()
{
	ProcessBase();
}

void ShoeSmoke::OnBeforeProcess()
{
	CheckCollision();
	CheckCollisionEnemy();
}

void ShoeSmoke::OnAfterProcess()
{
	m_fX += m_fXS;
	m_fY += m_fYS;

    ProcessState();
}

void ShoeSmoke::Render(int nColor, int nZOrder)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	float _x = x - pt.x;
	float _y = y - pt.y;

	int nFrame = GetSpriteIndex();
	bool bShadow = m_pGame->m_bModernStyle;
	if (nZOrder == -1)
		nZOrder = GetZOrder();
	
	float fScaleX, fScaleY;
	GetScale(fScaleX, fScaleY);

	if (m_nStateFrame >= 46)
		_y -= 8;

	(*m_ppSprite)->RenderToQueue(_x, _y, nFrame,
		m_bFlip, false, nColor, fScaleX, fScaleY, nZOrder, bShadow);
}

void ShoeSmoke::ChangeState(int nState, ChangeStateInfo * pInfo)
{
	switch (nState)
	{
	case STATE_PRESSED:
		break;
	default:
		EventObjectBase::ChangeState(nState, pInfo);
		return;
		break;
	}

	m_nNextState = nState;
}

void ShoeSmoke::OnIdle()
{
	if (m_nStateFrame > 0)
	{
		float fFactor = 1;
		if (m_fXS > 0)
			fFactor = -1;

		if (m_nStateFrame < 24)
		{
			m_fXS += (fFactor * 0.10f);
		}
		else if (m_nStateFrame >= 24)
		{
			m_fXS += (fFactor * 0.12f);
		}
	}

	std::vector<GameObjectBase*> vecGameObjects;
	vecGameObjects.insert(vecGameObjects.end(), m_vecOverlappedEnemy.begin(), m_vecOverlappedEnemy.end());

	// #TODO Add Items too

	if (vecGameObjects.size() > 0)
	{
		int nPower = BURN_STARMAN;
// 		if (m_bPowerUp)
// 			nPower = BURN_BOBOMB;

		BEGIN_VECTOR_LOOP(GameObjectBase*, vecGameObjects) {
			GameObjectBase *pObj = *it;
			if (pObj->IsAlive())
			{
				// Damage Only One Target
				if (IS_ENEMY(pObj))
				{
					if (pObj->CanBurnt(nPower))
					{
						ChangeStateInfo info;
						info.nNextState = STATE_BURNT;
						info.pFromObject = this;
						pObj->ChangeState(STATE_BURNT, &info);
					}
					else
					{
						if (pObj->m_bCollision[COLLISION_BOTTOM] && pObj->m_fYS == 0 &&
							pObj->m_fY == m_fY)
							pObj->m_fYS = -4.0f;
					}
				}
			}
		} END_VECTOR_LOOP();
	}

	if (m_nStateFrame == 54)
	{
		ChangeState(STATE_DEAD);
	}
}

bool ShoeSmoke::IsAlive()
{
	return (m_nState != STATE_DEAD);
}

bool ShoeSmoke::CanBornFromBlock()
{
	return false;
}

bool ShoeSmoke::CanCarried()
{
	return false;
}

void ShoeSmoke::AddPassenger(GameObjectBase * pNewObj)
{
	if (pNewObj->m_nType >= BULLET_BEGIN && pNewObj->m_nType < BULLET_END)
		return;

	switch (m_nState)
	{
	case STATE_ITEMPOP:
	case STATE_ITEMPOPDOWN:
	case STATE_PIPEUP:
	case STATE_PIPEDOWN:
	case STATE_PIPELEFT:
	case STATE_PIPERIGHT:
		return;
		break;
	}
}

int ShoeSmoke::GetSpriteIndex()
{
	if (EventObjectBase::GetSpriteIndex() == -1)
		return -1;

	int nFrame = SPRIDX_EFFECT_SHOESMOKE1;
	if (m_nStateFrame < 4)
		nFrame = SPRIDX_EFFECT_SHOESMOKE1;
	else if (m_nStateFrame < 8)
		nFrame = SPRIDX_EFFECT_SHOESMOKE2;
	else if (m_nStateFrame < 12)
		nFrame = SPRIDX_EFFECT_SHOESMOKE3;
	else if (m_nStateFrame < 28)
		nFrame = SPRIDX_EFFECT_SHOESMOKE4;
	else if (m_nStateFrame < 36)
		nFrame = SPRIDX_EFFECT_SHOESMOKE5;
	else if (m_nStateFrame < 40)
		nFrame = SPRIDX_EFFECT_SHOESMOKE4;
	else if (m_nStateFrame < 44)
		nFrame = SPRIDX_EFFECT_SHOESMOKE5;
	else if (m_nStateFrame < 46)
		nFrame = SPRIDX_EFFECT_SHOESMOKE4;
	else if (m_nStateFrame < 48)
		nFrame = SPRIDX_EFFECT_DUST4;
	else if (m_nStateFrame < 50)
		nFrame = SPRIDX_EFFECT_DUST3;
	else if (m_nStateFrame < 52)
		nFrame = SPRIDX_EFFECT_DUST2;
	else if (m_nStateFrame < 54)
		nFrame = SPRIDX_EFFECT_DUST1;
	else 
		nFrame = -1;

	// Continue to Dust

	return nFrame;
}

int ShoeSmoke::GetZOrder()
{
	int nZOrder = GameObjectBase::GetZOrder();
	if (nZOrder != 0)
		return nZOrder;

	if (m_nState == STATE_ITEMPOP || m_nState == STATE_ITEMPOPDOWN)
		return Z_BEHIND_MAP;

	return Z_EFFECT;
}

void ShoeSmoke::GetScale(float & fScaleX, float & fScaleY)
{
	fScaleX = m_fScaleX;
	fScaleY = m_fScaleY;

	if (m_nState == STATE_EXPLODE)
	{
		int nFactor = m_nStateFrame % 4;
		if (nFactor == 0)
		{
			fScaleX = m_fScaleX * (1.0f + (m_nStateFrame / 90.0f));
			fScaleY = fScaleX;
		}
		else if (nFactor == 2)
		{
			fScaleX = m_fScaleX * (1.0f + (m_nStateFrame / 30.0f));
			fScaleY = fScaleX;
		}
	}
}

SIZE ShoeSmoke::GetSize()
{
	SIZE s = { 16, 16 };

	return s;
}
