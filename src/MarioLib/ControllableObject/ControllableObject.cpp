#include "ControllableObject.h"

#include "Game.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "Camera.h"
#include "Stage.h"
#include "PipeInfo.h"
#include "Fireball.h"
#include "SoundManager.h"

#include "RenderManager.h"
#include "InputManager.h"
#include "Player.h"
#include "FModManager.h"
#include "EventObjectBase.h"
#include "VehicleObjectBase.h"
#include "Door.h"
#include "ThemeSetting.h"

#include "UIBase.h"

ControllableObject::ControllableObject(Game *pGame, Stage *pStage) :
	GameObjectBase(pGame, pStage)
{
	m_nType = -1;

	m_nWalkFrame = 0;
	m_nJumpHold = 0;
	m_pClimb = nullptr;
	
	m_nThrowFireFrame = 0;
	m_nKickFrame = 0;

	m_nSpriteIndex = 0;
	m_nCostume = 0;

	RequestSprites(pStage);
}

ControllableObject::~ControllableObject()
{

}

void ControllableObject::CheckCollision()
{
	if (m_nShape != SHAPE_GIANT && m_nNextShape != SHAPE_GIANT)
		GameObjectBase::CheckCollision();
	else
	{
		CheckCollisionGiant();
	}
}

void ControllableObject::CheckCollisionGiant()
{
	m_fX += m_fXS;
	m_fY += m_fYS;

	bool bBlockCrushed = false;
	int nPower = POWER_HITBLOCK_GIANTMARIO;
	if (m_nNextShape == SHAPE_GIANT)
		nPower = POWER_HITBLOCK_GROWING_GIANTMARIO;

	memset(&m_bCollision, 0, sizeof(m_bCollision));
	m_bStandOnJumpBlock = false;

	NaRect rc = GetRect();
	NaRect rcTile = GetTileRect();
	int x, y;

	// For Debug HitBox
	/*
#define SHOW_STAGETILEBOX	{ NaRect rcTemp = rcStageTile; rcTemp.Offset(-CAMERA->m_ptPos.x, -CAMERA->m_ptPos.y); u.RenderBox(rcTemp, 0x900000ff); }
	UIBase u(m_pGame);
	*/

#define SHOW_STAGETILEBOX	{}

	// Check Top
	for (int i = rcTile.left; i <= rcTile.right; i++)
	{
		x = i;
		y = rcTile.top;
		if ((CUR_STAGE->IsHardTile(x, y) || (m_fYS < 0 && CUR_STAGE->HasItem(x, y))) &&
			!CUR_STAGE->IsHardTile(x, y + 1))
		{
			NaRect rcStageTile = CUR_STAGE->GetTileRect(x, y);
			SHOW_STAGETILEBOX;
			if (CUR_STAGE->CanCrush(x, y, nPower))
			{
				CUR_STAGE->CrushBlock(x, y, this);
				bBlockCrushed = true;
			}
			else if (m_bCollision[COLLISION_TOP])
			{
				// Store best near tile
				if (abs((m_ptCollision[COLLISION_TOP].x) + (TILE_XS / 2) - m_fX) >
					abs((x * TILE_XS) + (TILE_XS / 2) - m_fX))
					m_ptCollision[COLLISION_TOP] = { (float)(x * TILE_XS), (float)(y * TILE_YS + TILE_YS - 1) };
			}
			else if (rc.IsOverlapped(rcStageTile))
			{
				m_bCollision[COLLISION_TOP] = true;
				m_ptCollision[COLLISION_TOP] = { (float)(x * TILE_XS), (float)(y * TILE_YS + TILE_YS - 1) };
			}
		}
	}

	// Check Left,Right
	for (int i = rcTile.top; i <= rcTile.bottom; i++)
	{
		x = rcTile.left;
		y = i;

		// ignore near underside
		if (y * 16 - m_fY > -4)
			continue;

		NaRect rcStageTile = CUR_STAGE->GetTileRect(x, y);
		SHOW_STAGETILEBOX;
		if (CUR_STAGE->IsHardTile(x, y) && !CUR_STAGE->IsHardTile(x + 1, y))
		{
			if (CUR_STAGE->CanCrush(x, y, nPower))
			{
				CUR_STAGE->CrushBlock(x, y, this);
				bBlockCrushed = true;
			}
			else if (rc.IsOverlapped(rcStageTile))
			{
				m_bCollision[COLLISION_LEFT] = true;
				m_ptCollision[COLLISION_LEFT] = { (float)rcStageTile.right - 1, (float)(y * TILE_YS) };
			}
		}

		x = rcTile.right;
		rcStageTile = CUR_STAGE->GetTileRect(x, y);
		SHOW_STAGETILEBOX;
		if (CUR_STAGE->IsHardTile(x, y) && !CUR_STAGE->IsHardTile(x - 1, y))
		{
			if (CUR_STAGE->CanCrush(x, y, nPower))
			{
				CUR_STAGE->CrushBlock(x, y, this);
				bBlockCrushed = true;
			}
			else if (rc.IsOverlapped(rcStageTile))
			{
				m_bCollision[COLLISION_RIGHT] = true;
				m_ptCollision[COLLISION_RIGHT] = { (float)rcStageTile.left, (float)(y * TILE_YS) };
			}
		}
	}

	// Check Inside of body
	for (int i = rcTile.top + 1; i <= rcTile.bottom - 1; i++)
	{
		for (int j = rcTile.left + 1; j <= rcTile.right - 1; j++)
		{
			x = j;
			y = i;

			NaRect rcStageTile = CUR_STAGE->GetTileRect(x, y);
			SHOW_STAGETILEBOX;
			if (CUR_STAGE->IsHardTile(x, y))
			{
				if (CUR_STAGE->CanCrush(x, y, nPower))
				{
					CUR_STAGE->CrushBlock(x, y, this);
					bBlockCrushed = true;
				}
				else
				{
					// Stuck!?
					m_bCollision[COLLISION_BOTTOM] = true;
					m_ptCollision[COLLISION_BOTTOM] = { (float)(x * TILE_XS), (float)(y * TILE_YS) };

					//m_fXS = 1.0f;
					//m_fYS = 0;
				}
			}
		}
	}

	// Check Bottom
	for (int i = rcTile.left; i <= rcTile.right; i++)
	{
		x = i;
		y = rcTile.bottom;

		if (m_fYS >= 0 && CUR_STAGE->IsJumpTile(x, y) && !CUR_STAGE->IsHardTile(x, y - 1, COLLISION_BOTTOM))
		{
			NaRect rcStageTile = CUR_STAGE->GetTileRect(x, y, true);
			if (m_bCollision[COLLISION_BOTTOM])
			{
				if (abs((m_ptCollision[COLLISION_BOTTOM].x) + (TILE_XS / 2) - m_fX) >
					abs((x * TILE_XS) + (TILE_XS / 2) - m_fX))
					m_ptCollision[COLLISION_BOTTOM] = { (float)(x * TILE_XS), (float)(y * TILE_YS) };
			}
			else if (rc.IsOverlapped(rcStageTile))
			{
				m_bCollision[COLLISION_BOTTOM] = true;
				m_ptCollision[COLLISION_BOTTOM] = { (float)(x * TILE_XS), (float)(y * TILE_YS) };

				m_bStandOnJumpBlock = true;
			}
		}
		else if(CUR_STAGE->IsHardTile(x, y, COLLISION_BOTTOM) && !CUR_STAGE->IsHardTile(x, y - 1))
		{
			NaRect rcStageTile = CUR_STAGE->GetTileRect(x, y);
			SHOW_STAGETILEBOX;
			if (rc.IsOverlapped(rcStageTile))
			{
 				if (m_fYS >= 0 && CUR_STAGE->CanPress(x, y))
 				{
 					CUR_STAGE->PressBlock(x, y, nPower, this);
					bBlockCrushed = true;
 				}
 				else
				{
					rcStageTile = CUR_STAGE->GetTileRect(x, y);
					m_bCollision[COLLISION_BOTTOM] = true;
					m_ptCollision[COLLISION_BOTTOM] = { (float)(x * TILE_XS), (float)(y * TILE_YS) };

					m_bStandOnJumpBlock = false;
					break;
				}	
			}
		}
	}

	{
		auto rcF = GetRectFloat();
		std::vector<GameObjectBase*> vecCollidable;
		PASTE_VECTOR(vecCollidable, CUR_STAGE->m_vecEventObject);
		PASTE_VECTOR(vecCollidable, CUR_STAGE->m_vecEnemy);

		BEGIN_VECTOR_LOOP(GameObjectBase*, vecCollidable) {
			if (pObj == this)
				continue;

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
				if (abs(rcF.bottom - rcObj.top) < (2 + abs(m_fYS)) && m_fYS >= 0)
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
				if (abs(rcF.left - rcObj.right) < (2 + abs(m_fXS)) && rcF.bottom - 4 >= rcObj.top)
				{
					if (pObj->CanCollide(COLLISION_RIGHT))
					{
						m_bCollision[COLLISION_LEFT] = true;
						m_ptCollision[COLLISION_LEFT] = { rcObj.right, m_fY };
					}
				}
				// obj is on my right
				if (abs(rcF.right - rcObj.left) < (2 + abs(m_fXS)) && rcF.bottom - 4 >= rcObj.top)
				{
					if (pObj->CanCollide(COLLISION_LEFT))
					{
						m_bCollision[COLLISION_RIGHT] = true;
						m_ptCollision[COLLISION_RIGHT] = { rcObj.left, m_fY };
					}
				}
			}
		} END_VECTOR_LOOP();
	}

	if (bBlockCrushed)
	{
		SOUND_MANAGER->PlaySoundEffect(L"BreakBlock");
	}

	m_fX -= m_fXS;
	m_fY -= m_fYS;
}

bool ControllableObject::CheckJump()
{
	if (IsInputPressed(INPUT_J) && m_bCollision[COLLISION_BOTTOM])
	{
		if (m_nShape != SHAPE_COSTUME &&
			m_pCarry == nullptr &&
			m_pGame->IsActiveAbility(ABILITY_SPINJUMP) && IsInputHold(INPUT_U))
			ChangeState(STATE_SPINJUMP);
		else
			ChangeState(STATE_JUMP);
		return true;
	}

	return false;
}

void ControllableObject::Process()
{
	GameObjectBase::Process();
}

