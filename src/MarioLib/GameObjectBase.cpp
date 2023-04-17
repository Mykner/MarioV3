#include "GameObjectBase.h"

#include "GameObjectStates.h"

#include "Game.h"
#include "Stage.h"
#include "Player.h"
#include "Camera.h"
#include "FModManager.h"
#include "Sprite.h"
#include "PipeInfo.h"
#include "SoundManager.h"

#include "EnemyObjectBase.h"
#include "ItemObjectBase.h"
#include "EffectObjectBase.h"
#include "EventObjectBase.h"
#include "VehicleObjectBase.h"
#include "BulletObjectBase.h"

#include "Mario.h"
#include "NewMario.h"

#include "UIBase.h"

#include "TypeNames.h"

GameObjectBase::GameObjectBase(Game *pGame, Stage *pStage) :
	PropertyObjectBase()
{
	m_pGame = pGame;
	m_pStage = pStage;
	m_pController = nullptr;

	m_pSprite = nullptr;
	m_ppSprite = nullptr;

	m_fX = 0;
	m_fY = 0;
	m_fXS = 0;
	m_fYS = 0;
	m_fScaleX = 1.0f;
	m_fScaleY = 1.0f;
	m_bFlip = false;
	m_bFlipV = false;

	m_nType = -1;
	m_nState = STATE_IDLE;
	m_nNextState = -1;
	m_nBaseState = STATE_STAND;
	m_nStateFrame = 0;
	m_nSpriteIndex = 0;

	m_nDamageGroup = DAMAGEGROUP_ENEMY;
	m_nDamageCooltime = 0;
	m_nInvinsibleTime = 0;
	m_nGiantTime = 0;
	m_nImmuneTime = 0;

	m_bReachFriend = false;
	m_pParent = nullptr;
	m_pVehicle = nullptr;
	m_pCarry = nullptr;
	m_pKickedBy = nullptr;

	m_nShape = SHAPE_MINI;
	m_nNextShape = -1;
	
	m_bPowerUp = false;
	m_bWinged = false;

	m_nCombo = 0;
	m_bDamagedByMap = false;
	m_bStandOnJumpBlock = false;

	RequestSprites(pStage);
}

GameObjectBase::~GameObjectBase()
{
	
}

void GameObjectBase::RequestSprites(Stage * pStage)
{
	if (pStage == nullptr)
		pStage = CUR_STAGE;

	if (m_nType >= ENEMY_BEGIN && m_nType <= ENEMY_END)
		m_ppSprite = &pStage->m_pSpriteEnemy;
	else if (m_nType >= ITEM_BEGIN && m_nType <= ITEM_END)
		m_ppSprite = &pStage->m_pSpriteItem;
	else if (m_nType >= VEHICLE_BEGIN && m_nType <= VEHICLE_END)
		m_ppSprite = &pStage->m_pSpriteVehicle;
	else if (m_nType >= EVENTOBJECT_BEGIN && m_nType <= EVENTOBJECT_END)
		m_ppSprite = &pStage->m_pSpriteMapObject;
	else if (m_nType >= EFFECT_BEGIN && m_nType <= EFFECT_END)
		m_ppSprite = &pStage->m_pSpriteEffect;
}

bool GameObjectBase::HitTest(int x, int y)
{
	NaRect rcMe = GetRect();
	if (rcMe.ptInRect(x, y))
		return true;

	return false;
}

void GameObjectBase::CheckCollision()
{
	memset(&m_bCollision, 0, sizeof(m_bCollision));
	m_bDamagedByMap = false;
	m_bStandOnJumpBlock = false;

	switch (m_nState)
	{
	case STATE_ITEMPOP:
		if (m_nStateFrame < 30)
			return;
		break;
	}

	m_fX += m_fXS;
	m_fY += m_fYS;

	NaRect rc = GetRect();
	NaRect rcTile = GetTileRect();
	int x, y;

	// For Debug HitBox
	///*
//#define DEBUG_SHOW_STAGE_COLLISION

#ifdef DEBUG_SHOW_STAGE_COLLISION
#define SHOW_STAGETILEBOX	{ NaRect rcTemp = rcStageTile; rcTemp.Offset(-CAMERA->m_ptPos.x, -CAMERA->m_ptPos.y); u.RenderBox(rcTemp, 0x600000ff); }
#define SHOW_STAGETILEBOX2(c)	{ NaRect rcTemp = rcStageTile; rcTemp.Offset(-CAMERA->m_ptPos.x, -CAMERA->m_ptPos.y); u.RenderBox(rcTemp, c); }
	UIBase u(m_pGame);
#else
#define SHOW_STAGETILEBOX		{}
#define SHOW_STAGETILEBOX2(c)	{}
#endif

	// #TODO Must pass hidden block if i'm neutral object.

	// Check Top
	for (int i = rcTile.left; i <= rcTile.right; i++)
	{
		x = i;
		y = rcTile.top;
		if ((CUR_STAGE->IsHardTile(x, y) || (m_fYS < 0 && CUR_STAGE->HasItem(x, y))) && 
			!CUR_STAGE->IsHardTile(x, y + 1))
		{
			NaRect rcStageTile = CUR_STAGE->GetTileRect(x, y);
			SHOW_STAGETILEBOX2(0x70000000);
			if (m_bCollision[COLLISION_TOP])
			{
				// Store best near tile
				if (abs((m_ptCollision[COLLISION_TOP].x) + (TILE_XS / 2) - m_fX) >
					abs((x * TILE_XS) + (TILE_XS / 2) - m_fX)) 
					m_ptCollision[COLLISION_TOP] = { (float)(x * TILE_XS), (float)(y * TILE_YS + TILE_YS - 1) };
			}
			else if (rc.IsOverlapped(rcStageTile))
			{
				SHOW_STAGETILEBOX2(0xb0ff0000);
				m_bCollision[COLLISION_TOP] = true;
				m_ptCollision[COLLISION_TOP] = { (float)(x * TILE_XS), (float)(y * TILE_YS + TILE_YS - 1) };
			}
		}

		if (CUR_STAGE->IsDamageTile(x, y) && m_nDamageCooltime == 0)
			m_bDamagedByMap = true;
	}

	// Check Left,Right
	rc.bottom -= 1;
	for (int i = rcTile.top; i <= rcTile.bottom; i++)
	{
		x = rcTile.left;
		y = i;

		// ignore near underside
		//if (y * 16 > m_fY - 8)
		if (y * 16 > m_fY - 4)
			continue;

		NaRect rcStageTile = CUR_STAGE->GetTileRect(x, y, true);
		if (CUR_STAGE->IsHardTile(x, y) && !CUR_STAGE->IsHardTile(x + 1, y))
		{
			SHOW_STAGETILEBOX2(0x70000000);
			if (rc.IsOverlapped(rcStageTile))
			{
				SHOW_STAGETILEBOX2(0xb0ff0000);
				m_bCollision[COLLISION_LEFT] = true;
				m_ptCollision[COLLISION_LEFT] = { (float)rcStageTile.right - 1, (float)(y * TILE_YS) };
			}
		}

		if (CUR_STAGE->IsDamageTile(x, y) && m_nDamageCooltime == 0)
			m_bDamagedByMap = true;

		x = rcTile.right;
		rcStageTile = CUR_STAGE->GetTileRect(x, y, true);
		if (CUR_STAGE->IsHardTile(x, y) && !CUR_STAGE->IsHardTile(x - 1, y))
		{
			SHOW_STAGETILEBOX2(0x70000000);
			if (rc.IsOverlapped(rcStageTile))
			{
				SHOW_STAGETILEBOX2(0xb0ff0000);
				m_bCollision[COLLISION_RIGHT] = true;
				m_ptCollision[COLLISION_RIGHT] = { (float)rcStageTile.left, (float)(y * TILE_YS) };
			}
		}

		if (CUR_STAGE->IsDamageTile(x, y) && m_nDamageCooltime == 0)
			m_bDamagedByMap = true;
	}
	rc.bottom += 1;

	// Check Bottom
	NaRect rcFoot = rc;
	rcFoot.top = rcFoot.bottom - rcFoot.Height() / 2;
	for (int i = rcTile.left; i <= rcTile.right; i++)
	{
		x = i;
		y = rcTile.bottom;

		if (!CanStandOnSpike() && CUR_STAGE->IsDamageTile(x, y) && m_nDamageCooltime == 0)
			m_bDamagedByMap = true;

		if (m_fYS >= 0 && CUR_STAGE->IsJumpTile(x, y) && !CUR_STAGE->IsHardTile(x, y - 1, COLLISION_TOP))
		{
			NaRect rcStageTile = CUR_STAGE->GetTileRect(x, y, true);
			if (rcStageTile.top + 8 < rcFoot.bottom)
				continue;

			SHOW_STAGETILEBOX2(0x70000000);
			if (m_bCollision[COLLISION_BOTTOM])
			{
				if (abs((m_ptCollision[COLLISION_BOTTOM].x) + (TILE_XS / 2) - m_fX) >
					abs((x * TILE_XS) + (TILE_XS / 2) - m_fX))
					m_ptCollision[COLLISION_BOTTOM] = { (float)(x * TILE_XS), (float)(y * TILE_YS) };
			}
			else if (rcFoot.IsOverlapped(rcStageTile))
			{
				SHOW_STAGETILEBOX2(0xb00000ff);
				m_bCollision[COLLISION_BOTTOM] = true;
				m_ptCollision[COLLISION_BOTTOM] = { (float)(x * TILE_XS), (float)(y * TILE_YS) };

				m_bStandOnJumpBlock = true;
			}
		}
		else if (CUR_STAGE->IsHardTile(x, y, COLLISION_BOTTOM) && !CUR_STAGE->IsHardTile(x, y - 1))
		{
			NaRect rcStageTile = CUR_STAGE->GetTileRect(x, y);
			if (rcStageTile.top + 8 < rcFoot.bottom)
				continue;

			SHOW_STAGETILEBOX2(0xb0ff0000);
			if (rcFoot.IsOverlapped(rcStageTile))
			{
				SHOW_STAGETILEBOX2(0xb0ff0000);
				rcStageTile = CUR_STAGE->GetTileRect(x, y);
				m_bCollision[COLLISION_BOTTOM] = true;
				m_ptCollision[COLLISION_BOTTOM] = { (float)(x * TILE_XS), (float)(y * TILE_YS) };

				m_bStandOnJumpBlock = false;
				break;
			}
		}	
	}

	// Check EventObject as tile block
	{
		auto rcF = GetRectFloat();

		std::vector<GameObjectBase*> vecCollidable;
		PASTE_VECTOR(vecCollidable, CUR_STAGE->m_vecEventObject);
		PASTE_VECTOR(vecCollidable, CUR_STAGE->m_vecEnemy);

		BEGIN_VECTOR_LOOP(GameObjectBase*, vecCollidable) {
			if (pObj == this)
				continue; 

			//pObj->m_fX += pObj->m_fXS;
			//pObj->m_fY += pObj->m_fYS;

			auto rcObj = pObj->GetRectFloat();
			if (CanCollisionWith(pObj) && rcF.IsOverlapped(rcObj))
			{
				// Carry object
				if (!(pObj->m_nType >= ENEMY_BEGIN && pObj->m_nType <= ENEMY_END) &&
					CanCarry() && pObj->CanCarried())
				{
					Carry(pObj);
					continue;
				}

				if (pObj == m_pCarry)
					continue;

				// obj is over me
				if (abs(rcF.top - rcObj.bottom) < (2 + abs(m_fYS)))
				{
					if (pObj->CanCollide(COLLISION_BOTTOM))
					{
						m_bCollision[COLLISION_TOP] = true;
						m_ptCollision[COLLISION_TOP] = { m_fX, rcObj.bottom };
					}
				}
				// obj is under me
				//if (abs(rcF.bottom - rcObj.top) < (2 + abs(m_fYS)) && m_fYS >= 0)
				if ((rcF.bottom - rcObj.top) < ((rcF.Height() * 0.2f) + (m_fYS)) && m_fYS >= 0)
				{
					if (pObj->CanCollide(COLLISION_TOP))
					{
						m_bCollision[COLLISION_BOTTOM] = true;
						m_ptCollision[COLLISION_BOTTOM] = { m_fX, rcObj.top };
					}

					if (IsAlive() && pObj->CanLand())
					{
						bool bLand = true;

						if (m_bCollision[COLLISION_BOTTOM])
						{
							if (m_ptCollision[COLLISION_BOTTOM].y < rcObj.top)
							{
								bLand = false;
							}
						}

						if (bLand)
						{
							m_bCollision[COLLISION_BOTTOM] = true;
							m_ptCollision[COLLISION_BOTTOM] = { m_fX, rcObj.top + 0.5f };

							pObj->AddPassenger(this);
						}
					}
				}
				// obj is on my left
				//if (abs(rcF.left - rcObj.right) < (2 + abs(m_fXS)) && rcF.bottom - 4 >= rcObj.top)
				if (abs(rcF.left - rcObj.right) < ((rcF.Width() * 0.25f) + abs(m_fXS)) && rcF.bottom - 4 >= rcObj.top)
				{
					if (pObj->CanCollide(COLLISION_RIGHT))
					{
						m_bCollision[COLLISION_LEFT] = true;
						m_ptCollision[COLLISION_LEFT] = { rcObj.right, m_fY };
					}
				}
				// obj is on my right
				//if (abs(rcF.right - rcObj.left) < (2 + abs(m_fXS)) && rcF.bottom - 4 >= rcObj.top)
				if (abs(rcF.right - rcObj.left) < ((rcF.Width() * 0.25f) + abs(m_fXS)) && rcF.bottom - 4 >= rcObj.top)
				{
					if (pObj->CanCollide(COLLISION_LEFT))
					{
						m_bCollision[COLLISION_RIGHT] = true;
						m_ptCollision[COLLISION_RIGHT] = { rcObj.left, m_fY };
					}
				}
			}

			//pObj->m_fX -= pObj->m_fXS;
			//pObj->m_fY -= pObj->m_fYS;
		} END_VECTOR_LOOP();
	}

	m_fX -= m_fXS;
	m_fY -= m_fYS;
}

