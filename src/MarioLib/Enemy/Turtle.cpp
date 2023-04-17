#include "Turtle.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "EventObjectBase.h"
#include "SoundManager.h"

#include "SpeechBubble.h"

BEGIN_IMPL_PROPERTYMAP(Turtle)
    PROP_BOOL("IsEmptyShell", VT_BOOL, false, nullptr, "Turtle"),
	PROP_BOOL("PowerUp", VT_BOOL, false, nullptr, "Turtle"),
	PROP_BOOL("Winged", VT_BOOL, false, nullptr, "Turtle"),
END_IMPL_PROPERTYMAP()

Turtle::Turtle(Game *pGame, Stage *pStage) :
	EnemyObjectBase(pGame, pStage)
{
	m_nType = ENEMY_TURTLE;
	m_nState = STATE_IDLE;

	m_bFlip = true;
	m_bWinged = false;
	m_nCombo = 0;

	m_nSpriteIndex = 0;
	m_bDeadInside = false;
	m_nAwakeTimer = 0;
	m_nShape = TURTLESHAPE_NORMAL;
    m_bIsEmptyShell = false;
}

Turtle::~Turtle()
{
}

void Turtle::Process()
{
	ProcessBase();
	if (m_nInvinsibleTime > 0)
		m_nInvinsibleTime--;
}

void Turtle::OnBeforeProcess()
{
	if (m_nState == STATE_SHELLRUN && m_bPowerUp)
		CheckCollisionPowerUp();
	else
		CheckCollision();

	CheckCollisionEnemy();
}

void Turtle::OnAfterProcess()
{
	EnemyObjectBase::OnAfterProcess();

	if (m_nShape == TURTLESHAPE_SHELL && m_nAwakeTimer > 0 && m_fYS == 0 && m_nState != STATE_SHELLRUN)
	{
		m_nAwakeTimer--;
		if (m_nAwakeTimer == 0 && !m_bDeadInside && !m_bIsEmptyShell)
		{
			m_nShape = TURTLESHAPE_SHELLAWAKEN;
		}
	}
}

void Turtle::Render(int nColor, int nZOrder)
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

		if (m_nState == STATE_PUSHEDUP && !IS_CLASSIC)
		{
			// 8frame = 90'
			if (m_bFlipV == false)
				fAngle = m_nStateFrame * 11.25f;
		}

		if (m_nState == STATE_SHELLAWAKE && !IS_CLASSIC)
		{
			if (m_nStateFrame < 80)
			{
				fAngle = 10.0f * ((m_nStateFrame % 6) / 2) - 10.0f;
			}
			if (m_nStateFrame > 80 && m_nStateFrame < 89)
				fAngle = m_nStateFrame * 22.5f;
		}

		float fScaleX, fScaleY;
		GetScale(fScaleX, fScaleY);

        float _x = x - pt.x;
        float _y = y - pt.y;

        if (m_nState == STATE_STACKED)
            _x += GetStackXOffset();

		(*m_ppSprite)->RenderToQueue(_x, _y, nFrame, m_bFlip, m_bFlipV, nColor,
			fScaleX, fScaleY, nZOrder, bShadow, fAngle);

		// Wing
		if (m_bWinged)
		{
			RenderWing(nColor, nZOrder);
		}
	}	

	// For Debug State
	/*
	if (m_nType == ENEMY_TURTLE)
	{
		SpeechBubble b(m_pGame);
		NaString str;
		switch (m_nShape)
		{
		case TURTLESHAPE_SHELLKICKED: str = L"ShellKicked"; break;
		case TURTLESHAPE_SHELL: str = L"Shell"; break;
		case TURTLESHAPE_SHELLAWAKEN: str = L"ShellAwaken"; break;
		case TURTLESHAPE_NORMAL: str = L"Normal"; break;
		}

		str.AppendFormat(L"(%d)", m_nAwakeTimer);
		b.RenderWithTextAutoSized(x - pt.x, y - pt.y - GetSize().cy - 4, str);
	}
	//*/
}