void ControllableObject::ProcessBase()
{
	switch (m_nState)
	{
	case STATE_SITJUMP:		OnJump(); break;
	case STATE_SITFALL:		OnFall(); break;
	case STATE_PRESSJUMP:	OnPressJump(); break;
	case STATE_FLAGDOWN:	OnFlagDown(); break;
	case STATE_FLAGFLIP:	OnFlagFlip(); break;
	case STATE_CLIMBIDLE:	OnClimbIdle(); break;
	case STATE_CLIMBMOVE:	OnClimbMove(); break;
	case STATE_PIPEDOWN:
	case STATE_PIPERIGHT:
	case STATE_PIPELEFT:
	case STATE_PIPEUP:
	case STATE_DIE:
		m_vecOverlappedEnemy.clear();
		GameObjectBase::ProcessBase();
		break;
	default:
		GameObjectBase::ProcessBase();
		break;
	}

	if (CanFire() && IsInputPressed(INPUT_F))
	{
		ThrowFireball();
	}

	if (IsAlive())
	{
		bool bInvinsible = (m_nInvinsibleTime > 0);
		bool bImmune = (m_nImmuneTime > 0);
		if (bInvinsible || bImmune)
			OnInvinsible(bInvinsible);

		if (m_pCarry)
		{
			if (!IsInputHold(INPUT_F))
			{
				Kick(m_pCarry, true);
			}
			else if (!m_pCarry->IsAlive())
			{
				CUR_STAGE->m_vecReservedGameObject.push_back(m_pCarry);
				m_pCarry = nullptr;
			}
		}

		if (m_vecOverlappedEnemy.size() > 0 && !bInvinsible)
		{
			bool bDamaged = false;
			std::vector<GameObjectBase*>::iterator it = m_vecOverlappedEnemy.begin();
			for (; it != m_vecOverlappedEnemy.end(); ++it)
			{
				GameObjectBase *pEnemy = *it;
				if (m_nShape == SHAPE_GIANT)
				{
					// Invinsible enemy = just pass
					if (!pEnemy->IsInvinsible())
					{
						ChangeStateInfo info;
						info.pFromObject = this;
						pEnemy->ChangeState(STATE_BURNT, &info);
					}
				}
				else if (m_nShape == SHAPE_SHELL && m_nState == STATE_SHELLRUN)
				{
					if (!pEnemy->IsInvinsible())
					{
						bool bAlive = true;
						if (pEnemy->CanBurnt(BURN_TURTLESHELL))
						{
							// #TODO Score
							ChangeStateInfo info;
							info.pFromObject = this;
							pEnemy->ChangeState(STATE_BURNT, &info);

							if (!pEnemy->IsAlive())
								bAlive = false;
						}

						if (bAlive)
						{
							if (pEnemy->m_fX < m_fX)
							{
								m_ptCollision[COLLISION_LEFT].x = m_fX;
								OnReachWall(true);
							}
							else
							{
								m_ptCollision[COLLISION_RIGHT].x = m_fX;
								OnReachWall(false);
							}
						}
					}
				}
				else if (pEnemy->CanDamageEnemy() && m_nDamageCooltime == 0)
				{
					if (!bImmune)
					{
						bDamaged = true;
						NaDebugOut(L"%ls damaged by %ls(%ls)!!\n",
							GetTypeName(), pEnemy->GetTypeName(), pEnemy->GetStateName());
					}
				}
				else if (CanCarry() && pEnemy->CanCarried())
				{
					Carry(pEnemy);
				}
				else if (pEnemy->CanKicked())
				{
					Kick(pEnemy, false);
				}
			}

			if (bDamaged && m_nDamageCooltime <= 0)
				OnDamaged();
		}

		if (m_bDamagedByMap && m_nShape != SHAPE_GIANT && m_nInvinsibleTime == 0)
		{
			OnDamaged();
		}
	}

	if (CUR_PLAYER->m_pControlObj == this && IsAlive() && IsOutOfStage())
	{
		if (CUR_STAGE->m_nEntranceType == STAGEENTRANCE_GROUNDTOSKY &&
			CUR_STAGE->m_pParentStage != nullptr)
		{
			m_pGame->PopStage();

			m_pGame->m_pGameState->MovePlayerObjectToSkyExit();
			m_pGame->m_pGameState->ClearEnemyInViewport();
		}
		else
		{
			if (m_nShape == SHAPE_GIANT &&
				m_pGame->m_nSpecialEvents & SPECIALEVENT_GIANTPREVENTHOLE)
			{
				ChangeState(STATE_TRAMPOLINJUMP);
				m_fYS = -11.0f;
			}
			else
			{
				ChangeState(STATE_DIE);
				m_pGame->ChangeState(GAMESTATE_DIE);
			}
		}
	}

	if (CanSwim())
	{
		if (m_pGame->m_nGameFrame % 70 == 0)
		{
			SIZE s = GetSize();
			CUR_STAGE->CreateEffect(m_fX, m_fY - s.cy + 8, EFFECT_AIRBUBBLE, 0);
		}
	}
}

void ControllableObject::ProcessShapeChange(int nStateFrame)
{
	// From
	switch(m_nShape)
	{
	case SHAPE_SHELL:
		if (m_nState == STATE_SHELLRUN || m_nState == STATE_SHELLIDLE)
			m_nState = STATE_SIT;
		break;
	case SHAPE_GIANT:
		m_fScaleX -= 0.05f;
		m_fScaleY -= 0.05f;
		if (m_fScaleX <= 1.0f || m_fScaleY <= 1.0f)
		{
			m_fScaleX = 1.0f;
			m_fScaleY = 1.0f;
		}
		break;
	}

	// To
	if (m_nNextShape == SHAPE_GIANT)
	{
		if (m_pCarry)
		{
			Kick(m_pCarry, true);
		}

		if (nStateFrame < 40)
		{
			m_fScaleX += 0.08f;
			m_fScaleY += 0.08f;
		}
		else if (nStateFrame < 50)
		{
			m_fScaleX += 0.04f;
			m_fScaleY += 0.04f;
		}
		else
		{
			m_fScaleX += 0.02f;
			m_fScaleY += 0.02f;
		}

		if (nStateFrame == 45 || nStateFrame == 55)
		{
			CUR_STAGE->MakeEarthquake(POWER_EARTHQUAKE_GIANTMARIOJUMP);
		}

		if (nStateFrame == 40)
		{
			SOUND_MANAGER->PlayGiantBGM();
		}
	}

	if (nStateFrame == 60 && m_nNextShape != -1)
	{
		OnChangeShape(m_nNextShape);
	}
}

void ControllableObject::OnBeforeProcess()
{
	GameObjectBase::OnBeforeProcess();

	CheckCollisionVehicle();
}

void ControllableObject::OnAfterProcess()
{
	if (m_nInvinsibleTime > 0)
	{
		if (m_nInvinsibleTime == 8 * 24)
		{
			SOUND_MANAGER->PlayStageBGM();
		}

		m_nInvinsibleTime--;
	}

	if (m_nGiantTime > 0)
	{
		m_nGiantTime--;
		if (m_nGiantTime == 0)
		{
			SOUND_MANAGER->PlayStageBGM();

			ChangeShape(SHAPE_BIG);
			m_pGame->ChangeState(GAMESTATE_SHAPECHANGE);
		}
	}

	GameObjectBase::OnAfterProcess();

	MoveCarryObject();
}