void GameObjectBase::CheckCollisionPlayer()
{
	m_bCollisionPlayer = false;
	if ((GameObjectBase*)CUR_PLAYER_OBJ == this)
		return;

	NaRect rcMe = GetRect();
	NaRect rcPlayer = CUR_PLAYER_OBJ->GetRect();
	if (rcMe.IsOverlapped(rcPlayer))
	{
		m_bCollisionPlayer = true;
	}
}

void GameObjectBase::CheckCollisionEnemy()
{
	if (!IsAlive())
		return;

	m_fX += m_fXS;
	m_fY += m_fYS;

	auto rcF = GetRectFloat();

	m_vecPressedEnemy.clear();
	m_vecOverlappedEnemy.clear();
	std::vector<GameObjectBase*>::iterator it = CUR_STAGE->m_vecEnemy.begin();
	for (; it != CUR_STAGE->m_vecEnemy.end(); ++it)
	{
		EnemyObjectBase *pEnemy = (EnemyObjectBase*)*it;
		if (pEnemy == this)
			continue;
		if (!pEnemy->IsAlive())
			continue;
		if (pEnemy->m_nState == STATE_IDLE && pEnemy->m_nStateFrame == 0)
			continue;
		if (abs(m_fX - pEnemy->m_fX) > GameDefaults::nPageWidth)
			continue;

		//auto rcObj = pEnemy->GetRectFloat();
		//if (rcF.IsOverlapped(rcObj))
		if (IsOverlapped(pEnemy))
		{
			NaRect rcEnemy = pEnemy->GetRect();
			if (!IsOpponent(pEnemy))
			{
				if (m_fY - rcEnemy.top < 2)
					continue;
				if (CanDamageFriend())
				{
					m_vecOverlappedEnemy.push_back(pEnemy);
				}
				else if (((m_fXS > 0 && m_fX < pEnemy->m_fX) || (m_fXS < 0 && m_fX > pEnemy->m_fX)) &&
					pEnemy->CanDamageFriend() == false)
				{
					m_bReachFriend = true;
				}
			}
			else
			{
				if (m_nInvinsibleTime > 0)
				{
					m_vecOverlappedEnemy.push_back(pEnemy);
				}
				else if (CanPressEnemy() && 
					pEnemy->CanPressed(GetPower(POWERTYPE_PRESS)) &&
					m_fY - rcEnemy.top < rcEnemy.Height() / 2)// && m_fYS > 0)
				{
					m_vecPressedEnemy.push_back(pEnemy);
				}
				else
				{
					m_vecOverlappedEnemy.push_back(pEnemy);
				}
			}
		}
	}

	m_fX -= m_fXS;
	m_fY -= m_fYS;
}

void GameObjectBase::CheckCollisionVehicle()
{
	if (!IsAlive())
		return;

	m_fX += m_fXS;
	m_fY += m_fYS;

	NaRect rcMe = GetRect();

	m_vecRidableVehicle.clear();
	std::vector<GameObjectBase*>::iterator it = CUR_STAGE->m_vecVehicle.begin();
	for (; it != CUR_STAGE->m_vecVehicle.end(); ++it)
	{
		GameObjectBase *pObj = *it;
		if (pObj == this)
			continue;
		if (!pObj->IsAlive())
			continue;
		if (pObj->m_nState == STATE_IDLE && pObj->m_nStateFrame == 0)
			continue;
		if (abs(m_fX - pObj->m_fX) > GameDefaults::nPageWidth)
			continue;

		if (IsOverlapped(pObj))
		{
			NaRect rcObj = pObj->GetRect();
			if (!IsOpponent(pObj) &&
				m_fY - rcObj.top < rcObj.Height() / 2 &&
				pObj->CanRided(this))
			{
				m_vecRidableVehicle.push_back(pObj);
			}
		}
	}

	m_fX -= m_fXS;
	m_fY -= m_fYS;
}

