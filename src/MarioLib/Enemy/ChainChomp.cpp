#include "ChainChomp.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "ChainChompStake.h"
#include "SoundManager.h"

BEGIN_IMPL_PROPERTYMAP(ChainChomp)
	PROP_BOOL("ChainedToStake", VT_BOOL, true, nullptr, "ChainChomp"),
	PROP_BOOL("PowerUp", VT_BOOL, false, nullptr, "ChainChomp"),
	PROP_BOOL("Winged", VT_BOOL, false, nullptr, "ChainChomp"),
END_IMPL_PROPERTYMAP()

ChainChomp::ChainChomp(Game *pGame, Stage *pStage) :
	EnemyObjectBase(pGame, pStage)
{
	m_nType = ENEMY_CHAINCHOMP;
	m_nState = STATE_IDLE;

	m_bFlip = true;
	m_nBaseState = STATE_STAND;

	m_pStake = nullptr;
	m_bStake = true;
	m_fAngle = 0;
}

ChainChomp::~ChainChomp()
{
}

bool ChainChomp::CanCollisionWith(GameObjectBase * pOther)
{
	if (pOther && pOther->m_nType == ENEMY_CHAINCHOMPSTAKE)
		return false;

	return EnemyObjectBase::CanCollisionWith(pOther);
}

void ChainChomp::Render(int nColor, int nZOrder)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	int nFrame = GetSpriteIndex();

	if (nZOrder == -1)
		nZOrder = GetZOrder();

	if (nFrame != -1)
	{
		bool bShadow = m_pGame->m_bModernStyle && nZOrder != Z_MAP_SHADOW;
		float fAngle = 0.0f;

		if (IS_SPINNING)
		{
			fAngle = m_nStateFrame * -14;
			if (m_fXS < 0)
				fAngle *= -1;
			if (!m_bFlip)
				fAngle *= -1;
		}
		else
		{
			fAngle = m_fAngle;
		}

		float fScaleX, fScaleY;
		GetScale(fScaleX, fScaleY);

		(*m_ppSprite)->RenderToQueue(x - pt.x, y - pt.y, nFrame, m_bFlip, m_bFlipV, nColor,
			fScaleX, fScaleY, nZOrder, bShadow, fAngle);

		if (m_bWinged)
			RenderWing(nColor, nZOrder);

		if (IsAlive())
			RenderChain();
	}
}

void ChainChomp::RenderChain(int nColor, int nZOrder)
{
	if (nZOrder == -1)
		nZOrder = Z_AFTER_MAP;

	NaPointT<float> ptCamera = CAMERA->GetPosition();
	bool bShadow = IS_MODERN;

	for (int i=0; i<4; i++)
	{
		if (m_ptChain[i].x == 0 && m_ptChain[i].y == 0)
			continue;

		(*m_ppSprite)->RenderToQueue(
			m_ptChain[i].x - ptCamera.x,
			m_ptChain[i].y - ptCamera.y, SPRIDX_CHAINCHOMP_CHAIN, false, false,
			nColor, m_fScaleX, m_fScaleY, nZOrder, bShadow);
	}		
}

void ChainChomp::RenderDesign(int nColor, int nZOrder)
{
	Render(nColor, nZOrder);

	if (m_bStake)
	{
		if (nZOrder == -1)
			nZOrder = Z_MAP;

		NaPointT<float> ptCamera = CAMERA->GetPosition();
		bool bShadow = IS_MODERN;

		(*m_ppSprite)->RenderToQueue(
			m_fX - ptCamera.x, m_fY - ptCamera.y, SPRIDX_CHAINCHOMP_STAKE, false, false,
			nColor, 1.0f, 1.0f, nZOrder, bShadow);
	}
}