void ControllableObject::ChangeState(int nState, ChangeStateInfo *pInfo)
{
	/*
	NaDebugOut(L"%s: ChangeState: %s -> %s\n",
		GetTypeName(), GetStateName(m_nState).wstr(), GetStateName(nState).wstr());
	*/

	switch (nState)
	{
	case STATE_BURNT:
	case STATE_PRESSED:
		OnDamaged();
		return;
		break;
	case STATE_TRAMPOLINPRESS:
	case STATE_TRAMPOLINJUMP:
		if (m_pVehicle)
			return;
		break;
	}

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

void ControllableObject::OnChangeState(int nState)
{
	ChangeStateInfo *pInfo = nullptr;
	if (m_ChangeStateInfo.nNextState != -1)
		pInfo = &m_ChangeStateInfo;

	bool bSwim = CanSwim();
	bool bSound = true;
	int nJumpBonus = 0;

	// Force Change State
	switch (nState)
	{
	case STATE_JUMP:
		if (m_nState == STATE_SIT)
			nState = STATE_SITJUMP;
		if (bSwim)
			nState = STATE_SWIMJUMP;
		break;
	case STATE_SITJUMP:
		if (bSwim)
			nState = STATE_SWIMJUMP;
		break;
	case STATE_PRESSJUMP:
		if (m_pGame->m_nStagePackType != STAGEPACK_CLASSIC)
		{
			if (IsInputHold(INPUT_J))
			{
				nJumpBonus = 1;
				nState = STATE_JUMP;
				bSound = false;
			}
		}
		break;
	case STATE_FALL:
		if (m_nState == STATE_SITJUMP || m_nState == STATE_SIT)
			nState = STATE_SITFALL;
		break;
	}

	// State beginning Condition
	switch (nState)
	{
	case STATE_REBORN:
		m_fXS = 0;
		m_fYS = 0;
		m_bFlip = false;
		m_nSpriteIndex = 2;
		OnChangeShape(SHAPE_MINI);

		// Trick
		m_nState = STATE_STAND;
		return;
		break;
	case STATE_STAND:
		m_fXS = 0;
		m_fYS = 0;
		m_nJumpHold = 0;
		break;
	case STATE_WALK:
		if (m_pClimb)
			m_pClimb = nullptr;
		m_nWalkFrame = 0;
		m_nJumpHold = 0;
		break;
	case STATE_BREAK:
		SOUND_MANAGER->PlaySoundEffect(L"RunBreak");
		break;
	case STATE_JUMP:
	case STATE_SITJUMP:
		if (m_pClimb)
			m_pClimb = nullptr;

		if (pInfo && pInfo->nPower != 0)
			m_fYS = -pInfo->nPower;
		else
			m_fYS = m_PhysicalProps.fJumpSpeed; // -4.0f;

		// #TODO!
		m_nJumpHold = m_PhysicalProps.nJumpPower;

		// If doing Dash, ControllableObject can jump more higher
		if (abs(m_fXS) > 1.0f)
			m_nJumpHold += 1;
		if (abs(m_fXS) > 1.5f)
			m_nJumpHold += 1;

		if (m_nJumpHold > m_PhysicalProps.nJumpPower || nJumpBonus > 0)
			m_fYS = -4.0f - (nJumpBonus * 0.25f) - ((m_nJumpHold - m_PhysicalProps.nJumpPower) * 0.25f);

		//NaDebugOut(L"%ls's JumpHold=%d, fYS=%.1f\n", GetTypeName(), m_nJumpHold, m_fYS);

		if (bSound)
		{
			if (m_nShape == SHAPE_MINI || m_nShape == SHAPE_COSTUME)
			{
				SOUND_MANAGER->PlaySoundEffect(L"JumpSmall", this);
			}
			else
			{
				SOUND_MANAGER->PlaySoundEffect(L"JumpBig");
			}
		}
		break;
	case STATE_SPINJUMP:
		if (m_pClimb)
			m_pClimb = nullptr;

		if (pInfo && pInfo->nPower != 0)
			m_fYS = -pInfo->nPower;
		else
			m_fYS = -4.0f;

		// #TODO!
		m_nJumpHold = m_PhysicalProps.nJumpPower;

		if (nJumpBonus > 0)
			m_fYS = -4.0f - (nJumpBonus * 0.25f) - ((m_nJumpHold - m_PhysicalProps.nJumpPower) * 0.25f);

		if (bSound)
		{
			SOUND_MANAGER->PlaySoundEffect(L"JumpSpin");
		}
		break;
	case STATE_FALL:
		m_nJumpHold = 0;
		break;
	case STATE_SWIMJUMP:
		{
			m_fYS = -2.0f;
		
			if (IS_MODERN)
			{
				SIZE s = GetSize();
				for (int i = 0; i < (1 + (rand() % 3)); i++)
				{
					int nOffsetX = 5 + (rand() % 5);
					if (m_bFlip)
						nOffsetX *= -1;

					CreateParameter param;
					param.nDuration = 16 + (rand() % 5);
					CUR_STAGE->CreateEffect(
						m_fX + nOffsetX,
						m_fY - s.cy + 3 + (rand() % 5),
						EFFECT_AIRBUBBLE, &param);
				}
			}

			SOUND_MANAGER->PlaySoundEffect(L"Pressed");
		}
		break;
	case STATE_TRAMPOLINPRESS:
		m_fXS = 0;
		m_fYS = 2.0f;
		break;
	case STATE_TRAMPOLINJUMP:
		m_fYS = -6.5f;
		m_nJumpHold = 0;
		if (IsInputHold(INPUT_J))
			m_fYS = -10.0f;
		break;
	case STATE_PARALYZE:
		m_fXS = 0;
		m_fYS = 0;
		{
			CreateParameter param;
			param.pFromObject = this;
			SIZE s = GetSize();
			CUR_STAGE->CreateEffect(m_fX - ((s.cx / 2) + 6), m_fY - (s.cy / 2), EFFECT_PARALYZE, &param);
			CUR_STAGE->CreateEffect(m_fX + ((s.cx / 2) + 6), m_fY - (s.cy / 2), EFFECT_PARALYZE, &param);
		}
		break;
	case STATE_DIE:
		if (m_pVehicle)
			m_pVehicle->OnGetOffPassenger();
		if (m_pCarry)
		{
			CUR_STAGE->m_vecReservedGameObject.push_back(m_pCarry);
			m_pCarry = nullptr;
		}
		ResetSpeed();
		StopCombo();
		
		//
		// #TODO A temporary measure. You need to fix it in the right place.
		//       Or at the moment of death, only the link with the key is removed.
		//
		if (m_pController)
			m_pController->ClearDoorKey();

		m_nInvinsibleTime = 0;
		m_nDamageCooltime = 0;
		m_nImmuneTime = 0;
		m_nJumpHold = 0;

		if (IS_ONLINESTAGE)
		{
			CreateParameter param;
			param.pFromObject = this;
			SIZE s = GetSize();
			CUR_STAGE->CreateEffect(m_fX, m_fY - (s.cy / 2), EFFECT_DEATHPOSITION, &param);
		}

		OnChangeShape(SHAPE_MINI);
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
	case STATE_VINEUP:
		m_fXS = 0;
		m_fYS = -1.0f;
		break;
	case STATE_FLAGDOWN:
		if (pInfo && pInfo->pFromObject)
		{
			m_pClimb = pInfo->pFromObject;
			
			if (m_fX < m_pClimb->m_fX)
				m_bFlip = false;
			else
				m_bFlip = true;
			
			if (m_bFlip)
				m_fX = m_pClimb->m_fX + 6;
			else
				m_fX = m_pClimb->m_fX - 6;
		}
		m_fXS = 0;
		m_fYS = 0;
		break;
	case STATE_FLAGFLIP:
		m_fXS = 0.5f;
		break;
	case STATE_CLIMBIDLE:
		if (pInfo && pInfo->pFromObject)
		{
			m_pClimb = pInfo->pFromObject;
		}
		m_fXS = 0;
		m_fYS = 0;

		if (m_pVehicle)
		{
			if (m_pVehicle->m_fX < m_pClimb->m_fX)
				m_pVehicle->m_fXS = -1.0f;
			if (m_pVehicle->m_fX > m_pClimb->m_fX)
				m_pVehicle->m_fXS = 1.0f;
			m_pVehicle->OnGetOffPassenger();
		}

		if (m_fX < m_pClimb->m_fX)
			m_bFlip = false;
		else
			m_bFlip = true;
		break;	
	case STATE_RIDE:
		m_fXS = 0;
		m_fYS = 0;

		if (m_pClimb)
			m_pClimb = nullptr;
		if (m_pCarry)
		{
			Kick(m_pCarry, true);
		}
		break;
	default:
		GameObjectBase::OnChangeState(nState);
		break;
	}

	m_nState = nState;
}

// Perform from->to action
void ControllableObject::ChangeShape(int nShape)
{
	// From
	switch (m_nShape)
	{
	case SHAPE_FIRE:
		if (nShape == SHAPE_BIG || nShape == SHAPE_MINI)
			m_nDamageCooltime = GameDefaults::nDamageCooltime; 
		break;
	case SHAPE_COSTUME:
		if (nShape != SHAPE_COSTUME)
		{
			CreateParameter param;
			param.fXS = 2.0f;
			param.fYS = -2.0f;
			param.fZS = -14.0f;
			param.pSprite = GetCostumeSprite();
			param.nType = 0;
			CUR_STAGE->ReserveCreateGameObject(m_fX, m_fY, EFFECT_DEBRISBASE, &param);
		}
		break;
	case SHAPE_GIANT:
		m_nDamageCooltime = GameDefaults::nDamageCooltime;

		SOUND_MANAGER->PlaySoundEffect(L"PowerDown");
		break;
	}

	// To
	switch (nShape)
	{
	case SHAPE_MINI:
		m_nDamageCooltime = GameDefaults::nDamageCooltime;
		break;
	case SHAPE_GIANT:
		m_nDamageCooltime = 0;
		break;
	}
	m_nNextShape = nShape;
}

void ControllableObject::OnChangeShape(int nShape)
{
	if (nShape == -1)
		return;

	// From
	switch (m_nShape)
	{
	case SHAPE_MINI:
		{
			if (m_bCollision[COLLISION_BOTTOM])
			{
				switch (m_nState)
				{
				case STATE_STAND:
				case STATE_WALK:
				case STATE_BREAK:
					{
						POINT ptCollidableOffset = FindCollidableOffset(COLLISION_TOP, 8);
						if (ptCollidableOffset.y <= 4)
							SetState(STATE_SIT);
					}
					break;
				}
			}
		}
		break;
	case SHAPE_GIANT:
		m_nGiantTime = 0;
		break;
	}

	// To
	switch (nShape)
	{
	case SHAPE_GIANT:
		m_nGiantTime = GameDefaults::nGiantTime;
		break;
	}

	// Scaling
	switch (nShape)
	{
	case SHAPE_GIANT:
		m_fScaleX = 4.0f;
		m_fScaleY = 4.0f;
		break;
	default:
		m_fScaleX = 1.0f;
		m_fScaleY = 1.0f;
		break;
	}

	m_nShape = nShape;
	m_nNextShape = -1;
}

bool ControllableObject::CanSit()
{
	if (m_pCarry)
		return false;

	if (m_pGame->IsActiveAbility(ABILITY_AUTORUN))
		return false;

	return true;
}

bool ControllableObject::CanFire()
{
	switch (m_nState)
	{
	case STATE_STAND:
	case STATE_WALK:
	case STATE_FALL:
	case STATE_SWIM:
	case STATE_JUMP:
		if (m_nShape == SHAPE_FIRE)
		{
			if (!IsSitSpriteIndex())
				return true;
		}
		break;
	case STATE_RIDE:
		if (m_nShape == SHAPE_FIRE && m_pVehicle)
		{
			int nRideType = ((VehicleObjectBase*)m_pVehicle)->m_nRideType;
			switch (nRideType)
			{
			case STATE_STAND:
			case STATE_CROUCH:
				return true;
			}
		}
	}
	return false;
}

void ControllableObject::ThrowFireball()
{
	int nFireBallCnt = 0;
	BEGIN_VECTOR_LOOP(GameObjectBase*, CUR_STAGE->m_vecBullet) {
		if (pObj->m_nType == BULLET_FIREBALL)
		{
			if (((FireBall*)pObj)->m_pOwner == this)
				nFireBallCnt++;
		}
	} END_VECTOR_LOOP();

	if (nFireBallCnt >= 2)
		return;

	CreateParameter param;
	param.pFromPlayer = m_pController;
	param.pFromObject = this;
	param.nType = BULLET_FIREBALL;
	
	auto rc = GetRect();
	CUR_STAGE->CreateBullet(m_fX, m_fY - rc.Height() / 2, BULLET_FIREBALL, &param);

	if (m_nState != STATE_SPINJUMP)
	{
		if (IS_CLASSIC)
			m_nThrowFireFrame = 8;
		else
			m_nThrowFireFrame = 16;
	}

	SOUND_MANAGER->PlaySoundEffect(L"ThrowFireball");
}

void ControllableObject::OnStand()
{
	if (IsInputHold(INPUT_L) && IsInputHold(INPUT_R))
	{
		;
	}
	else if (IsInputHold(INPUT_L) || IsInputHold(INPUT_R))
	{
		ChangeState(STATE_WALK);
		if (IsInputHold(INPUT_L))
			m_bFlip = true;
		else
			m_bFlip = false;
	}
	else if (IsInputHold(INPUT_D) && m_bCollision[COLLISION_BOTTOM])
	{
		// #TODO move to stage? gameobject?
		PipeInfo *pPipeUnderMe = FindPipeNearMe(COLLISION_BOTTOM);
		if (pPipeUnderMe)
		{
			EnterPipe(pPipeUnderMe);
			return;
		}
		else if (CanSit())
			ChangeState(STATE_SIT);
	}
	else if (CheckJump())
	{
		;
	}
	else if (CheckPressEnemy())
	{
		ChangeState(STATE_PRESSJUMP);
	}
	else if (IsInputPressed(INPUT_U))
	{
		GameObjectBase *pDoor = FindDoorNearMe();
		if (pDoor)
		{
			EnterDoor(pDoor);
			return;
		}

		ChangeState(STATE_SPECIALPOSE);
	}

	GameObjectBase::OnStand();
}

void ControllableObject::OnSit()
{
	float fMark = m_bFlip ? (-1) : 1;

	if (m_fXS != 0) 
	{
		if (m_nShape == SHAPE_SHELL && abs(m_fXS) > 1.0f)
		{
			ChangeState(STATE_SHELLRUN);
			return;
		}

		m_fXS -= fMark * 0.05f;
		
		if (m_fXS * fMark < 0)
			m_fXS = 0;

		CheckReachWall();
	}

	CheckFalling();

	POINT ptCollidableOffset = FindCollidableOffset(COLLISION_TOP, 8);
	if (!IsInputHold(INPUT_D) && ptCollidableOffset.y > 4)
	{
		if (m_fXS == 0)
			ChangeState(STATE_STAND);
		else
			ChangeState(STATE_WALK);
	}
	else if (IsInputPressed(INPUT_J) && m_bCollision[COLLISION_BOTTOM])
	{
		if (m_pGame->IsActiveAbility(ABILITY_SPINJUMP) && IsInputHold(INPUT_U) && ptCollidableOffset.y > 4)
			ChangeState(STATE_SPINJUMP);
		else
			ChangeState(STATE_SITJUMP);
	}
	else if (CheckPressEnemy())
	{
		ChangeState(STATE_PRESSJUMP);
	}
}

void ControllableObject::OnWalk()
{
	bool bSwim = CanSwim();
	bool bBoost = bSwim ? false : IsInputHold(INPUT_F);
	if (!bBoost)
	{
		if (m_pGame->IsActiveAbility(ABILITY_AUTOACCEL))
			bBoost = true;
	}

	bool bFront = m_bFlip ? IsInputHold(INPUT_L) : IsInputHold(INPUT_R);
	bool bBack = m_bFlip ? IsInputHold(INPUT_R) : IsInputHold(INPUT_L);
	float fMark = m_bFlip ? (-1) : 1;

	float fWalkSpeed = m_PhysicalProps.fAccelWalk; // 0.05f;
	float fMaxSpeed = m_PhysicalProps.fMaxWalkSpeed; // 1.5f;
	if (bSwim)
	{
		if (m_nShape != SHAPE_SHELL)
			fMaxSpeed = m_PhysicalProps.fMaxWalkSpeed / 1.5f; // 1.0f;
		else
			fMaxSpeed = m_PhysicalProps.fMaxWalkSpeed; // 1.5f;
	}

	if (bBoost)
	{
		fWalkSpeed = m_PhysicalProps.fAccelRun; // 0.1f;
		if (m_pGame->m_nStagePackType != STAGEPACK_CLASSIC)
		{
			if (m_nInvinsibleTime > 0)
				fMaxSpeed = m_PhysicalProps.fMaxRunSpeedInvinsible; // 4.0f;
			else
				fMaxSpeed = m_PhysicalProps.fMaxRunSpeedNonClassic; // 3.5f;
		}
		else
			fMaxSpeed = m_PhysicalProps.fMaxRunSpeedClassic; // 3.0f;
	}

	if (bFront && !bBack)
	{
		// Normal Move
		if (m_fXS * fMark < fMaxSpeed)
			m_fXS += (fMark) * (fWalkSpeed);

		// StopPress AccelKey -> SlowDown
		if (!bBoost && (m_fXS * fMark > fMaxSpeed))
			m_fXS += (-fMark) * (fWalkSpeed);

		if (!m_bFlip && m_bCollision[COLLISION_RIGHT])
		{
			PipeInfo *pPipeInfo = FindPipeNearMe(COLLISION_RIGHT);
			if (pPipeInfo)
			{
				EnterPipe(pPipeInfo);
				return;
			}
		}
		else if (m_bFlip && m_bCollision[COLLISION_LEFT])
		{
			PipeInfo *pPipeInfo = FindPipeNearMe(COLLISION_LEFT);
			if (pPipeInfo)
			{
				EnterPipe(pPipeInfo);
				return;
			}
		}
	}
	else 
	{ 
		// StopPress MoveKey -> SlowDown
		// 1.2f: to make fix slippery move
		m_fXS += (-fMark) * (m_PhysicalProps.fDecelWalk);

		if (m_bFlip ? (m_fXS > 0) : (m_fXS < 0))
		{
			m_fXS = 0;
			ChangeState(STATE_STAND);
		}
	}

	if (bBack && !bFront)
	{
		m_bFlip = !m_bFlip;
		ChangeState(STATE_BREAK);
	}

	if (IsInputHold(INPUT_D) && m_bCollision[COLLISION_BOTTOM]) 
	{
		PipeInfo *pPipeUnderMe = FindPipeNearMe(COLLISION_BOTTOM);
		if (pPipeUnderMe)
		{
			EnterPipe(pPipeUnderMe);
			return;
		}
		else if (CanSit())
			ChangeState(STATE_SIT);
	}
	else if (IsInputPressed(INPUT_U))
	{
		GameObjectBase *pDoor = FindDoorNearMe();
		if (pDoor)
		{
			EnterDoor(pDoor);
			return;
		}
	}

	// Walking Jump
	if (CheckJump())
	{
		;
	}
	else if (CheckPressEnemy())
	{
		ChangeState(STATE_PRESSJUMP);
	}

	// Fall Down
	GameObjectBase::OnWalk();

	// Process WalkFrame
	int nFriction;
	if (abs(m_fXS) < 1.5f)
		nFriction = 5;
	else
		nFriction = MAX(2, (5 - abs(m_fXS)));

	if (m_nShape == SHAPE_GIANT)
		nFriction *= 3;

	if (m_nStateFrame % (nFriction) == 0 && m_nNextShape == -1)
		m_nWalkFrame++;
}

void ControllableObject::OnShellRun()
{
	if (!IsInputHold(INPUT_D) || m_nShape != SHAPE_SHELL)
	{
		if (m_fYS == 0)
			ChangeState(STATE_WALK);
		else 
			ChangeState(STATE_FALL);
		return;
	}

	// SlowDown
	float fMark = m_bFlip ? (-1) : 1;
	m_fXS += (-fMark) * (0.002f);

	if (m_bFlip ? (m_fXS > 0) : (m_fXS < 0))
	{
		m_fXS = 0;
		ChangeState(STATE_SHELLIDLE);
	}

	if (!m_bCollision[COLLISION_BOTTOM])
	{
		if (m_nJumpHold > 0)
		{
			if (!IsInputHold(INPUT_J))
				m_nJumpHold = 0;
			else 
				m_nJumpHold--;
		}
		else
		{
			OnApplyGravity(1, 4.0f);
		}
	}
	else if (m_fYS == 0)
	{
		if (IsInputPressed(INPUT_J))
		{
			m_nJumpHold = 12;
			m_fYS = -4.0f;
			SOUND_MANAGER->PlaySoundEffect(L"JumpBig");
		}
	}
	else if (m_fYS > 0)
	{
		if ((int)m_fY <= m_ptCollision[COLLISION_BOTTOM].y)
		{
			m_fYS = 0;
			OnLanding();
		}
	}

	if (m_fYS < 0)
	{
		if (m_bCollision[COLLISION_TOP])
		{
			int nPower = GetPower(POWERTYPE_HITBLOCK);
			if (CheckHitBlock(nPower))
				return;
		}
	}

	if (m_nNextState != STATE_FALL)
	{
		if (CheckReachWall())
			return;
	}

	if (m_bStandOnJumpBlock)
		ChangeState(STATE_BLOCKJUMP);

	int nFriction;
	if (abs(m_fXS) < 1.5f)
		nFriction = 5;
	else
		nFriction = MAX(2, (5 - abs(m_fXS)));

	if (m_nStateFrame % (nFriction) == 0 && m_nNextShape == -1)
		m_nWalkFrame++;
	if (m_nWalkFrame > 3)
		m_nWalkFrame = 0;
}

void ControllableObject::OnShellIdle()
{
	if (!IsInputHold(INPUT_D) || m_nShape != SHAPE_SHELL)
	{
		ChangeState(STATE_STAND);
		return;
	}

	if (!m_bCollision[COLLISION_BOTTOM])
	{
		OnApplyGravity(1, 4.0f);
	}
	else if (m_fYS > 0)
	{
		if ((int)m_fY <= m_ptCollision[COLLISION_BOTTOM].y)
		{
			m_fYS = 0;
			OnLanding();
		}
	}
}

void ControllableObject::OnReachWall(bool bIsLeftWall)
{
	GameObjectBase::OnReachWall(bIsLeftWall);

	switch (m_nState)
	{
	case STATE_SHELLRUN:
		{
			POINT ptTile;
			if (bIsLeftWall)
				ptTile = m_ptCollision[COLLISION_LEFT];
			else
				ptTile = m_ptCollision[COLLISION_RIGHT];

			int nPower = POWER_HITBLOCK_BIGMARIO;
			CUR_STAGE->HitBlock(
				ptTile.x / TILE_XS,
				ptTile.y / TILE_YS,
				nPower,
				this,
				this);

			if ((!bIsLeftWall && m_fXS > 0 && !m_bFlip) ||
				(bIsLeftWall && m_fXS < 0 && m_bFlip))
			{
				m_fXS *= -1;
				m_bFlip = !m_bFlip;
			}
		}
		break;
	default:
		m_fXS = 0;
		break;
	}
}

void ControllableObject::OnBreak()
{
	bool bBoost = IsInputHold(INPUT_F);
	bool bFront = m_bFlip ? IsInputHold(INPUT_L) : IsInputHold(INPUT_R);
	bool bBack = m_bFlip ? IsInputHold(INPUT_R) : IsInputHold(INPUT_L);
	float fMark = m_bFlip ? (-1) : 1;

	m_fXS += (fMark) * (m_PhysicalProps.fAccelSkid); // 0.1f
	if (//m_nStateFrame > 10 &&
		((!m_bFlip && m_fXS > 0) || (m_bFlip && m_fXS < 0)))
		ChangeState(STATE_WALK);

	// #TODO When break, user try break again?

	// Walking Jump
	if (CheckJump())
	{
		;
	}
	else if (CheckPressEnemy())
	{
		ChangeState(STATE_PRESSJUMP);
	}

	if (m_pGame->m_bModernStyle && m_nStateFrame % 4 == 0)
	{
		// Skid dust
		int nDustX = m_fX + (rand() % 10) - 5;
		int nDustY = m_fY + (rand() % 5) - 3;

		CreateParameter param;
		param.nType = EFFECT_SMALLDUST;
		CUR_STAGE->CreateEffect(nDustX, nDustY, EFFECT_DUST, &param);
	}

	CheckReachWall();

	GameObjectBase::OnBreak();
}

void ControllableObject::OnJump()
{
	if (m_bCollision[COLLISION_TOP] && IsInputHold(INPUT_U))
	{
		PipeInfo *pPipeInfo = FindPipeNearMe(COLLISION_TOP);
		if (pPipeInfo)
		{
			EnterPipe(pPipeInfo);
			return;
		}
	}

	int nPower = GetPower(POWERTYPE_HITBLOCK);
	if (CheckHitBlock(nPower))
		return;

	OnAir();

	if (CanSwim())
	{
		ChangeState(STATE_SWIM);

		CreateParameter param;
		param.fScaleX = m_fScaleX;
		param.fScaleY = m_fScaleY;
		CUR_STAGE->CreateEffect(m_fX, m_fY, EFFECT_SPLASH, &param);
		return;
	}

	if (m_fYS >= 0 || m_nStateFrame >= 24)
	{
		if (CheckPressEnemy())
		{
			ChangeState(STATE_PRESSJUMP);
			return;
		}
	}

	// up frame
	// +-- 5 --+ +---- 7 ----+ +------ 9 ------+ +---- 7 ----+ +--- 6 ---+
	// 4 4 4 4 4 3 3 3 3 3 3 3 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 0 0 0 0 0 0

	// down frame
	// 1 1 2 2 3 3 3 4 4 5 5 4 4 4 5 4 4 4 5

	if (m_fYS <= 0)
	{
		if (m_nJumpHold > 0)
		{
			if (IsInputHold(INPUT_J))
			{
				// more higher
				m_nJumpHold--;
			}
			else
			{
				m_fYS = -1.0f;
				m_nJumpHold = -27;
			}
		}

		int nJumpFrame = m_nStateFrame;
		if (m_nJumpHold < 0)
			nJumpFrame = m_nStateFrame - m_nJumpHold;

		switch (nJumpFrame)
		{
		case 5:  // 4 -> 3
		case 12: // 3 -> 2
		case 21: // 2 -> 1
		case 28: // 1 -> 0
			m_fYS += 1.0f;
			break;
		}
		if (nJumpFrame > 33)
			ChangeState(STATE_FALL);
	}
}

void ControllableObject::OnTrampolinJump()
{
	int nPower = GetPower(POWERTYPE_HITBLOCK);

	if (CheckHitBlock(nPower))
		return;

	OnFall();
}

void ControllableObject::OnFall()
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

	if (CanSwim())
	{
		ChangeState(STATE_SWIM);
		
		CreateParameter param;
		param.fScaleX = m_fScaleX;
		param.fScaleY = m_fScaleY;
		CUR_STAGE->CreateEffect(m_fX, m_fY, EFFECT_SPLASH, &param);
		return;
	}

	if (CheckPressEnemy())
	{
		ChangeState(STATE_PRESSJUMP);
	}
	else if (CheckRideVehicle())
	{
		ChangeState(STATE_RIDE);
		return;
	}
	else if (!m_bCollision[COLLISION_BOTTOM])
	{
		OnApplyGravity(1, 4.0f);
	}
	else if ((int)m_fY - 4 <= m_ptCollision[COLLISION_BOTTOM].y)
	{
		m_fYS = 0;
		m_fY = m_ptCollision[COLLISION_BOTTOM].y;

		if (m_nInvinsibleTime == 0)
			StopCombo();

		POINT ptCollidableOffset = FindCollidableOffset(COLLISION_TOP, 8);
		if (m_bStandOnJumpBlock)
			ChangeState(STATE_BLOCKJUMP);
		else if ((IsInputHold(INPUT_D) && CanSit()) || (IsSitSpriteIndex() && ptCollidableOffset.y < 4))
			ChangeState(STATE_SIT);
		else if (m_fXS != 0)
			ChangeState(STATE_WALK);
		else if (m_fXS == 0)
			ChangeState(STATE_STAND);

		if (m_nShape == SHAPE_GIANT)
		{
			CUR_STAGE->MakeEarthquake();
			SOUND_MANAGER->PlaySoundEffect(L"Cannon");
		}
	}
	else if (CanSwim())
	{
		ChangeState(STATE_SWIM);
	}
	
	if (m_nNextState == -1)
		OnAir();
}