bool GameObjectBase::IsOverlapped(GameObjectBase * pOther)
{
	std::vector<NaRect> vecRcMe = GetRectList();
	std::vector<NaRect> vecRcOther = pOther->GetRectList();

	std::vector<NaRect>::iterator it1 = vecRcMe.begin();
	for (; it1 != vecRcMe.end(); ++it1)
	{
		std::vector<NaRect>::iterator it2 = vecRcOther.begin();
		for (; it2 != vecRcOther.end(); ++it2)
		{
			if ((*it1).IsOverlapped(*it2))
				return true;
		}
	}

	return false;
}

bool GameObjectBase::CanCollisionWith(GameObjectBase * pOther)
{
	return true;
}

POINT GameObjectBase::FindCollidableOffset(int nCollisionDir, int nMax /*= 32*/)
{
	POINT ptOffset = { 0, 0 };

	NaRect rc = GetRect();
	NaRect rcTile = GetTileRect();
	int x, y;

	// Check Top
	if (nCollisionDir == COLLISION_TOP)
	{
		for (int j = 0; j < nMax; j++)
		{
			rc.Offset(0, -1);
			for (int i = rcTile.left; i <= rcTile.right; i++)
			{
				x = i;
				y = (rc.top - j) / TILE_YS;
				if ((CUR_STAGE->IsHardTile(x, y) || (m_fYS < 0 && CUR_STAGE->HasItem(x, y))) &&
					!CUR_STAGE->IsHardTile(x, y + 1))
				{
					NaRect rcStageTile = CUR_STAGE->GetTileRect(x, y);
					if (rc.IsOverlapped(rcStageTile))
					{
						ptOffset.y = j;
						return ptOffset;
					}
				}
			}
		}

		ptOffset.y = nMax + 1;
		return ptOffset;
	}

	// Check Left
	if (nCollisionDir == COLLISION_LEFT)
	{
		for (int j = 0; j < nMax; j++)
		{
			for (int i = rcTile.top; i <= rcTile.bottom; i++)
			{
				x = (rc.left - j) / TILE_XS;
				y = i;

				if (y * 16 > m_fY - 4)
					continue;

				NaRect rcStageTile = CUR_STAGE->GetTileRect(x, y);
				if (CUR_STAGE->IsHardTile(x, y))
				{
					ptOffset.x = -j;
					return ptOffset;
				}
			}
		}

		ptOffset.x = -nMax;
		return ptOffset;
	}

	if (nCollisionDir == COLLISION_RIGHT)
	{
		for (int j = 0; j < nMax; j++)
		{
			for (int i = rcTile.top; i <= rcTile.bottom; i++)
			{
				x = (rc.right + j) / TILE_XS;
				y = i;

				if (y * 16 > m_fY - 4)
					continue;

				NaRect rcStageTile = CUR_STAGE->GetTileRect(x, y);
				if (CUR_STAGE->IsHardTile(x, y))
				{
					ptOffset.x = j;
					return ptOffset;
				}
			}
		}

		ptOffset.x = nMax;
		return ptOffset;
	}

	// Check Bottom
	/*
	NaRect rcFoot = rc;
	rcFoot.top = rcFoot.bottom - rcFoot.Height() / 2;
	for (int i = rcTile.left; i <= rcTile.right; i++)
	{
		x = i;
		y = rcTile.bottom;

		if (CUR_STAGE->IsDamageTile(x, y) && m_nDamageCooltime == 0)
			m_bDamagedByMap = true;

		if (m_fYS >= 0 && CUR_STAGE->IsJumpTile(x, y) && !CUR_STAGE->IsHardTile(x, y - 1, COLLISION_BOTTOM))
		{
			NaRect rcStageTile = CUR_STAGE->GetTileRect(x, y, true);
			if (rcStageTile.top + 8 < rcFoot.bottom)
				continue;

			if (m_bCollision[COLLISION_BOTTOM])
			{
				if (abs((m_ptCollision[COLLISION_BOTTOM].x) + (TILE_XS / 2) - m_fX) >
					abs((x * TILE_XS) + (TILE_XS / 2) - m_fX))
					m_ptCollision[COLLISION_BOTTOM] = { (float)(x * TILE_XS), (float)(y * TILE_YS) };
			}
			else if (rcFoot.IsOverlapped(rcStageTile))
			{
				m_bCollision[COLLISION_BOTTOM] = true;
				m_ptCollision[COLLISION_BOTTOM] = { (float)(x * TILE_XS), (float)(y * TILE_YS) };

				m_bStandOnJumpBlock = true;
			}
		}
		else if (CUR_STAGE->IsHardTile(x, y, COLLISION_BOTTOM) && !CUR_STAGE->IsHardTile(x, y - 1))
		{
			NaRect rcStageTile = CUR_STAGE->GetTileRect(x, y);
			if (rcStageTile.top + 8 < rcFoot.bottom)
				continue;

			if (rcFoot.IsOverlapped(rcStageTile))
			{
				rcStageTile = CUR_STAGE->GetTileRect(x, y);
				m_bCollision[COLLISION_BOTTOM] = true;
				m_ptCollision[COLLISION_BOTTOM] = { (float)(x * TILE_XS), (float)(y * TILE_YS) };

				m_bStandOnJumpBlock = false;
				break;
			}
		}
	}
	*/

	return ptOffset;
}

bool GameObjectBase::CheckHitBlock(int nPower, GameObjectBase *pWho)
{
	if (m_bCollision[COLLISION_TOP])
	{
		// Something on my head
		float fRangeX = (m_ptCollision[COLLISION_TOP].x + (TILE_XS / 2)) - m_fX;
		if (abs(fRangeX) >= 12.0f)
		{
			if (abs(fRangeX) < 16.0f)
			{
				// Slide
				if (fRangeX > 0)
					m_fX--;
				else
					m_fX++;
			}
		}
		else
		{
			// Hit the block
			NaRect rc = GetRect();
			m_fYS = 0;
			m_fY = ((m_ptCollision[COLLISION_TOP].y + 1)) + rc.Height();

			if (m_nState != STATE_SHELLRUN &&
				m_nState != STATE_SPINJUMP)
				ChangeState(STATE_FALL);

			if (nPower > POWER_HITBLOCK_UNKNOWN)
			{
				if (pWho == nullptr)
					pWho = this;
				CUR_STAGE->HitBlock(
					m_ptCollision[COLLISION_TOP].x / TILE_XS,
					(m_ptCollision[COLLISION_TOP].y) / TILE_YS, 
					nPower, 
					pWho, 
					m_pKickedBy ? m_pKickedBy : pWho);
			}
			return true;
		}
	}

	return false;
}

bool GameObjectBase::CheckReachWall()
{
	if (m_fXS < 0 && m_bCollision[COLLISION_LEFT])
	{
		OnReachWall(true);
		return true;
	}
	else if (m_fXS > 0 && m_bCollision[COLLISION_RIGHT])
	{
		OnReachWall(false);
		return true;
	}

	return false;
}

bool GameObjectBase::CheckPressEnemy(int nPower)
{
	if (nPower == -1)
		nPower = GetPower(POWERTYPE_PRESS);

	bool bPressSomething = false;
	std::vector<GameObjectBase*>::iterator it = m_vecPressedEnemy.begin();
	for (; it != m_vecPressedEnemy.end(); ++it)
	{
		GameObjectBase* pEnemy = *it;

		// #TODO move to own class...
		/*
		if (nPower == POWER_PRESS_YOSHI && pEnemy->m_nInvinsibleTime == 0)
		{
			// Stomp
			ChangeStateInfo info;
			info.nNextState = STATE_PRESSED;
			info.pFromObject = this;
			info.nPower = nPower;
			pEnemy->ChangeState(STATE_PRESSED, &info);

			bPressSomething = true;
		}
		else*/
		if (CanCarry() && pEnemy->CanCarried())
		{
			Carry(pEnemy);
		}
		else if (pEnemy->CanKicked())
		{
			Kick(pEnemy, false);
		}
		else if (pEnemy->m_nInvinsibleTime == 0)
		{
			ChangeStateInfo info;
			info.nNextState = STATE_PRESSED;
			info.pFromObject = this;
			info.nPower = nPower;
			pEnemy->ChangeState(STATE_PRESSED, &info);

			bPressSomething = true;
		}
	}

	return bPressSomething;
}

bool GameObjectBase::CheckRideVehicle()
{
	if (m_pVehicle)
		return false;

	if (m_nShape == SHAPE_GIANT)
		return false;

	std::vector<GameObjectBase*>::iterator it = m_vecRidableVehicle.begin();
	for (; it != m_vecRidableVehicle.end(); ++it)
	{
		GameObjectBase* pObj = *it;
		if (pObj->m_nDamageCooltime > 0)
			continue;
		
		if (!pObj->IsAlive())
			continue;

		m_pVehicle = (VehicleObjectBase*)pObj;
		m_pVehicle->OnRidePassenger(this);
		return true;
	}

	return false;
}