void ChainChomp::ChangeState(int nState, ChangeStateInfo *pInfo)
{
	switch (nState)
	{
	case STATE_WALK:
		if (m_bFlip)
			m_fXS = -1.0f;
		else
			m_fXS = 1.0f;
		break;
	case STATE_STAND:
		m_fXS = 0;
		m_fYS = 0;

		// Find Target
		if (m_pStake)
		{
			if (rand() % 2 == 0)
			{
				m_ptTarget.x = CUR_PLAYER_OBJ->m_fX;
				m_ptTarget.y = CUR_PLAYER_OBJ->m_fY;
			}
			else
			{
				m_ptTarget.x = m_pStake->m_fX + (rand() % 100) - (rand() % 100);
				m_ptTarget.y = m_pStake->m_fY + (rand() % 100) - (rand() % 100);
			}

			if (m_ptTarget.y > m_fY)
			{
				m_ptTarget.y = m_fY - (m_ptTarget.y - m_fY);
			}

			float fDistX = (m_ptTarget.x - m_fX);
			float fDistY = (m_ptTarget.y - m_fY);
			float fAngle = atan2(fDistY, fDistX) / fToRadian + 90.0f;

			if (fAngle > 0 && fAngle < 180)
			{
				m_bFlip = false;
				m_fAngle = fAngle - 90.0f;
			}
			else
			{
				m_bFlip = true;
				m_fAngle = 270.0f - fAngle;
			}
		}		
		break;
	case STATE_JUMP:
		if (m_ptTarget.x != 0 && m_ptTarget.y != 0)
		{
			float fDistX = (m_ptTarget.x - m_fX);
			float fDistY = (m_ptTarget.y - m_fY);

			m_fXS = fDistX / 10.0f;
			m_fYS = fDistY / 10.0f;

			if (m_fXS > 4.0f)
				m_fXS = 4.0f;
			else if (m_fXS < -4.0f)
				m_fXS = -4.0f;

			if (m_fYS > 4.0f)
				m_fYS = 4.0f;
			else if (m_fYS < -4.0f)
				m_fYS = -4.0f;
		}

		if (m_pStake == nullptr)
		{
			m_fYS = -2.5f;
			if (m_bFlip)
				m_fXS = -1.0f;
			else
				m_fXS = 1.0f;
		}
		break;
	case STATE_FLY:
		if (m_bFlip)
			m_fXS = -0.5f;
		else
			m_fXS = 0.5f;
		break;
	case STATE_RETREAT:
		if (m_pStake)
		{
			m_fYS = -2.0f;
			m_fXS = (m_pStake->m_fX - m_fX) / 24.0f;
			m_fAngle = 0;
		}
		break;
	case STATE_PRESSED:		
	case STATE_PUSHEDUP:
	case STATE_BURNT:
		MakeChainDebris();

		EnemyObjectBase::ChangeState(nState, pInfo);
		return;
		break;
	case STATE_DEAD:
		UnlinkStake();
		EnemyObjectBase::ChangeState(nState, pInfo);
		break;
	default:
		EnemyObjectBase::ChangeState(nState, pInfo);
		return;
		break;
	}

	m_nNextState = nState;
}

void ChainChomp::OnChangeState(int nState)
{
	ChangeStateInfo *pInfo = nullptr;
	if (m_ChangeStateInfo.nNextState != -1)
		pInfo = &m_ChangeStateInfo;

	switch (nState)
	{
	case STATE_TRAMPOLINJUMP:
		m_fYS = -6.0f;
		UnlinkStake();
		break;
	}
}

void ChainChomp::OnIdle()
{
	if (m_bStake && m_pStake == nullptr)
	{
		m_pStake = CUR_STAGE->ReserveCreateGameObject(m_fX, m_fY, ENEMY_CHAINCHOMPSTAKE);
		((ChainChompStake*)m_pStake)->m_pChainChomp = this;
	}

	if (!m_bWinged)
		ChangeState(STATE_WALK);
	else
		ChangeState(STATE_FLY);
}

void ChainChomp::OnWalk()
{
	PositionChain();

	CheckStakeAvailable();

	if (m_bCollision[COLLISION_BOTTOM])
	{
		m_fY = m_ptCollision[COLLISION_BOTTOM].y;
	}
	else
	{
		OnApplyGravity(1, -1);
	}

	if (m_pStake)
	{
		if (m_bCollision[COLLISION_BOTTOM])
			m_fYS = -2.0f;

		if (CheckChainLimit())
		{
			if (m_pStake == nullptr)
				return;
			else
			{
				if (m_fX < m_pStake->m_fX)
				{
					m_fXS = 1.0f;
					m_bFlip = false;
				}

				if (m_fX > m_pStake->m_fX)
				{
					m_fXS = -1.0f;
					m_bFlip = true;
				}
			}
		}

		CheckReachWall();

		// Prevent Falling
		{
			SIZE s = GetSize();
			float fFront = m_fX + m_fXS;
			if (m_bFlip)
				fFront -= (s.cx / 4);
			else
				fFront += (s.cx / 4);

			POINT ptNextGround = {
				(long)((fFront) / TILE_XS),
				(long)((m_pStake->m_fY) / TILE_YS),
			};

			int nNextGroundData = m_pStage->GetTileData(ptNextGround.x, ptNextGround.y);
			if (m_pStage->GetDataType(nNextGroundData) == TILETYPE_EMPTY)
			{
				m_bFlip = !m_bFlip;
				m_fXS *= -1;
			}

			if (m_nStateFrame > 120 && abs(m_pStake->m_fX - m_fX) < 16)
			{
				ChangeState(STATE_STAND);
			}
		}
	}
	else
	{
		ChangeState(STATE_JUMP);

		CheckReachWall();
	}
}