void ControllableObject::OnSwim()
{
	if (!CanSwim())
	{
		if (IsInputHold(INPUT_U))
			m_fYS -= 1.0f;
		ChangeState(STATE_FALL);
		return;
	}

	float fMaxSpeed = 1.0f;
	float fAccel = 0.05f;
	if (m_nShape == SHAPE_SHELL)
	{
		fMaxSpeed += 0.75f;
		fAccel *= 2.0f;
	}

	if (m_pCarry != nullptr)
	{
		fMaxSpeed += 0.75f;
	}

	if (m_bFlip)
	{
		if (IsInputHold(INPUT_L))
		{
			if (m_fXS > -(fMaxSpeed))
				m_fXS -= (fAccel);
		}
		else if (IsInputHold(INPUT_R))
		{
			m_bFlip = false;
			if (m_fXS < fMaxSpeed)
				m_fXS += (fAccel * 2.0f);
		}
	}
	else
	{
		if (IsInputHold(INPUT_R))
		{
			if (m_fXS < fMaxSpeed)
				m_fXS += fAccel;
		}
		else if (IsInputHold(INPUT_L))
		{
			m_bFlip = true;
			if (m_fXS > -(fMaxSpeed))
				m_fXS -= (fAccel * 0.2f);
		}
	}

	int nPower = GetPower(POWERTYPE_HITBLOCK);

	CheckHitBlock(nPower);
	CheckReachWall();

	SIZE s = GetSize();

	if (CheckPressEnemy())
	{
		ChangeState(STATE_PRESSJUMP);
	}
	else if (CheckRideVehicle())
	{
		ChangeState(STATE_RIDE);
		return;
	}
	else if (!m_bCollision[COLLISION_BOTTOM])
	{
		if (IsInputPressed(INPUT_J))
		{
			ChangeState(STATE_SWIMJUMP);
		}
		else
		{
			OnApplyGravity(0.75f, 4.0f);
		}
	}
	else if ((int)m_fY - 2 <= m_ptCollision[COLLISION_BOTTOM].y)
	{
		m_fYS = 0;
		m_fY = m_ptCollision[COLLISION_BOTTOM].y;

		if (IsInputHold(INPUT_D) && CanSit())
			ChangeState(STATE_SIT);
		else if (m_fXS != 0)
			ChangeState(STATE_WALK);
		else if (m_fXS == 0)
			ChangeState(STATE_STAND);
	}
}