bool GameObjectBase::CheckFalling()
{
	if (m_bStandOnJumpBlock)
	{
		ChangeState(STATE_BLOCKJUMP);
		return true;
	}
	else if (!m_bCollision[COLLISION_BOTTOM])
	{
		ChangeState(STATE_FALL);
		return true;
	}
	else if (m_fY > m_ptCollision[COLLISION_BOTTOM].y)
	{
		m_fY = m_ptCollision[COLLISION_BOTTOM].y;
	}

	return false;
}

bool GameObjectBase::CheckMoveWithMe(GameObjectBase * pObj)
{
	bool bRet = false;
	NaRectT<float> rcObj = pObj->GetRectFloat();
	if (m_fXS < 0 && pObj->m_bCollision[COLLISION_LEFT] == true)
	{
		if (rcObj.left < pObj->m_ptCollision[COLLISION_LEFT].x)
		{
			pObj->m_fX = pObj->m_ptCollision[COLLISION_LEFT].x + (rcObj.Width() / 2);
			bRet = true;
		}
	}
	else if (m_fXS > 0 && pObj->m_bCollision[COLLISION_RIGHT] == true)
	{
		if (rcObj.right > pObj->m_ptCollision[COLLISION_RIGHT].x)
		{
			pObj->m_fX = pObj->m_ptCollision[COLLISION_RIGHT].x - (rcObj.Width() / 2);
			bRet = true;
		}
	}
	else
		pObj->m_fX += m_fXS;

	if (m_fYS < 0 && pObj->m_bCollision[COLLISION_TOP] == true)
	{
		if (rcObj.top < pObj->m_ptCollision[COLLISION_TOP].y)
		{
			pObj->m_fY = pObj->m_ptCollision[COLLISION_TOP].y + rcObj.Height();
			bRet = true;
		}
	}
	else if (m_fYS > 0 && pObj->m_bCollision[COLLISION_BOTTOM] == true)
	{
		if (rcObj.bottom > pObj->m_ptCollision[COLLISION_BOTTOM].y)
		{
			pObj->m_fY = pObj->m_ptCollision[COLLISION_BOTTOM].y;
			bRet = true;
		}
	}
	else
		pObj->m_fY += m_fYS;

	return bRet;
}

void GameObjectBase::PressJumpBlock()
{
	NaRect rc = GetTileRect();
	int y = rc.bottom;

	for (int x=rc.left; x<=rc.right; x++)
	{
		CUR_STAGE->PressJumpBlock(x, y);
	}
}

bool GameObjectBase::IsInputHold(int nInputType)
{
	if (!m_pController)
		return false;
	return m_pController->IsInputHold(nInputType);
}

bool GameObjectBase::IsInputPressed(int nInputType)
{
	if (!m_pController)
		return false;
	return m_pController->IsInputPressed(nInputType);
}

void GameObjectBase::Process()
{
	ProcessBase();
}

void GameObjectBase::ProcessBase()
{
	if (m_nNextState != -1)
		return;

	switch (m_nState)
	{
	case STATE_IDLE:		OnIdle();		break;
	case STATE_STAND:		OnStand();		break;
	case STATE_SIT:			OnSit();		break;
	case STATE_WALK:		OnWalk();		break;
	case STATE_BREAK:		OnBreak();		break;
	case STATE_JUMP:		OnJump();		break;
	case STATE_FALL:		OnFall();		break;
	case STATE_SWIM:		OnSwim();		break;
	case STATE_SWIMJUMP:	OnSwimJump();	break;
	case STATE_SPINJUMP:	OnSpinJump();	break;
	case STATE_GROUNDPOUND:	OnGroundPound();	break;

	case STATE_ITEMPOP:		OnItemPop();	break;
	case STATE_ITEMPOPDOWN:	OnItemPopDown();	break;
	case STATE_PIPEUP:		OnPipeUp();		break;
	case STATE_PIPEDOWN:	OnPipeDown();	break;
	case STATE_PIPELEFT:	OnPipeLeft();	break;
	case STATE_PIPERIGHT:	OnPipeRight();	break;

	case STATE_PUSHEDUP:	OnPushedUp();	break;
	case STATE_GOTOFFVEHICLE:	OnGotOffVehicle();	break;
	case STATE_PRESSED:		OnPressed();	break;
	case STATE_THROWN:		OnThrown();		break;
	case STATE_THROWNFLY:	OnThrownFly();	break;
	case STATE_TRAMPOLINPRESS:	OnTrampolinPress();	break;
	case STATE_TRAMPOLINJUMP:	OnTrampolinJump();	break;

	case STATE_BURNT:		OnBurnt();		break;
	case STATE_DIE:			OnDie();		break;
	case STATE_SPRING:		OnSpring();		break;
	case STATE_SHELLIDLE:	OnShellIdle();	break;
	case STATE_SHELLRUN:	OnShellRun();	break;
	case STATE_SHELLAWAKE:	OnShellAwake();	break;
	case STATE_MOVE:		OnMove();		break;
	case STATE_MOVEUP:		OnMoveUp();		break;
	case STATE_MOVEDOWN:	OnMoveDown();	break;
	case STATE_THROWFIRE:	OnThrowFire();	break;
	case STATE_JUMPINTO:	OnJumpInto();	break;

	case STATE_RIDE:		OnRide();		break;
	case STATE_FLY:			OnFly();		break;
	case STATE_HATCH:		OnHatch();		break;
	case STATE_SPITTED:		OnSpitted();	break;
	case STATE_SPITTED_SAFE:	OnSpittedSafe();	break;

	case STATE_STACKED:		OnStacked();	break;
	case STATE_BLOCKJUMP:	OnBlockJump();	break;
	case STATE_PRESSJUMP:	OnPressJump();	break;

	case STATE_FOLLOWPREPARE:	OnFollowPrepare();	break;
	case STATE_FOLLOW:		OnFollow();		break;
	case STATE_PARALYZE:	OnParalyze();	break;
	case STATE_RETREAT:		OnRetreat();	break;
	case STATE_SPECIALPOSE:	OnSpecialPose();	break;
	case STATE_VICTORYPOSE:	OnVictoryPose();	break;

	case STATE_EVENTDEAD:	OnEventDead();	break;
	
	case STATE_KICKED:		OnKicked();		break;
	case STATE_CARRYBYHAND:	OnCarryByHand();	break;
	case STATE_PLACEDBYHAND:	OnPlacedByHand();	break;
	case STATE_KICKEDUP:	OnKickedUp();	break;
	case STATE_EXPLODE:		OnExplode();	break;
	case STATE_POWED:		OnPowed();		break;
	}
}

void GameObjectBase::OnBeforeProcess()
{
	/*
	if (m_nNextState != -1)
	{
		// Affected by other
		m_nState = m_nNextState;
		m_nNextState = -1;
		m_nStateFrame = 0;
	}
	*/

	CheckCollision();
	CheckCollisionEnemy();
}

void GameObjectBase::OnAfterProcess()
{
	m_fX += m_fXS;
	m_fY += m_fYS;

	if (m_nDamageCooltime > 0)
	{
		m_nDamageCooltime--;
	}

	m_nStateFrame++;
	if (m_nNextState != -1)
	{
		m_nState = m_nNextState;
		m_nNextState = -1;
		m_nStateFrame = 0;

		OnChangeState(m_nState);
	}
}

void GameObjectBase::RenderWing(int nColor, int nZOrder)
{
	NaPointT<float> ptCamera = CAMERA->GetPosition();

	Sprite *pSprite = CUR_STAGE->m_pSpriteEnemy;
	int nFrame = GetWingSpriteIndex();
	bool bShadow = IS_MODERN;
	std::vector<POINT> vec = GetWingPosition();

	if (nZOrder != Z_BEHIND_MAP)
		nZOrder += 1;

	for (int i = 0; i < vec.size(); i++)
	{
		bool bFlip = m_bFlip;
		if (bFlip)
			vec[i].x *= -1;
		if (i % 2 == 1)
			bFlip = !bFlip;

		float fScaleX, fScaleY;
		GetScale(fScaleX, fScaleY);

		pSprite->RenderToQueue(
			m_fX + ((float)vec[i].x * fScaleX) - ptCamera.x,
			m_fY + ((float)vec[i].y * fScaleY) - ptCamera.y,
			nFrame,
			bFlip, false, nColor, fScaleX, fScaleY, nZOrder, bShadow);
	}
}

void GameObjectBase::RenderShapeChange()
{
	Render();
}

void GameObjectBase::ChangeState(int nState, ChangeStateInfo *pInfo)
{
	if (m_nNextState != -1)
		return;

	m_nNextState = nState;

	if (pInfo)
	{
		m_ChangeStateInfo = *pInfo;
		m_ChangeStateInfo.nNextState = nState;
	}
	else
	{
		m_ChangeStateInfo.nNextState = -1;
	}
}