void Turtle::CheckCollisionPowerUp()
{
	m_fX += m_fXS;
	m_fY += m_fYS;

	bool bBlockCrushed = false;

	memset(&m_bCollision, 0, sizeof(m_bCollision));
	m_bStandOnJumpBlock = false;

	NaRect rc = GetRect();
	NaRect rcTile = GetTileRect();
	int x, y;

	// Check Top
	for (int i = rcTile.left; i <= rcTile.right; i++)
	{
		x = i;
		y = rcTile.top;
		if ((CUR_STAGE->IsHardTile(x, y) || (m_fYS < 0 && CUR_STAGE->HasItem(x, y))) &&
			!CUR_STAGE->IsHardTile(x, y + 1))
		{
			NaRect rcStageTile = CUR_STAGE->GetTileRect(x, y);
			if (m_bCollision[COLLISION_TOP])
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
		if (y * 16 - m_fY > -5)
			continue;

		NaRect rcStageTile = CUR_STAGE->GetTileRect(x, y);
		if (CUR_STAGE->IsHardTile(x, y) && !CUR_STAGE->IsHardTile(x + 1, y))
		{
			int nData = CUR_STAGE->GetTileData(x, y);
			int nType = CUR_STAGE->GetDataType(nData);
			bool bSkipCollision = false;

			if (nType != TILETYPE_SUPERHARD && CUR_STAGE->CanCrush(x, y, POWER_HITBLOCK_GIANTSHELL))
			{
				CUR_STAGE->CrushBlock(x, y, m_pKickedBy ? m_pKickedBy : this);
				bBlockCrushed = true;

				if (nType == TILETYPE_ITEM)
				{
					// #TODO Change to function 
					int nIdx = CUR_STAGE->GetTileIndex(x, y);
					CUR_STAGE->m_mapHitFrame.insert(
						std::pair<int, int>(nIdx, 15)
					);
				}

				if (nType == TILETYPE_SOFTHARD)
					bSkipCollision = true;
			}
			if (!bSkipCollision && rc.IsOverlapped(rcStageTile))
			{
				m_bCollision[COLLISION_LEFT] = true;
				m_ptCollision[COLLISION_LEFT] = { (float)rcStageTile.right - 1, (float)(y * TILE_YS) };
			}
		}

		x = rcTile.right;
		rcStageTile = CUR_STAGE->GetTileRect(x, y);
		if (CUR_STAGE->IsHardTile(x, y) && !CUR_STAGE->IsHardTile(x - 1, y))
		{
			int nData = CUR_STAGE->GetTileData(x, y);
			int nType = CUR_STAGE->GetDataType(nData);
			bool bSkipCollision = false;

			if (nType != TILETYPE_SUPERHARD && CUR_STAGE->CanCrush(x, y, POWER_HITBLOCK_GIANTSHELL))
			{
				CUR_STAGE->CrushBlock(x, y, m_pKickedBy ? m_pKickedBy : this);
				bBlockCrushed = true;

				if (nType == TILETYPE_ITEM)
				{
					// #TODO Change to function 
					int nIdx = CUR_STAGE->GetTileIndex(x, y);
					CUR_STAGE->m_mapHitFrame.insert(
						std::pair<int, int>(nIdx, 15)
					);
				}

				if (nType == TILETYPE_SOFTHARD)
					bSkipCollision = true;
			}
			if (!bSkipCollision && rc.IsOverlapped(rcStageTile))
			{
				m_bCollision[COLLISION_RIGHT] = true;
				m_ptCollision[COLLISION_RIGHT] = { (float)rcStageTile.left, (float)(y * TILE_YS) };
			}
		}
	}

	// Check Bottom
	for (int i = rcTile.left; i <= rcTile.right; i++)
	{
		x = i;
		y = rcTile.bottom;

		if (m_fYS >= 0 && CUR_STAGE->IsJumpTile(x, y) && !CUR_STAGE->IsHardTile(x, y - 1))
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
		else if(CUR_STAGE->IsHardTile(x, y) && !CUR_STAGE->IsHardTile(x, y - 1))
		{
			NaRect rcStageTile = CUR_STAGE->GetTileRect(x, y);
			if (rc.IsOverlapped(rcStageTile))
			{
				rcStageTile = CUR_STAGE->GetTileRect(x, y);
				m_bCollision[COLLISION_BOTTOM] = true;
				m_ptCollision[COLLISION_BOTTOM] = { (float)(x * TILE_XS), (float)(y * TILE_YS) };
				break;
			}
		}
	}

	{
		auto rcF = GetRectFloat();
		BEGIN_VECTOR_LOOP(GameObjectBase*, CUR_STAGE->m_vecEventObject) {
			if (pObj == this)
				continue;

			EventObjectBase *pEventObj = (EventObjectBase*)pObj;

			auto rcObj = pObj->GetRectFloat();
			if (rcF.IsOverlapped(rcObj))
			{
				// obj is over me
				if ((rcF.top - m_fYS > rcObj.bottom))
				{
					if (pEventObj->m_bCanCollide)
					{
						m_bCollision[COLLISION_TOP] = true;
						m_ptCollision[COLLISION_TOP] = { m_fX, rcObj.bottom };
					}
				}

				// obj is on my left
				if (rcF.left + m_fXS > rcObj.right)
				{
					if (pEventObj->m_bCanCollide)
					{
						m_bCollision[COLLISION_LEFT] = true;
						m_ptCollision[COLLISION_LEFT] = { rcObj.right, m_fY };
					}
				}

				// obj is on my right
				if (rcF.right - m_fXS < rcObj.left)
				{
					if (pEventObj->m_bCanCollide)
					{
						m_bCollision[COLLISION_RIGHT] = true;
						m_ptCollision[COLLISION_RIGHT] = { rcObj.left, m_fY };
					}
				}

				// obj is under me
				if ((m_fY - m_fYS) - 2 <= rcObj.top)
				{
					if (pEventObj->m_bCanCollide)
					{
						m_bCollision[COLLISION_BOTTOM] = true;
						m_ptCollision[COLLISION_BOTTOM] = { m_fX, rcObj.top };
					}

					if (pEventObj->CanLand())
					{
						m_bCollision[COLLISION_BOTTOM] = true;
						m_ptCollision[COLLISION_BOTTOM] = { m_fX, rcObj.top };

						pObj->AddPassenger(this);
					}
				}
			}
		} END_VECTOR_LOOP();
	}

	if (bBlockCrushed)
		SOUND_MANAGER->PlaySoundEffect(L"BreakBlock");

	m_fX -= m_fXS;
	m_fY -= m_fYS;
}

void Turtle::ChangeState(int nState, ChangeStateInfo * pInfo)
{
	switch (nState)
	{
	case STATE_SPITTED:
		{
			m_nShape = TURTLESHAPE_SHELLKICKED;
			m_bDeadInside = true;
			m_nDamageGroup = DAMAGEGROUP_ENEMY;
			if (pInfo && pInfo->pFromObject)
			{
				// #TODO Change 10 to FromObject Size.cx/2
				if (pInfo->pFromObject->m_fX > m_fX)
				{
					m_fX -= 10;
					m_bFlip = true;
				}
				else
				{
					m_fX += 10;
					m_bFlip = false;
				}

				m_pKickedBy = pInfo->pFromObject;
				m_nCombo = 0;
			}

			m_nInvinsibleTime = 10;
			OnShellKicked();

			return;
		}
		break;
	case STATE_SPITTED_SAFE:
		{
			m_nShape = TURTLESHAPE_SHELL;
			m_bDeadInside = true;
			m_nDamageGroup = DAMAGEGROUP_ENEMY;
			nState = STATE_SHELLIDLE;
		}
		break;
	case STATE_PRESSED:
		{
			ReleaseEnemyStack();

			int nPower = POWER_PRESS_NORMAL;
			if (pInfo && pInfo->nPower)
				nPower = pInfo->nPower;

			switch (nPower)
			{
			case POWER_PRESS_YOSHI:
				{
					OnStomped();
					nState = STATE_DEAD;

					SOUND_MANAGER->PlaySoundEffect(L"YoshiStomp");
				}
				break;
			case POWER_PRESS_GOOMBASHOES:
				{
					nState = STATE_BURNT;

					SOUND_MANAGER->PlaySoundEffect(L"Kicked");
				}
				break;
			default:
				{
					if (m_nState == STATE_SHELLIDLE || m_nState == STATE_SHELLAWAKE)
					{
						m_nShape = TURTLESHAPE_SHELLKICKED;
						OnShellKicked();

						SOUND_MANAGER->PlaySoundEffect(L"Kicked");
					}
					else if (m_nState == STATE_SHELLRUN)
					{
						m_nShape = TURTLESHAPE_SHELL;
						m_nAwakeTimer = 16 + 80;
						m_fXS = 0;
						StopCombo();

						SOUND_MANAGER->PlaySoundEffect(L"Pressed");
					}
					else
					{
						if (m_bWinged)
						{
							m_fYS = 0;
							m_bWinged = false;
							nState = STATE_WALK;

							OnLostWing();
						}
						else
						{
							m_nShape = TURTLESHAPE_SHELL;
							m_nAwakeTimer = 16 + 80;
							m_fXS = 0;
						}

						SOUND_MANAGER->PlaySoundEffect(L"Pressed");
					}

					m_nDamageCooltime = 8;
				}
				break;
			}

			if (pInfo && pInfo->pFromObject)
				GiveScoreAndComboToAttacker(pInfo->pFromObject, 100, true);
		}
		break;
	default:
		EnemyObjectBase::ChangeState(nState, pInfo);
		return;
		break;
	}
	
	m_nNextState = nState;
}

void Turtle::OnChangeState(int nState)
{
	ChangeStateInfo *pInfo = nullptr;
	if (m_ChangeStateInfo.nNextState != -1)
		pInfo = &m_ChangeStateInfo;

	switch (nState)
	{
	case STATE_WALK:
		if (m_nShape != TURTLESHAPE_NORMAL)
			m_nShape = TURTLESHAPE_NORMAL;
		if (m_bFlip)
			m_fXS = -0.5f;
		else
			m_fXS = 0.5f;
		break;
	case STATE_JUMP:
		m_fYS = -4.0f;
		break;
	case STATE_TRAMPOLINJUMP:
		m_fYS = -6.0f;

		if (m_nShape == TURTLESHAPE_SHELLKICKED)
			ChangeState(STATE_SHELLRUN);
		else if (m_nShape == TURTLESHAPE_SHELL)
		{
			if (m_fXS != 0)
				ChangeState(STATE_SHELLRUN);
			else
				ChangeState(STATE_SHELLIDLE);
		}
		break;
	case STATE_CARRYBYHAND:
		ResetSpeed();
		m_nShape = TURTLESHAPE_SHELL;
		break;
	case STATE_KICKEDUP:
	case STATE_PLACEDBYHAND:
		m_nShape = TURTLESHAPE_SHELL;
		m_nAwakeTimer = 16 + 80;
		EnemyObjectBase::OnChangeState(nState);
		//EnemyObjectBase::ChangeState(nState, pInfo);
		return;
		break;
	case STATE_KICKED:
		{
			if (m_nShape != TURTLESHAPE_NORMAL)
			{
				m_nShape = TURTLESHAPE_SHELLKICKED;
				if (pInfo && pInfo->pFromObject)
				{
					m_pKickedBy = pInfo->pFromObject;

					// #TODO Change 10 to FromObject Size.cx/2
					if (m_pKickedBy->m_fX > m_fX)
					{
						POINT ptOffset = FindCollidableOffset(COLLISION_LEFT, 10);
						//m_fX += ptOffset.x;
						if (ptOffset.x > -3)
						{
							m_bFlip = false;
							SOUND_MANAGER->PlaySoundEffect(L"HitBlock", this);
						}
						else
							m_bFlip = true;
					}
					else
					{
						POINT ptOffset = FindCollidableOffset(COLLISION_RIGHT, 10);
						//m_fX += ptOffset.x;

						if (ptOffset.x < 3)
						{
							m_bFlip = true;
							SOUND_MANAGER->PlaySoundEffect(L"HitBlock", this);
						}
						else
							m_bFlip = false;
					}
										
					if (m_pKickedBy->m_pController)
					{
						int nScore = m_pKickedBy->CalcScoreFrom(400);
						m_pKickedBy->m_pController->AddScoreWithEffect(nScore, this);
						m_pKickedBy->AddCombo();
						
						m_nCombo = m_pKickedBy->m_nCombo;
					}
				}

				m_nKickCooltime = 10;
				m_nInvinsibleTime = 10;
				OnShellKicked();

				SOUND_MANAGER->PlaySoundEffect(L"Kicked");
				
				// Maybe State Changed
				return;
			}
		}
		break;	
	case STATE_SHELLIDLE:
		m_pKickedBy = nullptr;
		break;
	case STATE_PUSHEDUP:
		m_nShape = TURTLESHAPE_SHELL;
		m_nAwakeTimer = 16 + 80;
		ReleaseEnemyStack();

		if (IS_CLASSIC)
			m_bFlipV = true;

		if (m_bWinged)
		{
			m_bWinged = false;
			OnLostWing();
		}

		m_fYS = -3.0f;
		if (pInfo && pInfo->pFromObject)
		{
			if (pInfo->pFromObject->m_fX < m_fX)
			{
				m_fXS = 1.0f;
				m_bFlip = false;
			}
			else if (pInfo->pFromObject->m_fX > m_fX)
			{
				m_fXS = -1.0f;
				m_bFlip = true;
			}

			if (pInfo->pFromObject)
				GiveScoreAndComboToAttacker(pInfo->pFromObject, 100, true);
		}

		SOUND_MANAGER->PlaySoundEffect(L"Kicked");

		break;
	case STATE_BURNT:
		// Use Default
	default:
		EnemyObjectBase::OnChangeState(nState);
		//EnemyObjectBase::ChangeState(nState, pInfo);
		return;
		break;
	}

	//m_nNextState = nState;
}

void Turtle::OnIdle()
{
    if (m_bIsEmptyShell)
    {
        m_bDeadInside = true;
        m_nBaseState = STATE_SHELLIDLE;
        m_nShape = TURTLESHAPE_SHELL;
        ReleaseWing();

        if (!CheckFalling())
            ChangeState(STATE_SHELLIDLE);
    }
    else
    {
        EnemyObjectBase::OnIdle();
    }
}

void Turtle::OnWalk()
{
	if (m_bWinged)
	{
		ChangeState(STATE_JUMP);
	}
	else
	{
		EnemyObjectBase::OnWalk();
	}
}

void Turtle::OnJump()
{
	if (CheckHitBlock(0))
		return;

	if (!m_bCollision[COLLISION_BOTTOM])
	{
		float fGravity = CUR_STAGE->GetGravity();

		m_fYS += 0.15f;
		if (m_fYS > 4.0f)
			m_fYS = 4.0f;
	}
	else
	{
		if ((int)m_fY <= m_ptCollision[COLLISION_BOTTOM].y)
		{
			m_fYS = 0;
			m_fY = m_ptCollision[COLLISION_BOTTOM].y;

			if (m_fXS != 0)
				ChangeState(STATE_WALK);
			else
				ChangeState(STATE_STAND);
		}
	}

	CheckReachWall();

	if (m_bReachFriend)
		OnReachFriend();
}

void Turtle::OnReachWall(bool bIsLeftWall)
{
	EnemyObjectBase::OnReachWall(bIsLeftWall);

	// NOTE
	// SoftBlock -> Destroyed in CheckCollisionXXX (no Collision)
	// HardBlock -> Hit in this func.
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
			if (m_bPowerUp)
				nPower = POWER_HITBLOCK_GIANTSHELL;

			m_pStage->HitBlock(
				ptTile.x / TILE_XS,
				ptTile.y / TILE_YS,
				nPower,
				this,
				m_pKickedBy ? m_pKickedBy : this);

			if (m_fXS != 0)
			{
				m_fX -= m_fXS;
			}
		}
		break;
	}
}