void ControllableObject::OnSwimJump()
{
	ChangeState(STATE_SWIM);
}

void ControllableObject::OnSpinJump()
{
	if (m_nStateFrame % 8 == 0)
	{
		SIZE s = GetSize();
		CreateParameter param;
		param.fScaleX = 2.0f * m_fScaleX;
		param.fScaleY = 1.0f * m_fScaleY;
		CUR_STAGE->CreateEffect(m_fX, m_fY - (s.cy / 2), EFFECT_SPIN, &param);
	}

	if (m_nShape == SHAPE_FIRE)
	{
		if (m_nStateFrame == 8)
			ThrowFireball();
		if (m_nStateFrame == 16)
		{
			m_bFlip = !m_bFlip;
			ThrowFireball();
			m_bFlip = !m_bFlip;
		}
	}

	if (m_bCollision[COLLISION_TOP] && IsInputHold(INPUT_U))
	{
		PipeInfo *pPipeInfo = FindPipeNearMe(COLLISION_TOP);
		if (pPipeInfo)
		{
			EnterPipe(pPipeInfo);
			return;
		}
	}

	int nPower = GetPower(POWERTYPE_HITBLOCK);
	if (CheckHitBlock(nPower))
		return;

	OnAir();

	if (m_fYS >= 0 || m_nStateFrame >= 24)
	{
		if (CheckPressEnemy(POWER_PRESS_YOSHI))
		{
			ChangeState(STATE_SPINJUMP);
			return;
		}
	}

	if (m_fYS <= 0)
	{
		if (m_nJumpHold > 0)
		{
			if (m_nJumpHold > 15 || IsInputHold(INPUT_J))
			{
				// more higher
				m_nJumpHold--;
			}
			else
			{
				m_fYS = -1.0f;
				m_nJumpHold = -27;
			}
		}

		int nJumpFrame = m_nStateFrame;
		if (m_nJumpHold < 0)
			nJumpFrame = m_nStateFrame - m_nJumpHold;

		switch (nJumpFrame)
		{
		case 5:  // 4 -> 3
		case 12: // 3 -> 2
		case 21: // 2 -> 1
		case 28: // 1 -> 0
			m_fYS += 1.0f;
			break;
		}

		if (nJumpFrame > 33)
			OnApplyGravity(0.5f, 4.0f);
	}

	if (m_fYS >= 0)
	{
		if (CheckRideVehicle())
		{
			ChangeState(STATE_RIDE);
			return;
		}
		else if (!m_bCollision[COLLISION_BOTTOM])
		{
			OnApplyGravity(0.5f, 4.0f);
		}
		else if ((int)m_fY - 4 <= m_ptCollision[COLLISION_BOTTOM].y)
		{
			m_fYS = 0;
			m_fY = m_ptCollision[COLLISION_BOTTOM].y;

			if (m_nInvinsibleTime == 0)
				StopCombo();

			// #TODO Destroy Block under my foot
			if (m_bStandOnJumpBlock)
				ChangeState(STATE_BLOCKJUMP);
			else if (m_nShape == SHAPE_MINI)
			{
				if (m_fXS != 0)
					ChangeState(STATE_WALK);
				else
					ChangeState(STATE_STAND);
			}
			else 
			{
				int nTX = (m_fX) / TILE_XS;
				int nTY = (m_ptCollision[COLLISION_BOTTOM].y + 2) / TILE_YS;
				int nData = CUR_STAGE->GetTileData(nTX, nTY);
				int nType = CUR_STAGE->GetDataType(nData);
				if (nType == TILETYPE_SOFTHARD && CUR_STAGE->CanCrush(nTX, nTY, POWER_HITBLOCK_BIGMARIO))
				{
					CUR_STAGE->CrushBlock(nTX, nTY, this);
					ChangeState(STATE_SPINJUMP);

					SOUND_MANAGER->PlaySoundEffect(L"BreakBlock");
					return;
				}
				else
				{
					nTX = (m_ptCollision[COLLISION_BOTTOM].x) / TILE_XS;
					nTY = (m_ptCollision[COLLISION_BOTTOM].y + 2) / TILE_YS;
					nData = CUR_STAGE->GetTileData(nTX, nTY);
					nType = CUR_STAGE->GetDataType(nData);
					if (nType == TILETYPE_SOFTHARD && CUR_STAGE->CanCrush(nTX, nTY, POWER_HITBLOCK_BIGMARIO))
					{
						CUR_STAGE->CrushBlock(nTX, nTY, this);
						ChangeState(STATE_SPINJUMP);

						SOUND_MANAGER->PlaySoundEffect(L"BreakBlock");
						return;
					}
					else if (IsInputHold(INPUT_D) && CanSit())
						ChangeState(STATE_SIT);
					else if (m_fXS != 0)
						ChangeState(STATE_WALK);
					else if (m_fXS == 0)
						ChangeState(STATE_STAND);
				}
			}

			if (m_nShape == SHAPE_GIANT)
			{
				CUR_STAGE->MakeEarthquake();
				SOUND_MANAGER->PlaySoundEffect(L"Cannon");
			}
		}
		else if (CanSwim())
		{
			ChangeState(STATE_SWIM);
		}
	}
}

void ControllableObject::OnRide()
{
	if (IsInputHold(INPUT_U) && IsInputPressed(INPUT_J))
	{
		m_fXS = m_pVehicle->m_fXS;

		m_pVehicle->OnGetOffPassenger();

		ChangeState(STATE_JUMP);
		return;
	}

	// Let my vehicle do anything.
}

void ControllableObject::OnPipeUp()
{
}

void ControllableObject::OnPipeDown()
{
}

void ControllableObject::OnPipeLeft()
{
}

void ControllableObject::OnPipeRight()
{
}

void ControllableObject::OnDie()
{
	switch (m_nStateFrame)
	{
	case 15:
		m_fYS = -4.0f;
		break;
	case 24:
	case 30:
	case 32:
	case 40:
	case 45:
	case 52:
	case 61:
		m_fYS += 1.0f;
		break;
	case 38:
		m_fYS += 0.5f;
		break;
	}
}