void GameObjectBase::OnChangeState(int nState)
{
	ChangeStateInfo *pInfo = nullptr;
	if (m_ChangeStateInfo.nNextState != -1)
		pInfo = &m_ChangeStateInfo;

	switch (nState)
	{
	case STATE_JUMP:
		m_fYS = -4.0f;
		break;
	case STATE_PRESSJUMP:
		if (pInfo)
			m_fYS = -pInfo->nPower;
		else
			m_fYS = -4.0f;
		break;
	case STATE_PIPEDOWN:
		m_fXS = 0;
		m_fYS = 1.0f;
		break;
	case STATE_PIPERIGHT:
		m_fXS = 0.5f;
		m_fYS = 0;
		break;
	case STATE_PIPELEFT:
		m_fXS = -0.5f;
		m_fYS = 0;
		break;
	case STATE_PIPEUP:
		m_fXS = 0;
		m_fYS = -1.0f;
		break;
	case STATE_TRAMPOLINPRESS:
		//m_fXS = m_fXS / 2;
		m_fYS = 2.0f;
		break;
	case STATE_TRAMPOLINJUMP:
		m_fYS = -6.0f;
		break;
	case STATE_PUSHEDUP:
		m_fYS = -4.0f;
		break;
	case STATE_HELD:
	case STATE_HELDBYTONGUE:
	case STATE_CARRYBYHAND:
		ResetSpeed();
		break;
	case STATE_PLACEDBYHAND:
		if (pInfo && pInfo->pFromObject)
		{
			float fOffset = abs(pInfo->pFromObject->m_fX - m_fX);
			bool bLeft = (pInfo->pFromObject->m_fX > m_fX);
			m_fX = pInfo->pFromObject->m_fX;
			POINT ptOffset = FindCollidableOffset(
				bLeft ? COLLISION_LEFT : COLLISION_RIGHT, 
				fOffset);
			m_fX += ptOffset.x;
		}

		ResetSpeed();

		m_nKickCooltime = 24;
		m_nInvinsibleTime = 10;

		break;
	case STATE_KICKED:
		if (pInfo && pInfo->pFromObject)
		{
			// #TODO Change 10 to FromObject Size.cx/2
			auto s = GetSize();
			if (pInfo->pFromObject->m_fX > m_fX)
			{
				m_fX = pInfo->pFromObject->m_fX;
				POINT ptOffset = FindCollidableOffset(COLLISION_LEFT, s.cx / 2);
				m_fX += ptOffset.x;

				m_fXS = -3.0f;
				m_bFlip = true;
			}
			else
			{
				m_fX = pInfo->pFromObject->m_fX;
				POINT ptOffset = FindCollidableOffset(COLLISION_RIGHT, s.cx / 2);
				m_fX += ptOffset.x;

				m_fXS = 3.0f;
				m_bFlip = false;
			}
			m_fYS = -2.0f;

			m_pKickedBy = pInfo->pFromObject;
			if (m_pKickedBy->m_pController && 
				IsOpponent(m_pKickedBy) &&
				IS_ENEMY(this))
			{
				// Remove combo scoring. 2017.05.03
				/*
				int nScore = m_pKickedBy->CalcScoreFrom(400);
				m_pKickedBy->m_pController->AddScoreWithEffect(nScore, this);
				m_pKickedBy->AddCombo();
				*/

				m_nCombo = m_pKickedBy->m_nCombo;
			}
		}
		m_nKickCooltime = 24;
		m_nInvinsibleTime = 10;

		SOUND_MANAGER->PlaySoundEffect(L"Kicked");
		break;
	case STATE_POWED:
		{
			if (m_pGame->m_bModernStyle)
				m_bFlipV = false;
			else
				m_bFlipV = true;

			m_fYS = -3.0f;
			if (pInfo && pInfo->pFromObject)
			{
				if (pInfo->pFromObject->m_fXS != 0)
					m_fXS = pInfo->pFromObject->m_fXS / abs(pInfo->pFromObject->m_fXS);
				else
				{
					if (pInfo->pFromObject->m_fX < m_fX)
						m_fXS = 2.0f;
					else
						m_fXS = -2.0f;
				}

				if (pInfo->pFromObject->m_pController)
					pInfo->pFromObject->m_pController->AddScoreWithEffect(100, this);
			}

			SOUND_MANAGER->PlaySoundEffect(L"Kicked");
		}
		break;
	case STATE_KICKEDUP:
		if (pInfo && pInfo->pFromObject)
		{
			float fOffset = abs(pInfo->pFromObject->m_fX - m_fX);
			bool bLeft = (pInfo->pFromObject->m_fX > m_fX);
			m_fX = pInfo->pFromObject->m_fX;
			POINT ptOffset = FindCollidableOffset(
				bLeft ? COLLISION_LEFT : COLLISION_RIGHT,
				fOffset);
			m_fX += ptOffset.x;
		}

		m_fYS = -8.0f;

		if (pInfo && pInfo->pFromObject)
		{
			m_pKickedBy = pInfo->pFromObject;
		}

		SOUND_MANAGER->PlaySoundEffect(L"Kicked");
		break;
	case STATE_BLOCKJUMP:
		m_fYS = 0;
		{
			int nIdx = CUR_STAGE->GetTileIndex(
				m_ptCollision[COLLISION_BOTTOM].x / TILE_XS,
				m_ptCollision[COLLISION_BOTTOM].y / TILE_YS
			);
			std::map<int, int>::iterator it = CUR_STAGE->m_mapPressFrame.find(nIdx);
			if (it != CUR_STAGE->m_mapPressFrame.end())
			{
				if (it->second > 7)
					m_nStateFrame = it->second;
				else
				{
					m_nStateFrame = 7;
				}
			}
		}
		break;
	}
}

void GameObjectBase::SetState(int nState)
{
	m_nState = nState;
	m_nNextState = -1;
	m_nStateFrame = 0;
}

void GameObjectBase::OnIdle()
{
	if (!CheckFalling())
		ChangeState(STATE_STAND);
}

void GameObjectBase::OnStand()
{
	CheckFalling();
}

void GameObjectBase::OnSit()
{
	CheckFalling();
}

void GameObjectBase::OnWalk()
{
	if (CheckFalling())
		return;

	if (m_bReachFriend)
		OnReachFriend();
	else
		CheckReachWall();
}

void GameObjectBase::OnReachWall(bool bIsLeftWall)
{
	// When walk, obj reaches the wall.
	// Turn back or etc...

	SIZE s = GetSize();

	if (bIsLeftWall)
		m_fX = (m_ptCollision[COLLISION_LEFT].x + 1) + s.cx / 2 + 1;
	else
		m_fX = (m_ptCollision[COLLISION_RIGHT].x) - s.cx / 2;
}

void GameObjectBase::OnReachFriend()
{
	m_bReachFriend = false;
}

void GameObjectBase::OnBreak()
{
	CheckFalling();

	CheckReachWall();
}

void GameObjectBase::OnJump()
{
	if (m_bCollision[COLLISION_TOP])
	{
		// Hit the block
		SIZE size = GetSize();
		m_fYS = 0;
		m_fY = ((m_ptCollision[COLLISION_TOP].y + 1)) + size.cy;

		ChangeState(STATE_FALL);
		return;
	}

	OnFall();
}

void GameObjectBase::OnFall()
{
	if (m_fYS < 0)
	{
		if (m_bCollision[COLLISION_TOP])
		{
			// Hit the block
			SIZE size = GetSize();
			m_fYS = 0;
			m_fY = ((m_ptCollision[COLLISION_TOP].y + 1)) + size.cy;

			return;
		}
	}

	if (CheckReachWall())
		return;

	if (!m_bCollision[COLLISION_BOTTOM])
	{
		OnApplyGravity(1.0f, 4.0f);
	}
	else
	{
		if (m_bStandOnJumpBlock)
			ChangeState(STATE_BLOCKJUMP);
		else if ((int)m_fY - 4 <= m_ptCollision[COLLISION_BOTTOM].y)
		{
			m_fYS = 0;

			OnLanding();
		}
	}

	if (m_bReachFriend)
		OnReachFriend();
}

void GameObjectBase::OnApplyGravity(float fMagnification, float fMaxSpeed)
{
	float fGravity = CUR_STAGE->GetGravity();

	if (CanSwim())
	{
		fMagnification /= 4.0f;
		fMaxSpeed /= 2.0f;
	}

	m_fYS += (fGravity * fMagnification);

	if (m_fYS > fMaxSpeed && fMaxSpeed > 0)
		m_fYS = fMaxSpeed;
}

void GameObjectBase::OnSwim()
{
	// #TODO OnSwim
	OnFall();
}

void GameObjectBase::OnSwimJump()
{
	// #TODO OnSwimJump
	OnJump();
}

void GameObjectBase::OnPushedUp()
{
	int nSpeedFrame[] = { 
		-2, 0, -3, -4, -2, -3, -2, -1, -2, -1, -1, -1, -1, 
		0, 0, 0, 0, 1, 0, 1, 2, 1, 2,
	};

	int nSpeedFrameLen = sizeof(nSpeedFrame) / sizeof(int);
	if (m_nStateFrame < nSpeedFrameLen)
		m_fYS = nSpeedFrame[m_nStateFrame];
	else
		m_fYS += 1.0f;

	if (m_bCollision[COLLISION_TOP])
	{
		m_fYS = 0;
		ChangeState(STATE_FALL);
	}
	if (m_fYS > 0)
	{
		ChangeState(STATE_FALL);
	}

	CheckReachWall();
}