void ChainChomp::OnFly()
{
	PositionChain();

	CheckStakeAvailable();

	if (m_pStake)
	{
		if (m_nStateFrame < 24 && m_fY + 16 > m_pStake->m_fY)
			m_fYS = -2.0f;
		else
			m_fYS = -sin(m_nStateFrame * 2 * fToRadian) * 0.5f;

		if (CheckChainLimit())
		{
			if (m_fX < m_pStake->m_fX)
			{
				m_fXS = 0.5f;
				m_bFlip = false;
			}

			if (m_fX > m_pStake->m_fX)
			{
				m_fXS = -0.5f;
				m_bFlip = true;
			}
		}

		if (m_nStateFrame > 120 && abs(m_pStake->m_fX - m_fX) < 16)
		{
			ChangeState(STATE_STAND);
		}
	}
	else
	{
		m_fYS = -sin(m_nStateFrame * 2 * fToRadian) * 0.5f;
	}
}

void ChainChomp::OnStand()
{
	PositionChain();

	if (!m_bWinged)
	{
		if (m_bCollision[COLLISION_BOTTOM])
		{
			m_fY = m_ptCollision[COLLISION_BOTTOM].y;
		}
		else
		{
			OnApplyGravity(1, -1);
		}
	}

	if (m_nStateFrame == 36)
	{
		ChangeState(STATE_JUMP);
	}
}

void ChainChomp::OnJump()
{
	PositionChain();

	CheckStakeAvailable();

	if (m_pStake)
	{
		CheckChainLimit();

		if (m_nStateFrame == 140)
			ChangeState(STATE_RETREAT);
	}
	else
	{
		if (m_bCollision[COLLISION_BOTTOM])
		{
			m_fY = m_ptCollision[COLLISION_BOTTOM].y;
			m_fYS = 0;

			ChangeState(STATE_WALK);
		}
		else
		{
			OnApplyGravity(0.25f, -1);
		}

		if (CheckHitBlock(0))
			return;

		CheckReachWall();
	}
}

void ChainChomp::OnRetreat()
{
	PositionChain();

	CheckStakeAvailable();

	if (m_pStake)
	{
		if (m_fY < m_pStake->m_fY)
		{
			OnApplyGravity(1, -1);
		}

		if (m_fY > m_pStake->m_fY)
		{
			if (m_fYS > -4.0f)
				m_fYS -= -1.0f;
		}

		if (abs(m_fX - m_pStake->m_fX) < 4)
		{
			m_fXS = 0;
		}

		if (abs(m_fY - m_pStake->m_fY) < 4)
		{
			m_fYS = 0;
			m_fY = m_pStake->m_fY;
		}
	}
	
	// #TODO Check bug fixed

	if (m_nStateFrame >= 24 && m_fXS == 0 && m_fYS == 0)
	{
		if (!m_bWinged)
			ChangeState(STATE_WALK);
		else
			ChangeState(STATE_FLY);
	}
}

void ChainChomp::OnTrampolinJump()
{
	PositionChain();

	EnemyObjectBase::OnTrampolinJump();
}

void ChainChomp::OnFall()
{
	PositionChain();

	if (m_bWinged)
		ChangeState(STATE_FLY);
	else
		EnemyObjectBase::OnFall();
}

bool ChainChomp::CheckChainLimit()
{
	if (m_pStake)
	{
		NaPointT<float> ptBegin;
		ptBegin.x = m_pStake->m_fX;
		ptBegin.y = m_pStake->m_fY - 2;

		float fDistX = (m_fX - ptBegin.x);
		float fDistY = -(m_fY - 4 - ptBegin.y);
		float fDist = sqrt(fDistX * fDistX + fDistY * fDistY);
		if (fDist > 16 * 3)
		{
			m_fXS = 0;
			m_fYS = 0;

			//float fRatio = (fDist) / (16.0f * 3.0f);
			/*
			if (fDistX > 0)
				m_bFlip = false;
			else
				m_bFlip = true;
			*/

			if (fDist > 16 * 4)
			{
				// Hit
				CreateParameter param;
				CUR_STAGE->CreateEffect(m_pStake->m_fX, m_pStake->m_fY - 8, EFFECT_HIT, &param);

				UnlinkStake();

				SOUND_MANAGER->PlaySoundEffect(L"Kicked");
			
			}
			return true;

		}

		//NaDebugOut(L"Dist: %.2f\n", fDist);
	}

	return false;
}