void ControllableObject::OnBlockJump()
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

		bool bFront = m_bFlip ? IsInputHold(INPUT_L) : IsInputHold(INPUT_R);
		bool bBack = m_bFlip ? IsInputHold(INPUT_R) : IsInputHold(INPUT_L);
		float fMark = m_bFlip ? (-1) : 1;

		if (bBack && !bFront)
		{
			float fWalkSpeed = 0.05f;
			m_fXS += (-fMark) * (fWalkSpeed);
			if (m_bFlip ? (m_fXS > 0) : (m_fXS < 0))
			{
				m_fXS = 0;
				m_bFlip = !m_bFlip;
			}
		}

		CheckReachWall();
	}

	if (m_nStateFrame >= GameDefaults::nBlockJumpFreeze && 
		m_nStateFrame <= GameDefaults::nBlockJumpFreeze + 2)
	{
		if (IsInputHold(INPUT_J))
		{
			m_fYS = -8.0f;
		}
		else
			m_fYS = -5.0f;
	}

	if (m_nStateFrame > GameDefaults::nBlockJumpFreeze)
	{
		OnApplyGravity(1, 4.0f);

		int nPower = GetPower(POWERTYPE_HITBLOCK);
		if (CheckHitBlock(nPower))
			return; 
		
		if (m_fYS > 0)
		{
			ChangeState(STATE_FALL);
		}

		OnAir();
	}
}

void ControllableObject::OnParalyze()
{
	if (m_nStateFrame > 24)
	{
		ChangeState(STATE_STAND);
	}
}

void ControllableObject::OnSpecialPose()
{
	if (!CheckFalling())
	{
		if (!IsInputHold(INPUT_U))
			ChangeState(STATE_STAND);

		if (CheckJump())
		{
			;
		}
	}
}

void ControllableObject::OnVictoryPose()
{

}

void ControllableObject::OnLanding()
{
	m_fY = m_ptCollision[COLLISION_BOTTOM].y;

	if (m_bStandOnJumpBlock)
		ChangeState(STATE_BLOCKJUMP);
	else if (m_nState == STATE_SHELLRUN || m_nState == STATE_SHELLIDLE)
		return;
	else if (m_fXS != 0)
		ChangeState(STATE_WALK);
	else
		ChangeState(STATE_STAND);
}

void ControllableObject::OnApplyItem(GameObjectBase *pItemObject)
{
	if (pItemObject->m_nType == ITEM_VINE ||
		pItemObject->m_nType == EVENTOBJECT_VINELADDER)
	{
		if (m_nState != STATE_CLIMBIDLE && m_nState != STATE_CLIMBMOVE &&
			m_nState != STATE_VINEUP &&
			m_pCarry == nullptr)
		{
			while(true)
			{
				if (m_pVehicle != nullptr)
					break;

				if (m_nShape == SHAPE_GIANT)
					break;

				if (m_pGame->m_nStagePackType != STAGEPACK_CLASSIC)
				{
					if (!IsInputHold(INPUT_U))
						break;

					if (m_nState == STATE_JUMP && m_nStateFrame < 24)
						break;
				}

				ChangeStateInfo info;
				info.pFromObject = pItemObject;
				ChangeState(STATE_CLIMBIDLE, &info);

				break;
			}
		}
	}
	else
	{
		NaDebugOut(L"%ls get an item %ls!\n", GetTypeName(), pItemObject->GetTypeName());
		OnApplyItem(pItemObject->m_nType);
	}
}

void ControllableObject::OnApplyItem(int nType)
{
	// Sound Part
	switch (nType)
	{
	case ITEM_COIN:
	case ITEM_THROWNCOIN:
		SOUND_MANAGER->PlaySoundEffect(L"Coin");
		break;
	case ITEM_VINE:
		break;
	case ITEM_1UP:
		// SOUND_MANAGER->PlaySoundEffect(L"1UP");
		break;
	case ITEM_GIANTMUSHROOM:
		if (m_pVehicle == nullptr)
		{
			SOUND_MANAGER->PlaySoundEffect(L"PowerUpMega");
		}
		break;
	case ITEM_DOORKEYCOIN:
		//SOUND_MANAGER->PlaySoundEffect(L"DoorKeyCoin");
		SOUND_MANAGER->PlaySoundEffect(L"Coin");
		break;
	case ITEM_DOORKEY:
		SOUND_MANAGER->PlaySoundEffect(L"DoorKey");
		return;
		break;
	default:
		SOUND_MANAGER->PlaySoundEffect(L"PowerUp");
		break;
	}

	// Action Part
	int nScore = 0;
	switch (nType)
	{
	case ITEM_COIN:
	case ITEM_THROWNCOIN:
		{
			m_pController->AddCoin();
			m_pController->AddScore(200);
		}
		break;
	case ITEM_DOORKEYCOIN:
		m_pController->AddDoorKeyCoin();
		break;
	case ITEM_MUSHROOM:
		if (m_nShape == SHAPE_MINI)
		{
			ChangeShape(SHAPE_BIG);
			m_pGame->ChangeState(GAMESTATE_SHAPECHANGE);
		}

		nScore = 1000;
		break;
	case ITEM_1UP:
		nScore = 10000;
		break;
	case ITEM_FIREFLOWER:
		if (m_nShape != SHAPE_FIRE && m_nShape != SHAPE_GIANT)
		{
			ChangeShape(SHAPE_FIRE);
			m_pGame->ChangeState(GAMESTATE_SHAPECHANGE);
		}

		nScore = 1000;
		break;
	case ITEM_TURTLESHELL:
		if (m_nShape != SHAPE_SHELL && m_nShape != SHAPE_GIANT)
		{
			ChangeShape(SHAPE_SHELL);
			m_pGame->ChangeState(GAMESTATE_SHAPECHANGE);
		}

		nScore = 1000;
		break;
	case ITEM_SUPERSTAR:
		nScore = 1000;

		// fast 24 sec
		// slow 8 sec
		m_nInvinsibleTime = GameDefaults::nInvinsibleTime;

		SOUND_MANAGER->PlayInvinsibleBGM();		
		break;
	case ITEM_VINE:		
		break;
	case ITEM_GIANTMUSHROOM:
		if (m_pVehicle)
			nScore = 10000;
		else
		{
			if (m_nShape != SHAPE_GIANT)
			{
				ChangeShape(SHAPE_GIANT);
				m_pGame->ChangeState(GAMESTATE_SHAPECHANGE);

				SOUND_MANAGER->StopBGM();
			}

			nScore = 1000;
		}
		break;
	default:
		if (nType >= ITEM_MYSTERYMUSHROOM &&
			nType <= ITEM_MYSTERYMUSHROOM_RESERVED_LAST)
			{
				if (m_nShape == SHAPE_GIANT)
				{
					nScore = 1000;
					break;
				}
				else
				{
					int nCostume = nType - ITEM_MYSTERYMUSHROOM - 1;
					if (nCostume == -1)
					{
						nCostume = rand() % (ITEM_MYSTERYMUSHROOM_RESERVED_LAST - ITEM_MYSTERYMUSHROOM);

						switch (nCostume)
						{
						case 5 + 1:
						case 149 + 1:
						case 151 + 1:
							nCostume--;
							break;
						}
					}

					NaDebugOut(L"-> Random Costume: %d\n", nCostume);

					if (m_nShape != SHAPE_COSTUME || m_nCostume != nCostume)
					{
						m_nCostume = nCostume;
						ChangeShape(SHAPE_COSTUME);
						m_pGame->ChangeState(GAMESTATE_SHAPECHANGE);
					}
				}
			}

			nScore = 1000;
			break;
		break;
	}

	if (nScore != 0)
	{
		CUR_PLAYER->AddScoreWithEffect(nScore, this);
	}
}

void ControllableObject::OnApplyEvent(GameObjectBase * pEventObject)
{
	switch (pEventObject->m_nType)
	{
	case EVENTOBJECT_FLAGPOLE:
		{
			ChangeStateInfo info;
			info.pFromObject = pEventObject;
			ChangeState(STATE_FLAGDOWN, &info);

			if (m_nGiantTime > 0)
				m_nGiantTime = 2;
			m_pGame->ChangeState(GAMESTATE_FLAGDOWN);
		}
		break;
	case EVENTOBJECT_GOALBOX:
		{
			if (m_nGiantTime > 0)
				m_nGiantTime = 2;
			m_pGame->ChangeState(GAMESTATE_HITGOALBOX);
		}
		break;
	case EVENTOBJECT_GOALPOST:
		{
			if (m_nGiantTime > 0)
				m_nGiantTime = 2;
			m_pGame->ChangeState(GAMESTATE_HITGOALPOST);
		}
		break;
	case EVENTOBJECT_GOALPOINT:
		{
			//ChangeState(STATE_STAND);
			m_pGame->ChangeState(GAMESTATE_GOAL);
		}
		break;
	case EVENTOBJECT_BOSSBRIDGE_AXE:
		{
			if (m_fXS < 0)
				m_fXS = 0;
			if (m_nGiantTime > 0)
				m_nGiantTime = 256;
			m_pGame->ChangeState(GAMESTATE_CUTBRIDGE);
		}
		break;
	case EVENTOBJECT_PRINCESS:
		{
			ChangeState(STATE_STAND);
			m_pGame->ChangeState(GAMESTATE_RESCUEPRINCESS);
		}
		break;
	case EVENTOBJECT_VORTEX:
		{
			NaRect rcVortex = pEventObject->GetRect();
			if (m_fX < rcVortex.Center())
			{
				if (!m_bCollision[COLLISION_RIGHT])
					m_fX += 0.25f;
			}
			if (m_fX > rcVortex.Center())
			{
				if (!m_bCollision[COLLISION_LEFT])
					m_fX -= 0.25f;
			}

			if (!m_bCollision[COLLISION_BOTTOM])
				m_fY += 0.5f;
		}
		break;
	case EVENTOBJECT_CHECKPOINT_FLAG:
		{
			if (!IS_CLASSIC && m_nShape == SHAPE_MINI)
			{
				ChangeShape(SHAPE_BIG);
				OnChangeShape(SHAPE_BIG);
			}

			auto pTopStage = CUR_STAGE->GetTopStage();
			if (CUR_STAGE->m_bIsSubStage)
				pTopStage->m_strLastCheckPoint = CUR_STAGE->m_strName;
			else
				pTopStage->m_strLastCheckPoint = L"";
			pTopStage->m_ptLastCheckPoint =
			{
				(int)pEventObject->m_fX, 
				(int)pEventObject->m_fY 
			};

			NaDebugOut(L"%ls activate CheckPoint: %ls / %d, %d\n",
				GetTypeName(),
				pTopStage->m_strLastCheckPoint.wstr(),
				CUR_STAGE->m_ptLastCheckPoint.x,
				CUR_STAGE->m_ptLastCheckPoint.y
			);
		}
		break;
	}
}

void ControllableObject::OnAir()
{
	float fMaxSpeedOnAir = 1.0f;
	float fAccelOnAir = 0.1f;
	
	/*
	if (m_pGame->m_nStagePackType != STAGEPACK_CLASSIC)
	{
		fMaxSpeedOnAir = 1.25f;
		fAccelOnAir = 0.15f;
	}
	*/

	if (m_bFlip) 
	{
		if (IsInputHold(INPUT_L))
		{
			if (m_fXS > -(fMaxSpeedOnAir))
				m_fXS -= (fAccelOnAir / 2.0f);
		}
		else if (IsInputHold(INPUT_R)) 
		{
			if (m_fXS < fMaxSpeedOnAir)
				m_fXS += (fAccelOnAir);
		}
	}
	else 
	{
		if (IsInputHold(INPUT_R))
		{
			if (m_fXS < fMaxSpeedOnAir)
				m_fXS += (fAccelOnAir / 2.0f);
		}
		else if (IsInputHold(INPUT_L))
		{
			if (m_fXS > -(fMaxSpeedOnAir))
				m_fXS -= (fAccelOnAir);
		}
	}

	CheckReachWall();
}