void GameObjectBase::OnPressJump()
{
	if (m_fYS < 0)
		OnJump();
	else
		ChangeState(STATE_FALL);
}

void GameObjectBase::OnGotOffVehicle()
{
	OnPushedUp();
}

void GameObjectBase::OnTrampolinPress()
{
	m_fX -= m_fXS;

	if (m_fYS < 0)
		ChangeState(STATE_FALL);
}

void GameObjectBase::OnTrampolinJump()
{
	if (CheckHitBlock(1))
		return;

	if (m_fYS >= 0)
		ChangeState(STATE_FALL);

	OnFall();
}

void GameObjectBase::OnItemPop()
{
	if (m_nStateFrame == 0)
	{
		SOUND_MANAGER->PlaySoundEffect(L"ItemPop");
	}

	if (m_nStateFrame == 30)
		m_fYS = -0.5f;

	if (m_nStateFrame == 62)
		OnAfterItemPop();
}

void GameObjectBase::OnItemPopDown()
{
	if (m_nStateFrame == 0)
	{
		SOUND_MANAGER->PlaySoundEffect(L"ItemPop");
	}

	if (m_nStateFrame == 4)
		m_fYS = 2.0f;

	if (m_nStateFrame == 4 + 4)
		OnAfterItemPop();
}

void GameObjectBase::OnAfterItemPop(int nNextStatePopUp, int nNextStatePopDown)
{
	if (m_nState == STATE_ITEMPOP)
		ChangeState(nNextStatePopUp);
	else if (m_nState == STATE_ITEMPOPDOWN)
		ChangeState(nNextStatePopDown);
}

void GameObjectBase::OnPipeUp()
{
	if (m_nStateFrame > 16)
		ChangeState(m_nBaseState);
}

void GameObjectBase::OnPipeDown()
{
}

void GameObjectBase::OnPipeLeft()
{
}

void GameObjectBase::OnPipeRight()
{
}

void GameObjectBase::OnThrown()
{
	if (m_nStateFrame < 8)
		return;

	if (!m_bCollision[COLLISION_BOTTOM])
	{
		OnApplyGravity(1, 4.0f);
	}
	else
	{
		if ((int)m_fY <= m_ptCollision[COLLISION_BOTTOM].y)
		{
			m_fYS = 0;
			
			OnLanding();
		}
	}

	CheckReachWall();

	if (IsOutOfStage())
		ChangeState(STATE_DEAD);
}

void GameObjectBase::OnBlockJump()
{
	if (m_nStateFrame == 0)
	{
		PressJumpBlock();
	}

	if (m_nStateFrame < GameDefaults::nBlockJumpFreeze)
	{
		POINT ptOffset = CUR_STAGE->GetHitAnimatedTileOffset(
			m_ptCollision[COLLISION_BOTTOM].x / TILE_XS,
			m_ptCollision[COLLISION_BOTTOM].y / TILE_YS
		);

		m_fY = m_ptCollision[COLLISION_BOTTOM].y + ptOffset.y;
	}

	if (m_nStateFrame == GameDefaults::nBlockJumpFreeze)
	{
		m_fYS = -6.0f;
	}

	if (m_nStateFrame > GameDefaults::nBlockJumpFreeze)
	{
		OnApplyGravity(1);

		if (m_bCollision[COLLISION_TOP])
		{
			SIZE size = GetSize();
			m_fYS = 0;
			m_fY = ((m_ptCollision[COLLISION_TOP].y + 1)) + size.cy;

			ChangeState(STATE_FALL);
		}

		if (m_fYS > 0)
			ChangeState(STATE_FALL);
	}

	if (m_nNextState != STATE_FALL)
		CheckReachWall();
}

void GameObjectBase::OnLanding()
{
	m_fY = m_ptCollision[COLLISION_BOTTOM].y;

	if (m_bStandOnJumpBlock)
		ChangeState(STATE_BLOCKJUMP);
	else if (m_fXS != 0)
		ChangeState(STATE_WALK);
	else
		ChangeState(STATE_STAND);
}

void GameObjectBase::OnStomped(bool bCauseDead, GameObjectBase *pWhoPressed)
{
	NaRect rc = GetRect();

	// StarDust x4
	if (bCauseDead)
	{
		CreateParameter param;
		param.fXS = 1.0f;
		param.fYS = 0.3f;
		CUR_STAGE->CreateEffect(m_fX, m_fY - rc.Height() / 2, EFFECT_STARDUST, &param);

		param.fXS = -1.0f;
		param.fYS = 0.3f;
		CUR_STAGE->CreateEffect(m_fX, m_fY - rc.Height() / 2, EFFECT_STARDUST, &param);

		param.fXS = 1.0f;
		param.fYS = -0.3f;
		CUR_STAGE->CreateEffect(m_fX, m_fY - rc.Height() / 2, EFFECT_STARDUST, &param);

		param.fXS = -1.0f;
		param.fYS = -0.3f;
		CUR_STAGE->CreateEffect(m_fX, m_fY - rc.Height() / 2, EFFECT_STARDUST, &param);
	}

	// Hit
	{
		CreateParameter param;
		if (pWhoPressed)
			CUR_STAGE->CreateEffect(pWhoPressed->m_fX, pWhoPressed->m_fY + 2, EFFECT_HIT, &param);
		else
			CUR_STAGE->CreateEffect(m_fX, rc.top, EFFECT_HIT, &param);
	}

	// Dust
	if (bCauseDead)
	{
		CreateParameter param;
		param.fScaleX = m_fScaleX;
		param.fScaleY = m_fScaleY;
		param.nType = EFFECT_DUST;
		CUR_STAGE->CreateEffect(m_fX, m_fY - rc.Height() / 2, EFFECT_DUST, &param);
	}
}

void GameObjectBase::OnLostWing()
{
	// Wing Debris Effect
	NaPointT<float> ptCamera = CAMERA->GetPosition();
	std::vector<POINT> vec = GetWingPosition();
	for (int i=0; i<vec.size(); i++)
	{
		if (m_bFlip)
			vec[i].x *= -1;

		float fScaleX, fScaleY;
		GetScale(fScaleX, fScaleY);

		CreateParameter param;
		param.fScaleX = fScaleX;
		param.fScaleY = fScaleY;
		param.pFromObject = this;
		param.nFlip = (i % 2 == 0) ? 0 : 1;
		CUR_STAGE->CreateEffect(
			m_fX + ((float)vec[i].x * fScaleX),
			m_fY + ((float)vec[i].y * fScaleY),
			EFFECT_WINGDEBRIS,
			&param);
	}
}

void GameObjectBase::OnKicked()
{
	if (CheckHitBlock(POWER_HITBLOCK_NORMAL))
		return;

	if (!m_bCollision[COLLISION_BOTTOM])
	{
		OnApplyGravity(1.0f, 4.0f);
	}
	else
	{
		if ((int)m_fY - 4 <= m_ptCollision[COLLISION_BOTTOM].y)
		{
			OnLanding();
		}
	}

	if (CheckReachWall())
	{
		m_fXS = 0;
		return;
	}
}

void GameObjectBase::OnPlacedByHand()
{
	if (!m_bCollision[COLLISION_BOTTOM])
	{
		OnApplyGravity(1.0f, 4.0f);
	}
	else
	{
		if ((int)m_fY - 4 <= m_ptCollision[COLLISION_BOTTOM].y)
		{
			m_fYS = 0;

			OnLanding();
		}
	}
}

void GameObjectBase::OnKickedUp()
{
	if (CheckHitBlock(POWER_HITBLOCK_NORMAL))
		return;

	if (m_bCollision[COLLISION_TOP])
	{
		// Hit the block
		SIZE size = GetSize();
		m_fYS = 0;
		m_fY = ((m_ptCollision[COLLISION_TOP].y + 1)) + size.cy;

		ChangeState(STATE_FALL);
		return;
	}

	if (CheckReachWall())
		return;

	if (!m_bCollision[COLLISION_BOTTOM])
	{
		OnApplyGravity(1.0f, 4.0f);
	}
	else
	{
		if ((int)m_fY - 4 <= m_ptCollision[COLLISION_BOTTOM].y)
		{
			m_fYS = 0;

			OnLanding();
		}
	}
}

void GameObjectBase::OnPowed()
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

void GameObjectBase::OnBounceTrampolinMiniHorz(GameObjectBase *pTrampolin)
{
	NaRect rc = pTrampolin->GetRect();
	bool bLeft = false;
	if (pTrampolin->m_fX < m_fX)
		bLeft = true;

	if (bLeft)
	{
		m_bCollision[COLLISION_LEFT] = true;
		m_ptCollision[COLLISION_LEFT].x = rc.right;
	}
	else
	{
		m_bCollision[COLLISION_RIGHT] = true;
		m_ptCollision[COLLISION_RIGHT].x = rc.left;
	}

	OnReachWall(bLeft);

	m_fXS = 3.0f;
	m_fYS = -4.0f;

	if (!bLeft)
	{
		m_fXS *= -1;
	}
}