void Turtle::OnReachFriend()
{
	switch (m_nState)
	{
	case STATE_SHELLRUN:
		break;
	default:
		m_fXS *= -1;
		m_bFlip = !m_bFlip;

		if (m_bCollision[COLLISION_BOTTOM] && !m_bWinged)
			ChangeState(STATE_STAND);
		break;
	}

	m_bReachFriend = false;
}

void Turtle::OnPushedUp()
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
	if (m_fYS > 3)
		m_fYS = 3;

	if (!IS_CLASSIC && m_nStateFrame == 16)
	{
		m_bFlipV = true;
		ChangeState(STATE_SHELLIDLE);
	}

	if (IsOutOfStage())
		ChangeState(STATE_DEAD);
}

void Turtle::OnPressed()
{
	ChangeState(STATE_SHELLIDLE);
}

void Turtle::OnShellIdle()
{
	if (CanKicked())
	{
		std::vector<GameObjectBase*>::iterator it = m_vecOverlappedEnemy.begin();
		for (; it != m_vecOverlappedEnemy.end(); ++it)
		{
			if (!m_bFlip)
				m_fXS = 2.0f;
			else
				m_fXS = -2.0f;

			ChangeState(STATE_SHELLRUN);
			return;
		}
	}

	if (!m_bCollision[COLLISION_BOTTOM])
	{
		OnApplyGravity(1, 4.0f);
	}
	else if (m_fYS != 0)
	{
		if ((int)m_fY <= m_ptCollision[COLLISION_BOTTOM].y)
		{
			m_fYS = 0;
			
			OnLanding();
		}
	}

	NaRect rc = GetRect();
	if (m_bCollision[COLLISION_LEFT])
	{
		m_fX = m_ptCollision[COLLISION_LEFT].x + (rc.Width() / 2) + 2; // !?
	}
	else if (m_bCollision[COLLISION_RIGHT])
	{
		m_fX = m_ptCollision[COLLISION_RIGHT].x - (rc.Width() / 2) - 1;
	}

	if (m_bDeadInside == false && m_nStateFrame > 245)
	{
		ChangeState(STATE_SHELLAWAKE);
	}
}