void ControllableObject::OnPressJump()
{
	int nSpeedFrame[] = {
		-4, -3, -3, -3, -3, -2, -2, -1, -1, -1, 
		0, 0, 1, 1, 1, 2,
	};

	int nSpeedFrameLen = sizeof(nSpeedFrame) / sizeof(int);
	if (m_nStateFrame < nSpeedFrameLen)
	{
		m_vecOverlappedEnemy.clear();
		m_fYS = nSpeedFrame[m_nStateFrame];
	}
	else
		m_fYS += 1.0f;

	int nPower = GetPower(POWERTYPE_HITBLOCK);
	if (CheckHitBlock(nPower))
		return;

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

void ControllableObject::OnFlagDown()
{
	if (m_nStateFrame == 0)
	{
		m_fXS = 0;
		m_fYS = 0;
	}

	if (m_pGame->m_nStagePackType != STAGEPACK_CLASSIC)
	{
		if (m_nStateFrame == 1)
			m_fYS = 2.0f;
	}
	else if (m_nStateFrame == 30)
		m_fYS = 2.0f;

	if (IsInputHold(INPUT_L) && !IsInputHold(INPUT_R))
	{
		if (!m_bFlip)
		{
			if (m_fX + 6 < m_pClimb->m_fX)
			{
				m_bFlip = true;
				m_fXS = -1.0f;
				ChangeState(STATE_WALK);

				return;
			}
		}
		else
		{
			if (m_fX < m_pClimb->m_fX)
				m_bFlip = false;
		}

		m_fXS = -0.5f;
	}
	else if (!IsInputHold(INPUT_L) && IsInputHold(INPUT_R))
	{
		if (m_bFlip)
		{
			if (m_fX - 6 > m_pClimb->m_fX)
			{
				m_bFlip = false;
				m_fXS = 1.0f;
				ChangeState(STATE_WALK);

				return;
			}
		}
		else
		{
			if (m_fX > m_pClimb->m_fX)
				m_bFlip = true;
		}

		m_fXS = 0.5f;
	}
	else if (!IsInputHold(INPUT_L) && !IsInputHold(INPUT_R))
	{
		m_fXS = 0;
	}

	if (IsInputPressed(INPUT_J))
	{
		ChangeState(STATE_JUMP);
	}

	if (m_bCollision[COLLISION_BOTTOM])
	{
		m_fYS = 0;
		ChangeState(STATE_CLIMBIDLE);
	}
}

void ControllableObject::OnFlagFlip()
{
	if (m_pClimb == nullptr)
	{
		m_bFlip = false;
		ChangeState(STATE_WALK);
		return;
	}

	if (!m_bFlip && m_fX >= m_pClimb->m_fX)
	{
		m_bFlip = true;
	}

	if (m_fX - m_pClimb->m_fX > 6)
	{
		m_pClimb = nullptr;
		m_bFlip = false;
		ChangeState(STATE_WALK);
	}
}

void ControllableObject::OnClimbIdle()
{
	if (!m_bCollision[COLLISION_TOP] && IsInputHold(INPUT_U))
	{
		ChangeState(STATE_CLIMBMOVE);
		m_fYS = -1.0f;
	}
	else if (!m_bCollision[COLLISION_BOTTOM] && IsInputHold(INPUT_D))
	{
		ChangeState(STATE_CLIMBMOVE);
		m_fYS = 2.0f;
	}
	else if (OnClimbCheckFlip())
		return;

	if (IsInputPressed(INPUT_J))
	{
		ChangeState(STATE_JUMP);
	}
	else
	{
		OnClimbCheckAway();
	}

	if (m_fYS > 0)
	{
		if (m_bCollision[COLLISION_BOTTOM])
		{
			if (IsInputHold(INPUT_D))
				ChangeState(STATE_STAND);
			else
				m_fYS = 0;
		}
		else if (m_fY - 8 > m_pClimb->m_fY)
		{
			ChangeState(STATE_FALL);
		}
	}
}

void ControllableObject::OnClimbMove()
{
	if (OnClimbCheckFlip())
		return;

	if (m_fYS < 0)
	{
		NaRect rc = GetRect();

		if (m_bCollision[COLLISION_TOP])
			ChangeState(STATE_CLIMBIDLE);
		else if (!IsInputHold(INPUT_U))
			ChangeState(STATE_CLIMBIDLE);
		else if (rc.top < 32 && m_pStage->CanEnterSky())
		{
			ChangeState(STATE_VINEUP);
			m_pGame->ChangeState(GAMESTATE_ENTERSKY);
		}
		else
		{
			NaRect rcClimb = m_pClimb->GetRect();
			if (m_pGame->m_nStagePackType == STAGEPACK_CLASSIC)
			{
				if (rc.top < rcClimb.top)
				{
					if (!m_bFlip)
						m_fX = m_pClimb->m_fX - 4;
					else
						m_fX = m_pClimb->m_fX + 4;

					m_fXS = 0;
					m_fYS = -2.0f;
					ChangeState(STATE_WALK);
				}
			}
			else
			{
				if (rc.top < rcClimb.top)
				{
					m_fY = rcClimb.top + rc.Height();
				}
			}
		}
	}
	else if (m_fYS > 0)
	{
		if (m_bCollision[COLLISION_BOTTOM])
		{
			if (IsInputHold(INPUT_D))
				ChangeState(STATE_STAND);
			else
				ChangeState(STATE_CLIMBIDLE);
		}
		else if (m_fY - 8 > m_pClimb->m_fY)
		{
			ChangeState(STATE_FALL);
		}
		else if (!IsInputHold(INPUT_D))
			ChangeState(STATE_CLIMBIDLE);
	}
	else if (m_fYS == 0)
	{
		ChangeState(STATE_CLIMBIDLE);
	}

	if (IsInputPressed(INPUT_J))
	{
		ChangeState(STATE_JUMP);
	}
}

bool ControllableObject::OnClimbCheckFlip()
{
	if (IsInputHold(INPUT_L) && !IsInputHold(INPUT_R))
	{
		if (!m_bFlip)
		{
			if (m_fX + 6 < m_pClimb->m_fX)
			{
				m_bFlip = true;
				m_fXS = -1.0f;
				ChangeState(STATE_WALK);

				return true;
			}
		}
		else
		{
			if (m_fX < m_pClimb->m_fX)
				m_bFlip = false;
		}

		m_fXS = -0.5f;
	}
	else if (!IsInputHold(INPUT_L) && IsInputHold(INPUT_R))
	{
		if (m_bFlip)
		{
			if (m_fX - 6 > m_pClimb->m_fX)
			{
				m_bFlip = false;
				m_fXS = 1.0f;
				ChangeState(STATE_WALK);

				return true;
			}
		}
		else
		{
			if (m_fX > m_pClimb->m_fX)
				m_bFlip = true;
		}

		m_fXS = 0.5f;
	}
	else if (!IsInputHold(INPUT_L) && !IsInputHold(INPUT_R))
	{
		m_fXS = 0;
	}

	return false;
}

void ControllableObject::OnClimbCheckAway()
{
	if (m_pClimb == nullptr)
		return;

	NaRect rc = GetRect();
	NaRect rcClimb = m_pClimb->GetRect();
	if (m_pGame->m_nStagePackType == STAGEPACK_CLASSIC)
	{
		if (rc.top < rcClimb.top)
		{
			if (!m_bFlip)
				m_fX = m_pClimb->m_fX - 4;
			else
				m_fX = m_pClimb->m_fX + 4;

			m_fXS = 0;
			m_fYS = -2.0f;
			ChangeState(STATE_WALK);
			return;
		}
	}
	else
	{
		if (rc.top < rcClimb.top)
		{
			m_fY = rcClimb.top + rc.Height();
		}
	}

	int nDist = 8;
	if (!m_bFlip)
	{
		if (m_fX + nDist < m_pClimb->m_fX)
		{
			m_bFlip = true;
			m_fXS = -1.0f;
			ChangeState(STATE_WALK);
			return;
		}
	}
	else 
	{
		if (m_fX - nDist > m_pClimb->m_fX)
		{
			m_bFlip = false;
			m_fXS = 1.0f;
			ChangeState(STATE_WALK);
			return;
		}
	}

}

void ControllableObject::OnInvinsible(bool bEffect)
{
	int nFreq = 4;
	if (m_nInvinsibleTime < 8 * 24)
		nFreq = 16;

	if (bEffect && (rand() % nFreq < 2))
	{
		for (int i = 0; i < (rand() % 4); i++)
		{
			NaRect rc = GetRect();
			rc.left -= 8;
			rc.top -= 8;
			rc.right += 8;
			rc.bottom += 8;
			int nX = rc.left + (rand() % rc.Width());
			int nY = rc.top + (rand() % rc.Height());
			CreateParameter param;
			if (rand() % 2 == 0)
				param.nType = EFFECT_PINKTWINKLE;
			else
				param.nType = EFFECT_GOLDTWINKLE;
			CUR_STAGE->CreateEffect(nX, nY, EFFECT_TWINKLE, &param);
		}
	}

	std::vector<GameObjectBase*>::iterator it = m_vecOverlappedEnemy.begin();
	for (; it != m_vecOverlappedEnemy.end(); ++it)
	{
		GameObjectBase *pEnemy = *it;
		if (pEnemy->IsAlive() && !pEnemy->IsInvinsible() && pEnemy->CanBurnt(BURN_STARMAN))
		{
			ChangeStateInfo info;
			info.nNextState = STATE_BURNT;
			info.pFromObject = this;
			info.nPower = BURN_STARMAN;
			pEnemy->ChangeState(STATE_BURNT, &info);
		}
	}
}

void ControllableObject::OnDamaged()
{
	if (CUR_PLAYER->m_pControlObj != this)
		return;

	if (m_nInvinsibleTime > 0)
		return;
	
	if (m_pVehicle)
	{
		m_pVehicle->OnDamagedPassenger();
	}
	else if (m_nShape == SHAPE_MINI)
	{
		ChangeState(STATE_DIE);
		m_pGame->ChangeState(GAMESTATE_DIE);
	}
	else if (m_pGame->m_bModernStyle && m_nShape == SHAPE_FIRE)
	{
		ChangeShape(SHAPE_BIG);
		m_pGame->ChangeState(GAMESTATE_SHAPECHANGE);

		SOUND_MANAGER->PlaySoundEffect(L"PowerDown");
	}
	else if (m_nShape == SHAPE_COSTUME)
	{
		ChangeShape(SHAPE_MINI);
		OnChangeShape(m_nNextShape);

		SOUND_MANAGER->PlaySoundEffect(L"PowerDown");
	}
	else
	{
		ChangeShape(SHAPE_MINI);
		m_pGame->ChangeState(GAMESTATE_SHAPECHANGE);

		SOUND_MANAGER->PlaySoundEffect(L"PowerDown");
	}
}

void ControllableObject::Carry(GameObjectBase * pObj)
{
	m_pCarry = pObj;

	ChangeStateInfo info;
	info.pFromObject = this;
	pObj->ChangeState(STATE_CARRYBYHAND, &info);

	NaDebugOut(L"%ls begins carry %ls!!\n",
		GetTypeName(), pObj->GetTypeName());
}

void ControllableObject::MoveCarryObject()
{
	if (m_pCarry)
	{
		POINT ptHand = GetHandPosition();

		m_pCarry->m_fX = m_fX + ptHand.x;
		m_pCarry->m_fY = m_fY + ptHand.y;
	}
}

void ControllableObject::Kick(GameObjectBase * pObj, bool bIsCarried)
{
	bool bEffect = false;
	if (bIsCarried)
	{
		ChangeStateInfo info;
		info.pFromObject = this;

		if (IsInputHold(INPUT_U))
		{			
			pObj->ChangeState(STATE_KICKEDUP, &info);

			bEffect = true;
			m_nKickFrame = 16;
			NaDebugOut(L"%ls kicks up (carried) %ls!!\n", GetTypeName(), pObj->GetTypeName());
		}
		else if (m_bCollision[COLLISION_BOTTOM] && IsInputHold(INPUT_D))
		{
			pObj->ChangeState(STATE_PLACEDBYHAND, &info);
			NaDebugOut(L"%ls places (carried) %ls!!\n", GetTypeName(), pObj->GetTypeName());
		}
		else
		{
			pObj->ChangeState(STATE_KICKED, &info);

			bEffect = true;
			m_nKickFrame = 16;
			NaDebugOut(L"%ls kicks (carried) %ls!!\n", GetTypeName(), pObj->GetTypeName());
		}

		CUR_STAGE->m_vecReservedGameObject.push_back(m_pCarry);
		m_pCarry = nullptr;
	}
	else
	{
		ChangeStateInfo info;
		info.pFromObject = this;
		pObj->ChangeState(STATE_KICKED, &info);

		bEffect = true;
		NaDebugOut(L"%ls kicks %ls!!\n", GetTypeName(), pObj->GetTypeName());
	}

	if (bEffect)
	{
		// Hit Effect
		auto rcObj = pObj->GetRect();
		CreateParameter param;
		CUR_STAGE->CreateEffect(pObj->m_fX, rcObj.Middle(), EFFECT_HIT, &param);
	}
}

bool ControllableObject::IsAlive()
{
	switch (m_nState)
	{
	case STATE_DIE:
	case STATE_DEAD:
		return false;
		break;
	}

	return true;
}

bool ControllableObject::HasMushroom()
{
	if (m_nShape == SHAPE_MINI)
		return false;
	return true;
}

bool ControllableObject::CanBurnt(int nType)
{
	if (m_nInvinsibleTime > 0)
		return false;
	if (m_nDamageCooltime > 0)
		return false;
	if (m_nGiantTime > 0)
		return false;
	return true;
}

bool ControllableObject::CanPressEnemy()
{
	if (m_pVehicle)
		return false;
	return true;
}

bool ControllableObject::CanCarry()
{
	if (!IsAlive())
		return false;

	switch (m_nState)
	{
	case STATE_SIT:
	case STATE_SPINJUMP:
	case STATE_SHELLRUN:
	case STATE_SHELLIDLE:
		return false;
		break;
	}

	if (m_nShape == SHAPE_GIANT || m_nNextShape == SHAPE_GIANT)
		return false;

	if (m_pVehicle)
		return false;

	if (m_nShape != SHAPE_COSTUME && m_pGame->IsActiveAbility(ABILITY_CARRYOBJECT))
	{
		if (m_nKickFrame > 0)
			return false;

		if (m_pCarry == nullptr && IsInputHold(INPUT_F))
			return true;
	}

	return false;
}

bool ControllableObject::IsSitSpriteIndex()
{
	return false;
}

int ControllableObject::GetPower(int nPowerForWhat)
{
	int nPower = 0;
	switch (nPowerForWhat)
	{
	case POWERTYPE_HITBLOCK:
		{
			nPower = POWER_HITBLOCK_NORMAL;
			if (m_nShape >= SHAPE_BIG)
				nPower = POWER_HITBLOCK_BIGMARIO;
		}
		break;
	case POWERTYPE_PRESS:
		if (m_nState == STATE_SPINJUMP)
			nPower = POWER_PRESS_YOSHI;
		else
			nPower = POWER_PRESS_NORMAL;
		break;
	}

	return nPower;
}

void ControllableObject::EnterPipe(PipeInfo * pPipeInfo)
{
	ChangeStateInfo info;
	GameStateChangeParameter param;
	param.m_pCurPipe = pPipeInfo;
	param.m_strOption = pPipeInfo->m_strStageName;

	switch (pPipeInfo->m_nType)
	{
	case PIPEENTRANCE_UPTODOWN:
		{
			m_fX = pPipeInfo->m_rcEnterance.left + pPipeInfo->m_rcEnterance.Width() / 2;
			ChangeState(STATE_PIPEDOWN, &info);
		}
		break;
	case PIPEENTRANCE_LEFTTORIGHT:
		ChangeState(STATE_PIPERIGHT, &info);
		break;
	case PIPEENTRANCE_DOWNTOUP:
		{
			m_fX = pPipeInfo->m_rcEnterance.left + pPipeInfo->m_rcEnterance.Width() / 2;
			ChangeState(STATE_PIPEUP, &info);
		}
		break;
	case PIPEENTRANCE_RIGHTTOLEFT:
		ChangeState(STATE_PIPELEFT, &info);
		break;
	default:
		return;
		break;
	}

	m_pGame->ChangeState(GAMESTATE_ENTERPIPE, &param);
}

void ControllableObject::EnterDoor(GameObjectBase * pObj)
{
	Door *pDoor = (Door*)pObj;
	if (pDoor)
	{
		if (pDoor->m_bLocked && m_pController)
		{
			if (!m_pController->HasDoorKey())
			{
				pDoor->ChangeState(STATE_DENY);				
				return;
			}
			else
			{
				m_pController->UseDoorKey();
				pDoor->ChangeState(STATE_UNLOCK);
			}
		}
		else
		{
			pDoor->ChangeState(STATE_OPEN);
		}

		GameStateChangeParameter param;
		param.pFromObject = pDoor;
		m_pGame->ChangeState(GAMESTATE_ENTERDOOR, &param);
	}
}

int ControllableObject::GetSpriteIndex()
{
	int nFrame = m_nSpriteIndex;

	return nFrame;
}

int ControllableObject::GetZOrder()
{
	switch (m_nState)
	{
	case STATE_PIPEDOWN:
	case STATE_PIPERIGHT:
	case STATE_PIPELEFT:
	case STATE_PIPEUP:
		return Z_IN_PIPE_OBJ;
	}

	if (m_pVehicle)
	{
		switch(m_pVehicle->m_nState)
		{
		case STATE_PIPEDOWN:
		case STATE_PIPERIGHT:
		case STATE_PIPELEFT:
		case STATE_PIPEUP:
			return Z_IN_PIPE_OBJ;
		}
	}

	return Z_COMMON_OBJ;
}

void ControllableObject::GetScale(float & fScaleX, float & fScaleY)
{
	fScaleX = m_fScaleX;
	fScaleY = m_fScaleY;

	// #TODO tuning minimum Scale value.

	switch (m_nState)
	{
	case STATE_PIPEDOWN:
		{
			SIZE s = GetSize();
			if (s.cx > 32)
			{
				float fMinScale = 1.0f;
				fScaleX = m_fScaleX - (m_nStateFrame * 0.1f);
				fScaleY = m_fScaleY - (m_nStateFrame * 0.1f);

				if (fScaleX < fMinScale)
					fScaleX = fScaleY = fMinScale;
			}
		}
		break;
	case STATE_PIPERIGHT:
		{
			SIZE s = GetSize();
			if (s.cy > 32)
			{
				float fMinScale = 1.0f;
				fScaleX = m_fScaleX - (m_nStateFrame * 0.1f);
				fScaleY = m_fScaleY - (m_nStateFrame * 0.1f);
				
				if (fScaleX < fMinScale)
					fScaleX = fScaleY = fMinScale;

				// Total 32frame
				m_fX += ((32.0f / 16.0f) / m_fScaleX) - m_fXS;
			}
		}
		break;
	case STATE_PIPEUP:
		{
			SIZE s = GetSize();
			if (s.cx > 32)
			{
				float fMinScale = 1.0f;
				fScaleX = m_fScaleX - ((30 - m_nStateFrame) * 0.1f);
				fScaleY = m_fScaleY - ((30 - m_nStateFrame) * 0.1f);

				if (fScaleX < fMinScale)
					fScaleX = fScaleY = fMinScale;
			}
		}
		break;
	}
}

SIZE ControllableObject::GetSize()
{
	SIZE s;
	int nSmallHeight = CUR_STAGE->m_ThemeSetting.m_nControllableObjectSmallHeight;
	int nBigHeight = CUR_STAGE->m_ThemeSetting.m_nControllableObjectBigHeight;

	if (m_nShape == SHAPE_MINI || m_nShape == SHAPE_COSTUME)
	{
		s.cx = 12;
		s.cy = nSmallHeight;
	}
	else
	{
		s.cx = 12;
		s.cy = nBigHeight;
		switch (m_nState)
		{
		case STATE_SIT:
		case STATE_SITJUMP:
		case STATE_SITFALL:
		case STATE_SHELLRUN:
		case STATE_SHELLIDLE:
			s.cx = 11;
			s.cy = nSmallHeight;
			break;
		default:
			s.cy = nBigHeight;
			break;
		}
	}

	s.cx *= m_fScaleX;
	s.cy *= m_fScaleY;
	return s;
}

POINT ControllableObject::GetHandPosition()
{
	POINT pt = { 12, -2 };
	
	switch (m_nState) 
	{
	case STATE_PIPEDOWN:
	case STATE_PIPEUP:
	case STATE_PIPELEFT:
	case STATE_PIPERIGHT:
		pt.x = 4;
		break;		
	}

	if (m_bFlip)
		pt.x *= -1;

	if (m_nShape > SHAPE_MINI)
		pt.y = -8;

	return pt;
}

void ControllableObject::GetColor(int *pnColor)
{
	
}

Sprite * ControllableObject::GetCostumeSprite(int nCostume)
{
	if (nCostume == -1)
		nCostume = m_nCostume;

	if (nCostume < 0)
		return nullptr;

	if ((m_bFlip && m_nState != STATE_BREAK) ||
		(!m_bFlip && m_nState == STATE_BREAK))
	{
		switch (nCostume)
		{
		case 5:
		case 149:
		case 151:
			nCostume++;
			break;
		}
	}

	NaString str;
	str.Format(L"Costume%03d", nCostume);
	return SPR_MANAGER->RequestCostume(L"Costume", str.wstr());
}

void ControllableObject::Render(int nColor, int nZOrder)
{
		
}

void ControllableObject::RenderShapeChange()
{
	
}