void GameObjectBase::Carry(GameObjectBase * pObj)
{
}

void GameObjectBase::Kick(GameObjectBase * pObj, bool bIsCarried)
{
}

void GameObjectBase::ReleaseWing()
{
	if (m_bWinged)
	{
		m_bWinged = false;
		OnLostWing();
	}
}

bool GameObjectBase::IsAlive()
{
	switch (m_nState)
	{
	case STATE_PRESSED:
	case STATE_DIE:
	case STATE_DEAD:
	case STATE_BURNT:
	case STATE_POWED:
		return false;
	}

	switch (m_nNextState)
	{
	case STATE_PRESSED:
	case STATE_DIE:
	case STATE_DEAD:
	case STATE_BURNT:
		return false;
	}

	return true;
}

bool GameObjectBase::IsOutOfStage()
{
	if (m_fX < -100 || m_fX > CUR_STAGE->m_sizeTile.cx * TILE_XS + 100)
		return true;
	if (m_fY >  CUR_STAGE->m_sizeTile.cy * TILE_YS + 100)
		return true;

	return false;
}

bool GameObjectBase::IsOutOfCamera()
{
	if (CAMERA->IsOutOfCamera(this))
		return true;
	return false;
}

bool GameObjectBase::HasMushroom()
{
	return false;
}

bool GameObjectBase::IsInvinsible()
{
	return false;
}

bool GameObjectBase::IsOpponent(GameObjectBase *pObj)
{
	if (m_nDamageGroup == DAMAGEGROUP_NEUTRAL ||
		pObj->m_nDamageGroup == DAMAGEGROUP_NEUTRAL)
		return false;

	if (m_nDamageGroup != pObj->m_nDamageGroup)
		return true;

	return false;
}

bool GameObjectBase::CanLand()
{
	return false;
}

bool GameObjectBase::CanCollide(int nDir)
{
	return false;
}

bool GameObjectBase::CanKicked()
{
	return false;
}

bool GameObjectBase::CanPressed(int nPower)
{
	return true;
}

bool GameObjectBase::CanPushedUp()
{
	return true;
}

bool GameObjectBase::CanRided(GameObjectBase *pByWho)
{
	return false;
}

bool GameObjectBase::CanBurnt(int nType)
{
	if (m_nInvinsibleTime > 0)
		return false;
	if (m_nDamageCooltime > 0)
		return false;
	return true;
}

bool GameObjectBase::CanPowed()
{
	return CanBurnt(BURN_POWBLOCK);
}

bool GameObjectBase::CanCarry()
{
	return false;
}

bool GameObjectBase::CanCarried()
{
	return false;
}

bool GameObjectBase::CanEat()
{
	return false;
}

bool GameObjectBase::CanSpit()
{
	return false;
}

bool GameObjectBase::CanDamageFriend()
{
	return false;
}

bool GameObjectBase::CanDamageEnemy()
{
	switch (m_nState)
	{
	case STATE_HELDBYTONGUE:
	case STATE_ITEMPOP:
	case STATE_ITEMPOPDOWN:
		return false;
		break;
	}

	return true;
}

bool GameObjectBase::CanPressEnemy()
{
	return false;
}

bool GameObjectBase::CanSwim()
{
	NaRect rc = GetRect();
	if (CUR_STAGE->CanSwim(rc))
		return true;

	return false;
}

bool GameObjectBase::CanBounceTrampolinMiniHorz()
{
	return true;
}

bool GameObjectBase::CanBornFromBlock()
{
	return false;
}

bool GameObjectBase::CanStandOnSpike()
{
	return false;
}

void GameObjectBase::SetPosition(float fX, float fY)
{
	m_fX = fX;
	m_fY = fY;
}

void GameObjectBase::SetSpeed(float fXS, float fYS)
{
	m_fXS = fXS;
	m_fYS = fYS;
}

void GameObjectBase::ResetSpeed()
{
	m_fXS = 0;
	m_fYS = 0;
}

int GameObjectBase::GetPower(int nPowerForWhat)
{
	return 0;
}

int GameObjectBase::GetZOrder()
{
	switch (m_nState)
	{
	case STATE_PIPEDOWN:
	case STATE_PIPERIGHT:
	case STATE_PIPELEFT:
	case STATE_PIPEUP:
		return Z_IN_PIPE_OBJ;
		break;
	case STATE_CARRYBYHAND:
		if (m_pGame->m_pGameState->m_nType == GAMESTATE_ENTERPIPE ||
			m_pGame->m_pGameState->m_nType == GAMESTATE_EXITPIPE)
			return Z_IN_PIPE_OBJ - 1;

		return Z_VEHICLE_BEHIND_OBJ;
		break;
	case STATE_ITEMPOP:
	case STATE_ITEMPOPDOWN:
		return Z_BEHIND_MAP;
		break;
	}

	return 0;
}

void GameObjectBase::GetScale(float & fScaleX, float & fScaleY)
{
	fScaleX = m_fScaleX;
	fScaleY = m_fScaleY;
}

std::vector<POINT> GameObjectBase::GetWingPosition()
{
	POINT pt;
	SIZE s = GetSize();
	pt.x = - s.cx / 2;
	pt.y = - s.cy;

	std::vector<POINT> vec;
	vec.push_back(pt);

	pt.x = + s.cx / 2;
	vec.push_back(pt); 

	return vec;
}

int GameObjectBase::GetWingSpriteIndex()
{
	int nFrame = SPRIDX_WING1;
	if (m_fYS != 0)
	{
		if (m_nStateFrame % 16 >= 8)
			nFrame = SPRIDX_WING2;
	}

	return nFrame;
}

NaRect GameObjectBase::GetTileRect()
{
	NaRect rc = GetRect();
	rc.left = rc.left / TILE_XS;
	rc.top = rc.top / TILE_YS;
	rc.right = rc.right / TILE_XS;
	rc.bottom = rc.bottom / TILE_YS;

	return rc;
}

NaRect GameObjectBase::GetRect()
{
	NaRect rc;
	SIZE s = GetSize();
	rc.left = m_fX - s.cx / 2;
	rc.right = m_fX + s.cx / 2 - 1;
	rc.top = m_fY - s.cy + 1;
	rc.bottom = m_fY;

	return rc;
}

NaRectT<float> GameObjectBase::GetRectFloat()
{
	NaRectT<float> rc;
	SIZE s = GetSize();
	rc.left = m_fX - s.cx / 2;
	rc.right = m_fX + s.cx / 2 - 1;
	rc.top = m_fY - s.cy + 1;
	rc.bottom = m_fY;

	return rc;
}

std::vector<NaRect> GameObjectBase::GetRectList()
{
	std::vector<NaRect> vecRect;
	NaRect rc = GetRect();
	vecRect.push_back(rc);

	return vecRect;
}

SIZE GameObjectBase::GetSize()
{
	SIZE s;
	s.cx = 15 * m_fScaleX;
	s.cy = 15 * m_fScaleY;
	return s;
}

int  GameObjectBase::GetMaskedType()
{
	return m_nType;
}

SIZE GameObjectBase::GetSpriteSize()
{
	return SIZE();
}

PipeInfo * GameObjectBase::FindPipeNearMe(int nCollisionType)
{
	NaRect rcMe = GetRect();

	// #TODO move to stage? gameobject?
	PipeInfo *pPipeUnderMe = nullptr;
	std::vector<PipeInfo*>::iterator it = CUR_STAGE->m_vecPipeInfo.begin();
	for (; it != CUR_STAGE->m_vecPipeInfo.end(); ++it)
	{
		PipeInfo *pInfo = *it;
		if (pInfo->m_rcEnterance.IsOverlapped(rcMe))
		{
			if (nCollisionType == COLLISION_BOTTOM && pInfo->m_nType == PIPEENTRANCE_UPTODOWN)
				return pInfo;

			if (nCollisionType == COLLISION_RIGHT && pInfo->m_nType == PIPEENTRANCE_LEFTTORIGHT &&
				m_fY == pInfo->m_rcEnterance.bottom)
			{
				return pInfo;
			}

			if (nCollisionType == COLLISION_TOP && pInfo->m_nType == PIPEENTRANCE_DOWNTOUP)
				return pInfo;

			if (nCollisionType == COLLISION_LEFT && pInfo->m_nType == PIPEENTRANCE_RIGHTTOLEFT &&
				m_fY == pInfo->m_rcEnterance.bottom)
			{
				return pInfo;
			}
		}
	}

	return nullptr;
}

GameObjectBase * GameObjectBase::FindDoorNearMe()
{
	NaRect rcMe = GetRect();

	BEGIN_VECTOR_LOOP(GameObjectBase*, CUR_STAGE->m_vecEventObject) {
		if (pObj->m_nType != EVENTOBJECT_DOOR)
			continue;

		NaRect rc = pObj->GetRect();
		if (rc.ptInRect(m_fX, m_fY - 4))
			return pObj;
	} END_VECTOR_LOOP();

	return nullptr;
}