void ChainChomp::MakeChainDebris()
{
	for (int i = 0; i < 4; i++)
	{
		CreateParameter param;
		param.fScaleX = m_fScaleX;
		param.fScaleY = m_fScaleY;
		CUR_STAGE->CreateEffect(m_ptChain[i].x, m_ptChain[i].y, EFFECT_CHAINDEBRIS, &param);
	}
}

void ChainChomp::CheckStakeAvailable()
{
	if (m_pStake && !m_pStake->IsAlive())
	{
		m_pStake = nullptr;
		m_fAngle = 0;
	}
}

void ChainChomp::PositionChain()
{
	if (m_pStake)
	{
		NaPointT<float> ptBegin;
		ptBegin.x = m_pStake->m_fX;
		ptBegin.y = m_pStake->m_fY - 2;

		float fDistX = ptBegin.x - m_fX;
		float fDistY = ptBegin.y - (m_fY - 4);
		float fDist = sqrt(fDistX * fDistX + fDistY * fDistY);

		for (int i = 0; i < 4; i++)
		{
			float fX = ptBegin.x - (fDistX / 4.0f) * i;
			float fY = ptBegin.y - (fDistY / 4.0f) * i;

			if (i == 1 || i == 2)
			{
				if (fDist < (16 * 3) - 8)
				{
					if (fY < m_pStake->m_fY)
						fY += 4;
				}
				else if (fDist < 16 * 3)
				{
					if (fY < m_pStake->m_fY)
						fY += 2;
				}

				if (m_nState == STATE_JUMP && m_nStateFrame > 10 && m_nStateFrame % 2 == 0)
					fY++;
			}

			m_ptChain[i].x = fX;
			m_ptChain[i].y = fY;
		}
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			NaPointT<float> ptOld;
			if (i == 0)
				ptOld = { m_fX, m_fY - 4 };
			else
				ptOld = m_ptChain[i - 1];

			if (m_ptChain[i].x == 0 && m_ptChain[i].y == 0)
				m_ptChain[i] = ptOld;

			float fDistX = m_ptChain[i].x - ptOld.x;
			float fDistY = m_ptChain[i].y - ptOld.y;
			float fDist = sqrt(fDistX * fDistX + fDistY * fDistY);

			if (fDist > 12)
			{
				float fFactor = fDist / 12.0f;
				m_ptChain[i].x = ptOld.x + (fDistX / fFactor);
				m_ptChain[i].y = ptOld.y + (fDistY / fFactor);
			}
			
			if (!m_bWinged)
			{
				if (CUR_STAGE->IsHardTile(
					m_ptChain[i].x / TILE_XS, (m_ptChain[i].y + 1) / TILE_YS) == false)
				{
					m_ptChain[i].y += 1;
				}
			}
		}
	}
}

void ChainChomp::UnlinkStake()
{
	if (m_pStake)
	{
		((ChainChompStake*)m_pStake)->m_pChainChomp = nullptr;
		m_pStake = nullptr;
	}
	m_fAngle = 0;
}

bool ChainChomp::IsAlive()
{
	return EnemyObjectBase::IsAlive();
}

bool ChainChomp::CanPressed(int nPower)
{
	switch (nPower)
	{
	case POWER_PRESS_GOOMBASHOES:
		return true;
		break;
	}

	return false;
}

bool ChainChomp::CanBurnt(int nType)
{
	switch (nType)
	{
	case BURN_TURTLESHELL:
	case BURN_BOBOMB:
	case BURN_STARMAN:
	case BURN_POWBLOCK:
		return true;
		break;
	}

	return false;
}

std::vector<POINT> ChainChomp::GetWingPosition()
{
	POINT pt;
	SIZE s = GetSize();
	pt.x = -8;
	pt.y = -5;

	std::vector<POINT> vec;
	vec.push_back(pt);

	return vec;
}

int ChainChomp::GetSpriteIndex()
{
	if (EnemyObjectBase::GetSpriteIndex() == -1)
		return -1;

	int nFrame;
	switch (m_nState)
	{
	case STATE_PRESSED:
	case STATE_PUSHEDUP:
	case STATE_BURNT:
		nFrame = SPRIDX_CHAINCHOMP2;
		break;
	case STATE_STAND:
		nFrame = SPRIDX_CHAINCHOMP1;
		break;
	case STATE_JUMP:
		if (m_pStake)
			nFrame = SPRITE_INDEX_CYCLE(SPRIDX_CHAINCHOMP1, 2, 4);
		else
			nFrame = SPRITE_INDEX_CYCLE(SPRIDX_CHAINCHOMP1, 2, 8);
		break;
	default:
		nFrame = SPRITE_INDEX_CYCLE(SPRIDX_CHAINCHOMP1, 2, 8);
		break;
	}

	return nFrame;
}