void Turtle::OnShellRun()
{
	if (m_nNextState != STATE_FALL)
	{
		if (CheckReachWall())
		{
			return;
		}
	}

	std::vector<GameObjectBase*>::iterator it = m_vecOverlappedEnemy.begin();
	for (; it != m_vecOverlappedEnemy.end(); ++it)
	{
		GameObjectBase *pEnemy = *it;
		if (pEnemy->IsAlive() && 
			pEnemy->CanCollide(-1) == false && 
			!pEnemy->IsInvinsible() &&
			pEnemy->CanBurnt(BURN_TURTLESHELL))
		{
			if ((m_fXS > 0 && pEnemy->m_fX > m_fX) ||
				(m_fXS < 0 && pEnemy->m_fX < m_fX))
			{
				if (m_pKickedBy)
				{
					AddCombo();
					if (m_pKickedBy->m_pController)
					{
						int nScore = CalcScoreFrom(100);
						m_pKickedBy->m_pController->AddScoreWithEffect(nScore, pEnemy);
					}
				}
				pEnemy->ChangeState(STATE_BURNT);

				if (pEnemy->IsAlive())
				{
					auto rcEnemy = pEnemy->GetRect();
					bool bIsLeftWall = pEnemy->m_fX < m_fX;
					if (bIsLeftWall)
						m_ptCollision[COLLISION_LEFT].x = rcEnemy.right;
					else
						m_ptCollision[COLLISION_RIGHT].x = rcEnemy.left;
					OnReachWall(pEnemy->m_fX < m_fX);
				}

				NaDebugOut(L"%s Hits %s(0x%08x)\n",
					GetTypeName(), pEnemy->GetTypeName(), pEnemy);
			}
		}
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
	
	if (m_fYS < 0)
	{
		if (m_bCollision[COLLISION_TOP])
		{
			int nPower = GetPower(POWERTYPE_HITBLOCK);
			if (CheckHitBlock(nPower))
				return;
		}
	}

	if (m_bStandOnJumpBlock)
		ChangeState(STATE_BLOCKJUMP);
	
	if (m_bReachFriend)
		OnReachFriend();

	if (IsOutOfStage())
		ChangeState(STATE_DEAD);
}

void Turtle::OnShellAwake()
{
	if (m_nStateFrame == 80)
	{
		if (IS_CLASSIC || m_bFlipV == false)
		{
			ChangeState(STATE_WALK);
		}
		else
		{
			m_fYS = -3.0f;
		}
	}

	if (m_nStateFrame == 88)
	{
		m_bFlipV = false;
		m_nShape = TURTLESHAPE_SHELLAWAKEN;
	}

	if (m_nStateFrame > 80)
	{		
		if (!m_bCollision[COLLISION_BOTTOM])
		{
			OnApplyGravity(1, 4.0f);
		}
		else
		{
			if ((int)m_fY - 4 <= m_ptCollision[COLLISION_BOTTOM].y)
			{
				m_fYS = 0;

				m_bFlipV = false;
				ChangeState(STATE_WALK);
			}
		}
	}
}

void Turtle::OnShellKicked()
{
	if (!m_bFlip)
		m_fXS = 3.3f;
	else
		m_fXS = -3.3f;

	ChangeState(STATE_SHELLRUN);
}

void Turtle::OnStacked()
{
    if (m_bDeadInside || m_bIsEmptyShell)
    {
        if (m_bIsEmptyShell)
        {
            m_bDeadInside = true;
            m_nBaseState = STATE_SHELLIDLE;
            m_nShape = TURTLESHAPE_SHELL;
            ReleaseWing();
        }

        ReleaseEnemyStack();
        ChangeState(STATE_FALL);
    }
    else
    {
        EnemyObjectBase::OnStacked();
    }
}

void Turtle::OnBlockJump()
{
	if (m_nStateFrame < GameDefaults::nBlockJumpFreeze)
	{
		PressJumpBlock();

		POINT ptOffset = CUR_STAGE->GetHitAnimatedTileOffset(
			m_ptCollision[COLLISION_BOTTOM].x / TILE_XS,
			m_ptCollision[COLLISION_BOTTOM].y / TILE_YS
		);

        if (m_nStateFrame < GameDefaults::nBlockJumpFreeze - 1)
		    m_fY = m_ptCollision[COLLISION_BOTTOM].y + ptOffset.y;
	}

    bool bChangeState = false;

    if (m_nStateFrame == GameDefaults::nBlockJumpFreeze)
    {
        m_fYS = -6.0f;
        bChangeState = true;
    }

    if (m_nStateFrame > GameDefaults::nBlockJumpFreeze)
    {
        m_fYS = -(6.0f - ((m_nStateFrame - GameDefaults::nBlockJumpFreeze) * 0.1f));
        bChangeState = true;
    }

    if (bChangeState)
    {
		switch (m_nShape)
		{
		case TURTLESHAPE_SHELLKICKED:
			ChangeState(STATE_SHELLRUN);
			break;
		case TURTLESHAPE_SHELL:
			ChangeState(STATE_SHELLIDLE);
			break;
		case TURTLESHAPE_SHELLAWAKEN:
			{
				m_nShape = TURTLESHAPE_NORMAL;
				m_bFlipV = false;
				ChangeState(STATE_WALK);
			}
			break;
		case TURTLESHAPE_NORMAL:
			ChangeState(STATE_WALK);
			break;
		}
			
	}

	CheckReachWall();
}

void Turtle::OnLanding()
{
	m_fY = m_ptCollision[COLLISION_BOTTOM].y;

	if (m_bStandOnJumpBlock)
		ChangeState(STATE_BLOCKJUMP);
	else
	{
		switch (m_nShape)
		{
		case TURTLESHAPE_SHELLKICKED:
			ChangeState(STATE_SHELLRUN);
			break;
		case TURTLESHAPE_SHELL:
			m_fXS = 0;
			ChangeState(STATE_SHELLIDLE);
			break;
		case TURTLESHAPE_SHELLAWAKEN:
			{
				m_nShape = TURTLESHAPE_NORMAL;
				m_bFlipV = false;
				ChangeState(STATE_WALK);
			}
			break;
		case TURTLESHAPE_NORMAL:
			ChangeState(STATE_WALK);
			break;
		}
	}

}

bool Turtle::CanDamageEnemy()
{
	switch (m_nState)
	{
    case STATE_IDLE:
        return !m_bDeadInside && !m_bIsEmptyShell;
	case STATE_PRESSED:
	case STATE_KICKED:
	case STATE_SHELLIDLE:
	case STATE_SHELLAWAKE:
	case STATE_CARRYBYHAND:
	case STATE_KICKEDUP:
	case STATE_PLACEDBYHAND:
		return false;
	case STATE_BLOCKJUMP:
	case STATE_TRAMPOLINJUMP:
	case STATE_TRAMPOLINPRESS:
	case STATE_FALL:
		if (m_nShape == TURTLESHAPE_SHELL)
			return false;
		else
			return true;
		break;
	case STATE_SHELLRUN:
		if (m_nKickCooltime > 0)
			return false;
		else
			return true;
		break;
	}

	return EnemyObjectBase::CanDamageEnemy();
}

bool Turtle::CanSpit()
{
	return true;
}

bool Turtle::CanCarried()
{
	switch (m_nState)
	{
	case STATE_SHELLIDLE:
	case STATE_KICKEDUP:
	case STATE_PLACEDBYHAND:
		return true;
		break;
	case STATE_FALL:
		if (m_nShape == TURTLESHAPE_SHELL &&
			m_fXS == 0)
			return true;
		break;
	}

	return false;
}

bool Turtle::CanKicked()
{
	bool bBaseRet = EnemyObjectBase::CanKicked();
	if (bBaseRet)
		return true;

	switch (m_nState)
	{
	case STATE_FALL:
		if (m_nShape == TURTLESHAPE_SHELL &&
			m_fXS == 0)
			return true;
		break;
	}

	return false;
}

std::vector<POINT> Turtle::GetWingPosition()
{
	POINT pt;
	SIZE s = GetSize();
	pt.x = -1;
	pt.y = -10;

	std::vector<POINT> vec;
	vec.push_back(pt);

	return vec;
}

int Turtle::GetSpriteIndex()
{
	if (EnemyObjectBase::GetSpriteIndex() == -1)
		return -1;

	int nFrame = m_nSpriteIndex;
	switch (m_nState)
	{
	case STATE_SHELLAWAKE:
		if (IS_CLASSIC)
			nFrame = SPRIDX_TURTLE_SHELL1 + (m_nStateFrame / 8) % 2;
		else
		{
			nFrame = SPRIDX_TURTLE_SPIN1;
		}
		break;
	case STATE_PRESSED:
	case STATE_SHELLIDLE:
	case STATE_CARRYBYHAND:
	case STATE_KICKEDUP:
	case STATE_PLACEDBYHAND:
		if (IS_MODERN)
		{
			if (m_bDeadInside)
				nFrame = SPRIDX_TURTLE_SHELL1;
			else
				nFrame = SPRIDX_TURTLE_SPIN1;
		}
		else
			nFrame = SPRIDX_TURTLE_SHELL1;
		break;
	case STATE_SHELLRUN:
		if (m_pGame->m_bModernStyle)
		{
			nFrame = SPRITE_INDEX_CYCLE(SPRIDX_TURTLE_SPIN1, 4, 3);
		}
		else
			nFrame = SPRIDX_TURTLE_SHELL1;
		break;
	case STATE_PUSHEDUP:
	case STATE_BURNT:
		nFrame = SPRIDX_TURTLE_SHELL1;
		break;
	case STATE_HELDBYTONGUE:
		if (nFrame == 0)
			nFrame = SPRIDX_TURTLE_SHELL1;
		break;
	default:
		nFrame = SPRIDX_TURTLE1 + (m_nStateFrame / 8) % 2;
        if (m_nShape < TURTLESHAPE_NORMAL || m_bDeadInside || m_bIsEmptyShell)
        {
            if (m_bDeadInside || m_bIsEmptyShell)
                nFrame = SPRIDX_TURTLE_SPIN1;
            else
                nFrame = SPRIDX_TURTLE_SHELL1;
        }
		
		break;
	}

	m_nSpriteIndex = nFrame;

	return nFrame;
}

SIZE Turtle::GetSize()
{
	SIZE s = EnemyObjectBase::GetSize();
	if (m_nState == STATE_SHELLRUN)
	{
		s.cx -= 2;
	}

	return s;
}