void GameObjectBase::AddCombo()
{
	switch (m_nState)
	{
	case STATE_STAND:
	case STATE_WALK:
		if (m_nInvinsibleTime == 0)
			return;
		break;
	}

	m_nCombo++;
}

void GameObjectBase::StopCombo()
{
	m_nCombo = 0;
}

int GameObjectBase::CalcScoreFrom(int nBeginScore)
{
	int nScores[] = { 100, 200, 400, 500, 1000, 2000, 4000, 8000, 10000 };
	int nLen = sizeof(nScores) / sizeof(nScores[0]);

	int nFound = -1;
	for (int i=0; i<nLen; i++)
	{
		if (nBeginScore == nScores[i])
		{
			nFound = i;
			break;
		}
	}

	if (nFound == -1)
		return nBeginScore;
	else if (nFound + m_nCombo >= nLen - 1)
		return 10000; // == 1UP
	else
		return nScores[nFound + m_nCombo];

	return nBeginScore;
}

const wchar_t* GameObjectBase::GetTypeName()
{
	int nType = m_nType;
	const wchar_t* str;

	if (nType >= EFFECT_BEGIN && nType <= EFFECT_END)
		str = g_szEffectTypes[nType - EFFECT_BEGIN];
	else if (nType >= ITEM_BEGIN && nType <= ITEM_END)
		str = g_szItemTypes[nType - ITEM_BEGIN];
	else if (nType >= ENEMY_BEGIN && nType <= ENEMY_END)
		str = g_szEnemyTypes[nType - ENEMY_BEGIN];
	else if (nType >= EVENTOBJECT_BEGIN && nType <= EVENTOBJECT_END)
		str = g_szEventObjectTypes[nType - EVENTOBJECT_BEGIN];
	else if (nType >= VEHICLE_BEGIN && nType <= VEHICLE_END)
		str = g_szVehicleObjectTypes[nType - VEHICLE_BEGIN];
	else if (nType >= BULLET_BEGIN && nType <= BULLET_END)
		str = g_szBulletTypes[nType - BULLET_BEGIN];
	else if (nType >= PLAYEROBJECT_BEGIN && nType <= PLAYEROBJECT_END)
		str = g_szPlayerObjectTypes[nType - PLAYEROBJECT_BEGIN];
	else
		str = L"Unknown";

	return str;
}

// Factory function
GameObjectBase * GameObjectBase::CreateInstance(Game * pGame, Stage *pStage, int nType)
{
	GameObjectBase *pObj = nullptr;
	
	if (nType >= EFFECT_BEGIN && nType <= EFFECT_END)
		pObj = EffectObjectBase::CreateInstance(pGame, pStage, nType);
	else if (nType >= ITEM_BEGIN && nType <= ITEM_END)
		pObj = ItemObjectBase::CreateInstance(pGame, pStage, nType);
	else if (nType >= ENEMY_BEGIN && nType <= ENEMY_END)
		pObj = EnemyObjectBase::CreateInstance(pGame, pStage, nType);
	else if (nType >= EVENTOBJECT_BEGIN && nType <= EVENTOBJECT_END)
		pObj = EventObjectBase::CreateInstance(pGame, pStage, nType);
	else if (nType >= VEHICLE_BEGIN && nType <= VEHICLE_END)
		pObj = VehicleObjectBase::CreateInstance(pGame, pStage, nType);
	else if (nType >= BULLET_BEGIN && nType <= BULLET_END)
		pObj = BulletObjectBase::CreateInstance(pGame, pStage, nType);
	else
	{
		// ETC
		switch (nType)
		{
			// PlayerObject
		case PLAYEROBJECT_MARIO:			pObj = new Mario(pGame, pStage);				break;
		case PLAYEROBJECT_NEWMARIO:			pObj = new NewMario(pGame, pStage);				break;
		}
	}

	if (pObj)
		pObj->BuildPropertyMap();
	else
	{
		NaDebugOut(L"Error: Cannot Create GameObject / Type: %d\n", nType);
	}

	return pObj;
}

void GameObjectBase::LoadProperty(Json::Value & jObj)
{
	JsonManager::LoadJsonObjectProperty(this, jObj);

	if (m_bPowerUp)
		OnPowerUp();
}

NaString GameObjectBase::GetStateName(int nState)
{
	if (nState == -1)
		nState = m_nState;

	NaString str = L"";
	switch (nState)
	{
	case STATE_UNDEFINED:		str = L"Undefined";			break;
	case STATE_IDLE:			str = L"Idle";				break;
	case STATE_REBORN:			str = L"Reborn";			break;
	case STATE_STAND:			str = L"Stand";				break;
	case STATE_SIT:				str = L"Sit";				break;
	case STATE_WALK:			str = L"Walk";				break;
	case STATE_BREAK:			str = L"Break";				break;
	case STATE_JUMP:			str = L"Jump";				break;
	case STATE_SITJUMP:			str = L"SitJump";			break;
	case STATE_FALL:			str = L"Fall";				break;
	case STATE_SITFALL:			str = L"SitFall";			break;

	case STATE_SWIM:			str = L"Swim";				break;
	case STATE_SWIMJUMP:		str = L"SwimJump";			break;
	case STATE_SPINJUMP:		str = L"SpinJump";			break;
	case STATE_GROUNDPOUND:		str = L"GroundPound";		break;

	case STATE_CLIMBIDLE:		str = L"ClimbIdle";			break;
	case STATE_CLIMBMOVE:		str = L"ClimbMove";			break;

	case STATE_ITEMPOP:			str = L"ItemPop";			break;
	case STATE_ITEMPOPDOWN:		str = L"ItemPopDown";		break;
	case STATE_THROWFIRE:		str = L"ThrowFire";			break;

	case STATE_PIPEDOWN:		str = L"PipeDown";			break;
	case STATE_PIPERIGHT:		str = L"PipeRight";			break;
	case STATE_PIPEUP:			str = L"PipeUp";			break;
	case STATE_PIPELEFT:		str = L"PipeLeft";			break;

	case STATE_VINEUP:			str = L"VineUp";			break;

	case STATE_PUSHEDUP:		str = L"PushedUp";			break;
	case STATE_GOTOFFVEHICLE:	str = L"GotOffVehicle";		break;
	case STATE_TRAMPOLINPRESS:	str = L"TrampolinPress";	break;
	case STATE_TRAMPOLINJUMP:	str = L"TrampolinJump";		break;

	case STATE_RIDE:			str = L"Ride";				break;

	case STATE_SHELLIDLE:		str = L"ShellIdle";			break;
	case STATE_SHELLRUN:		str = L"ShellRun";			break;
	case STATE_SHELLAWAKE:		str = L"ShellAwake";		break;

	case STATE_DIE:				str = L"Die";				break;
	case STATE_KICKED:			str = L"Kicked";			break;
	case STATE_PRESSED:			str = L"Pressed";			break;
	case STATE_PRESSJUMP:		str = L"PressJump";			break;
	case STATE_BURNT:			str = L"Burnt";				break;

	case STATE_SPRING:			str = L"Spring";			break;

	case STATE_THROWN:			str = L"Thrown";			break;
	case STATE_THROWNFLY:		str = L"ThrownFly";			break;

	case STATE_FIREDOWN:		str = L"FireDown";			break;
	case STATE_FIREUP:			str = L"FireUp";			break;

	case STATE_MOVE:			str = L"Move";				break;
	case STATE_MOVEUP:			str = L"MoveUp";			break;
	case STATE_MOVEDOWN:		str = L"MoveDown";			break;

	case STATE_FLY:				str = L"Fly";				break;
	case STATE_RETREAT:			str = L"Retreat";			break;

	case STATE_FINDBALANCE:		str = L"FindBalance";		break;

	case STATE_FLAGDOWN:		str = L"FlagDown";			break;
	case STATE_FLAGSTOP:		str = L"FlagStop";			break;
	case STATE_FLAGFLIP:		str = L"FlagFlip";			break;
	case STATE_FLAGUP:			str = L"FlagUp";			break;

	case STATE_VINEGROW:		str = L"VineGrow";			break;

	case STATE_HATCH:			str = L"Hatch";				break;

	case STATE_HELDBYTONGUE:	str = L"HeldByTongue";		break;
	case STATE_HELD:			str = L"Held";				break;
	case STATE_SPITTED:			str = L"Spitted";			break;
	case STATE_SPITTED_SAFE:	str = L"SpittedSafe";		break;

	case STATE_STACKED:			str = L"Stacked";			break;
	case STATE_BLOCKJUMP:		str = L"BlockJump";			break;

	case STATE_CARRYBYHAND:		str = L"CarryByHand";		break;
	case STATE_PLACEDBYHAND:	str = L"PlacedByHand";		break;
	case STATE_KICKEDUP:		str = L"KickedUp";			break;
	case STATE_EXPLODE:			str = L"Explode";			break;

	case STATE_TEST:			str = L"Test";				break;

	case STATE_EVENTDEAD:		str = L"EventDead";			break;
	case STATE_DEAD:			str = L"Dead";				break;
	default:
		break;
	}
	return str;
}
