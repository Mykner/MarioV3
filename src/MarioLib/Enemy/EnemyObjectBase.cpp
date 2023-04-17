#include "EnemyObjectBase.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "ThemeManager.h"
#include "SoundManager.h"
#include "VehicleObjectBase.h"

#include "Goomba.h"
#include "Turtle.h"
#include "RedTurtle.h"
#include "PiranhaPlant.h"
#include "FireBar.h"
#include "Podoboo.h"
#include "Bowser.h"
#include "Bloober.h"
#include "CheepCheep.h"
#include "RedCheepCheep.h"
#include "CheepCheepFly.h"
#include "RedCheepCheepFly.h"
#include "HammerBrothers.h"
#include "Lakitu.h"
#include "Spiny.h"
#include "BuzzyBeetle.h"
#include "BulletBill.h"
#include "ChainChomp.h"
#include "Bobomb.h"
#include "SledgeBrothers.h"
#include "Thwomp.h"
#include "DryBone.h"
#include "Grinder.h"
#include "ChainChompStake.h"
#include "BoomerangBrothers.h"
#include "FireBrothers.h"
//#include "ChargingChuck.h"
#include "BowserJunior.h"
#include "Boo.h"
//#include "DryFish.h"
//#include "MontyMale.h"
#include "RockyWrench.h"
#include "CannonBall.h"
//#include "BlooberBaby.h"
//#include "Wiggler.h"
//#include "Anger.h"
//#include "MagiKoopa.h"
#include "BoomBoom.h"

// Deprecated
//#include "RedTurtleWingedPatrol.h"
//#include "TurtleWinged.h"

EnemyObjectBase::EnemyObjectBase(Game *pGame, Stage *pStage) :
	GameObjectBase(pGame, pStage)
{
	m_nType = ENEMY_NONE;
	m_nState = STATE_IDLE;
	
	RequestSprites(pStage);

	m_bFlip = true;

	m_nBaseState = STATE_WALK;

	m_pOnMyHead = nullptr;
	m_pUnderMyFoot = nullptr;
}

EnemyObjectBase::~EnemyObjectBase()
{
    SAFE_DELETE(m_pController);
}

void EnemyObjectBase::Process()
{
	ProcessBase();

	if (m_pVehicle)
		m_pVehicle->Process();
}

void EnemyObjectBase::Render(int nColor, int nZOrder)
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

		float fScaleX, fScaleY;
		GetScale(fScaleX, fScaleY);

        float _x = x - pt.x;
        float _y = y - pt.y;

        if (m_nState == STATE_STACKED)
            _x += GetStackXOffset();

		(*m_ppSprite)->RenderToQueue(_x, _y, nFrame, m_bFlip, m_bFlipV, nColor,
			fScaleX, fScaleY, nZOrder, bShadow, fAngle);
		
		if (m_bWinged)
			RenderWing(nColor, nZOrder);
	}

	if (m_pVehicle)
		m_pVehicle->Render();
}

void EnemyObjectBase::ChangeState(int nState, ChangeStateInfo * pInfo)
{
	switch (nState)
	{
	case STATE_PRESSED:
		{
			ReleaseEnemyStack();

			ReleaseWing();

			m_bFlipV = true;
			m_fXS = 0;
			m_fYS = -1.0;
			if (pInfo->pFromObject)
				GiveScoreAndComboToAttacker(pInfo->pFromObject, 100, true);

			nState = STATE_BURNT;

			int nPower = POWER_PRESS_NORMAL;
			if (pInfo && pInfo->nPower)
				nPower = pInfo->nPower;

			ReleaseVehicle();

			switch (nPower)
			{
			case POWER_PRESS_YOSHI:
				{
					OnStomped();
					nState = STATE_DEAD;

					SOUND_MANAGER->PlaySoundEffect(L"YoshiStomp");
				}
				break;
			default:
				SOUND_MANAGER->PlaySoundEffect(L"Pressed");
				break;
			}
		}
		break;
	case STATE_BURNT:
		{
			ReleaseEnemyStack();

			ReleaseWing();

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

				if (pInfo->pFromObject)
					GiveScoreAndComboToAttacker(pInfo->pFromObject, 100, true);
			}

			ReleaseVehicle();

			SOUND_MANAGER->PlaySoundEffect(L"Kicked");
		}
		break;
	case STATE_POWED:
		{
			ReleaseEnemyStack();

			ReleaseWing();

			ReleaseVehicle();

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

				if (pInfo->pFromObject)
					GiveScoreAndComboToAttacker(pInfo->pFromObject, 100, true);
			}

			SOUND_MANAGER->PlaySoundEffect(L"Kicked");
		}
		break;
	case STATE_DEAD:
		ReleaseVehicle();
		break;
	default:
		GameObjectBase::ChangeState(nState, pInfo);
		return;
		break;
	}

	m_nNextState = nState;
}

void EnemyObjectBase::OnChangeState(int nState)
{
	ChangeStateInfo *pInfo = nullptr;
	if (m_ChangeStateInfo.nNextState != -1)
		pInfo = &m_ChangeStateInfo;

	switch (nState)
	{
	case STATE_WALK:
		if (m_bFlip)
			m_fXS = -0.5f;
		else
			m_fXS = 0.5f;
		break;
	case STATE_TRAMPOLINJUMP:
		// Get Speed Bonus
		if (m_fXS == 0)
		{
			//GameObjectBase::ChangeState(nState, pInfo);
			GameObjectBase::OnChangeState(nState);

			// Change to Default Stage
			m_nNextState = -1;
			ChangeState(m_nBaseState);
		}
		else
		{
			GameObjectBase::OnChangeState(nState);
			//GameObjectBase::ChangeState(nState, pInfo);
		}
		return;
		break;
	case STATE_PUSHEDUP:
		{
			ReleaseEnemyStack();

			if (m_bWinged)
			{
				m_bWinged = false;
				OnLostWing();
			}

			m_bFlipV = true;
			m_fYS = -3.0f;
			if (pInfo && pInfo->pFromObject)
			{
				if (pInfo->pFromObject->m_fXS != 0)
					m_fXS = pInfo->pFromObject->m_fXS / abs(pInfo->pFromObject->m_fXS);

				if (pInfo->pFromObject)
					GiveScoreAndComboToAttacker(pInfo->pFromObject, 100, true);
			}

			SOUND_MANAGER->PlaySoundEffect(L"Kicked");
		}
		break;	
	case STATE_DEAD:
		{
			if (m_bWinged)
			{
				m_bWinged = false;
				OnLostWing();
			}

			ReleaseEnemyStack();
		}
		break;
	case STATE_HELDBYTONGUE:
		ReleaseEnemyStack();

		if (m_bWinged)
		{
			m_bWinged = false;
			OnLostWing();
		}
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
	default:
		GameObjectBase::OnChangeState(nState);
		break;
	}
}

void EnemyObjectBase::OnBeforeProcess()
{
	GameObjectBase::OnBeforeProcess();

	if (m_pVehicle)
		m_pVehicle->OnBeforeProcess();

#pragma message("# VehicleRideTest - Hardcoded #")
	switch (m_nType)
	{
	case ENEMY_GOOMBA:
	case ENEMY_PIRANHA_PLANT:
	case ENEMY_FIREPIRANHAPLANT:
	case ENEMY_MUNCHER:
	case ENEMY_HAMMERBROTHERS:
	case ENEMY_BOOMERANGBROTHERS:
	case ENEMY_FIREBROTHERS:
	case ENEMY_SLEDGEBROTHERS:
	case ENEMY_BOWSER:
		CheckCollisionVehicle();
		break;
	default:
		break;
	}
}

void EnemyObjectBase::OnAfterProcess()
{
	m_fX += m_fXS;
	m_fY += m_fYS;

	if (m_nInvinsibleTime > 0)
		m_nInvinsibleTime--;

	if (m_nDamageCooltime > 0)
		m_nDamageCooltime--;

	if (m_nKickCooltime > 0)
		m_nKickCooltime--;

	// #TODO Move with ..
	if (m_pOnMyHead)
	{
		m_pOnMyHead->m_fX += m_fXS;
		m_pOnMyHead->m_fY += m_fYS;
	}

    ProcessState();

	if (m_pVehicle)
		m_pVehicle->OnAfterProcess();
}

void EnemyObjectBase::OnIdle()
{
	if (!CheckFalling())
		ChangeState(STATE_WALK);
}

void EnemyObjectBase::OnStand()
{
	ChangeState(STATE_WALK);
}

void EnemyObjectBase::OnWalk()
{
	GameObjectBase::OnWalk();
	
	if (IsOutOfStage())
		ChangeState(STATE_DEAD);
}

void EnemyObjectBase::OnFall()
{
	GameObjectBase::OnFall();

	if (m_fYS >= 0)
	{
		if (CheckRideVehicle())
		{
			ChangeState(STATE_RIDE);
			return;
		}
	}

	if (IsOutOfStage())
		ChangeState(STATE_DEAD);
}

void EnemyObjectBase::OnReachWall(bool bIsLeftWall)
{
	GameObjectBase::OnReachWall(bIsLeftWall);

	m_fXS *= -1;
	m_bFlip = !m_bFlip;
}

void EnemyObjectBase::OnReachFriend()
{
	m_fXS *= -1;
	m_bFlip = !m_bFlip;
	m_bReachFriend = false;

	ChangeState(STATE_STAND);
}

void EnemyObjectBase::OnItemPop()
{
	if (m_nStateFrame == 0)
	{
		SOUND_MANAGER->PlaySoundEffect(L"ItemPop");
	}

	if (m_nStateFrame == 30)
		m_fYS = -1.6f;

	if (m_nStateFrame == 42)
		OnAfterItemPop(m_nBaseState);
}

void EnemyObjectBase::OnPushedUp()
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

	if (IsOutOfStage())
		ChangeState(STATE_DEAD);
}

void EnemyObjectBase::OnPressed()
{
	if (m_nStateFrame > 40)
		ChangeState(STATE_DEAD);
}

void EnemyObjectBase::OnBurnt()
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

void EnemyObjectBase::OnStacked()
{
	if (m_pUnderMyFoot)
	{
		if (!m_pUnderMyFoot->IsAlive())
		{
			// Buggy condition
			m_pUnderMyFoot->ReleaseEnemyStack();
			return;
		}

		NaRect rc = GetRect();

		if (m_bCollision[COLLISION_LEFT])
		{
			if (m_ptCollision[COLLISION_LEFT].x - rc.left > 2)
			{
				ReleaseFootStack();

				m_fX = m_ptCollision[COLLISION_LEFT].x + (rc.Width() / 2);
				ChangeState(STATE_IDLE);
				return;
			}
		}

		if (m_bCollision[COLLISION_RIGHT])
		{
			if (rc.right - m_ptCollision[COLLISION_RIGHT].x > 2)
			{
				ReleaseFootStack();

				m_fX = m_ptCollision[COLLISION_RIGHT].x - (rc.Width() / 2);
				ChangeState(STATE_IDLE);
				return;
			}
		}

		NaRect rcStack = m_pUnderMyFoot->GetRect();
		m_fX = m_pUnderMyFoot->m_fX - m_pUnderMyFoot->m_fXS;
		m_fY = rcStack.top - 1;

		m_bFlip = m_pUnderMyFoot->m_bFlip;
	}
}

void EnemyObjectBase::OnBlockJump()
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
		ChangeState(m_nBaseState);
	}
}

void EnemyObjectBase::OnCarryByHand()
{
	std::vector<GameObjectBase*>::iterator it = m_vecOverlappedEnemy.begin();
	for (; it != m_vecOverlappedEnemy.end(); ++it)
	{
		GameObjectBase *pEnemy = *it;
		if (pEnemy->IsAlive() && pEnemy->CanCollide(-1) == false)
		{
			ChangeStateInfo info;
			info.pFromObject = this;
			info.nPower = BURN_STARMAN;
			pEnemy->ChangeState(STATE_BURNT, &info);

			info.pFromObject = nullptr;

			// #TODO Bonus score to carrier
			if (CUR_PLAYER_OBJ->m_pCarry == this)
			{
				info.pFromObject = CUR_PLAYER_OBJ;
				
				if (CUR_PLAYER_OBJ->m_pController)
				{
					if (CUR_PLAYER_OBJ->m_pController)
						CUR_PLAYER_OBJ->m_pController->AddScoreWithEffect(100, this);

					// #TODO Change to below code?
					/*
					if (pInfo->pFromObject)
						GiveScoreAndComboToAttacker(pInfo->pFromObject, 100, true);
						*/
				}
			}

			NaDebugOut(L"%s(carried) Hits %s(0x%08x)(%s)\n",
				GetTypeName(), pEnemy->GetTypeName(), pEnemy, pEnemy->GetStateName());
			
			info.pFromObject = pEnemy;
			ChangeState(STATE_BURNT, &info);
			break;
		}
	}
}

void EnemyObjectBase::OnRide()
{
	if (m_pVehicle)
	{
		switch (m_pVehicle->m_nType)
		{
		case VEHICLE_CLOWNCAR:
		case VEHICLE_GOOMBASHOE:
			// #TODO #ASAP must delete this
			if (m_pController == nullptr)
				m_pController = new ControllerBase(m_pGame);

			// #TODO #ASAP must reset this too.
			m_pVehicle->m_pController = m_pController;
			m_pVehicle->OnControlBy(this);
			break;
		}
	}
}

void EnemyObjectBase::OnPowerUp()
{
	if (m_bPowerUp)
	{
		m_fScaleX = m_fScaleY = 2.0f;
	}
	else
	{
		m_fScaleX = m_fScaleY = 1.0f;
	}
}

void EnemyObjectBase::GiveScoreAndComboToAttacker(GameObjectBase * pAttacker, int nScoreFrom, bool bAddCombo)
{
	if (pAttacker)
	{
		if (pAttacker->m_pController)
		{
			int nScore = pAttacker->CalcScoreFrom(nScoreFrom);
			pAttacker->m_pController->AddScoreWithEffect(nScore, this);
		}

		if (bAddCombo)
		{
			pAttacker->AddCombo();
		}
	}
}

void EnemyObjectBase::ReleaseEnemyStack()
{
	ReleaseHeadStack();
	ReleaseFootStack();
}

void EnemyObjectBase::ReleaseHeadStack()
{
	// Check on my head
	if (m_pOnMyHead)
	{
		((EnemyObjectBase*)m_pOnMyHead)->m_pUnderMyFoot = nullptr;
		if (m_pOnMyHead->IsAlive())
			m_pOnMyHead->ChangeState(STATE_IDLE);
		m_pOnMyHead = nullptr;
	}
}

void EnemyObjectBase::ReleaseFootStack()
{
	// Check under my foot
	if (m_pUnderMyFoot)
	{
		((EnemyObjectBase*)m_pUnderMyFoot)->m_pOnMyHead = nullptr;
		m_pUnderMyFoot = nullptr;
	}
}

void EnemyObjectBase::ReleaseVehicle()
{
	if (m_pVehicle)
		m_pVehicle->OnDamagedPassenger();
}

bool EnemyObjectBase::IsAlive()
{
	switch (m_nState)
	{
	case STATE_PUSHEDUP:
	case STATE_PRESSED:
	case STATE_BURNT:
	case STATE_DEAD:
	case STATE_POWED:
		return false;
		break;
	}

	switch (m_nNextState)
	{
	case STATE_PUSHEDUP:
	case STATE_PRESSED:
	case STATE_BURNT:
	case STATE_DEAD:
		return false;
		break;
	}

	return true;
}

bool EnemyObjectBase::CanKicked()
{
	switch (m_nState)
	{
	case STATE_SHELLIDLE:
	case STATE_SHELLAWAKE:
		return true;
		break;
	case STATE_KICKEDUP:
	case STATE_PLACEDBYHAND:
		if (m_nStateFrame > 16)
			return true;
		break;
	}

	return false;
}

bool EnemyObjectBase::CanEat()
{
	return true;
}

bool EnemyObjectBase::CanSpit()
{
	return false;
}

bool EnemyObjectBase::CanDamageFriend()
{
	switch (m_nState)
	{
	case STATE_SHELLRUN:
	case STATE_CARRYBYHAND:
	case STATE_KICKED:
	case STATE_KICKEDUP:
		return true;
	}
	return false;
}

bool EnemyObjectBase::CanDamageEnemy()
{
	switch (m_nState)
	{
	case STATE_PRESSED:
	case STATE_KICKED:
	case STATE_SHELLIDLE:
	case STATE_SHELLAWAKE:
	case STATE_HELDBYTONGUE:
	case STATE_ITEMPOP:
	case STATE_ITEMPOPDOWN:
	case STATE_POWED:
		return false;
		break;
	}

	return true;
}

bool EnemyObjectBase::CanBornFromBlock()
{
	return true;
}

int EnemyObjectBase::GetSpriteIndex()
{
	if ((m_nState == STATE_ITEMPOP || m_nState == STATE_ITEMPOPDOWN) &&
		m_nStateFrame < 30)
		return -1;

	return 0;
}

void EnemyObjectBase::GetScale(float & fScaleX, float & fScaleY)
{
	fScaleX = m_fScaleX;
	fScaleY = m_fScaleY;

	if (m_nState == STATE_ITEMPOP && m_nStateFrame < 40)
	{
		float fFactorX = 1.0f, fFactorY = 1.0f;
		SIZE s = GetSpriteSize();
		if (s.cx > 16)
			fFactorX = (16.0f / (float)s.cx);

		if (s.cy > 16)
			fFactorY = (16.0f / (float)s.cy);

		float fFactor = fFactorX < fFactorY ? fFactorX : fFactorY;
		if (m_nStateFrame >= 36 && m_nStateFrame < 40)
		{
			// factor = 0.5

			// 0.5 === 1
			// 0.6 === f * 6/5
			// 0.7 === f * 7/5
			// ..
			// 1
			int nFrame = m_nStateFrame - 35;
			int nMaxFrame = 4;
			fFactor = fFactor * ((float)(nFrame + nMaxFrame) / (float)nMaxFrame);
			if (fFactor > 1)
				fFactor = 1.0f;
		}
		
		fScaleX *= fFactor;
		fScaleY *= fFactor;
	}
}

int EnemyObjectBase::GetZOrder()
{
	if (m_nState == STATE_THROWN && m_nStateFrame < 16)
		return Z_BEHIND_MAP;

	if (m_nState == STATE_THROWNFLY && m_nStateFrame < 16)
		return Z_BEHIND_MAP;
	
	if (m_nState == STATE_ITEMPOP || m_nState == STATE_ITEMPOPDOWN)
		return Z_BEHIND_MAP;

	int nZOrder = GameObjectBase::GetZOrder();
	if (nZOrder != 0)
		return nZOrder;

	return Z_ENEMY;
}

SIZE EnemyObjectBase::GetSize()
{
	SIZE s;
	s.cx = 13 * m_fScaleX;
	s.cy = 15 * m_fScaleY;
	return s;
}

SIZE EnemyObjectBase::GetSpriteSize()
{
	SIZE s = { 16, 16 };
	int nIndex = GetSpriteIndex();
	if (nIndex != -1 && (*m_ppSprite) != nullptr)
	{
		RECT rc = (*m_ppSprite)->m_vecFrames[nIndex].m_rc;
		s.cx = (rc.right - rc.left) * m_fScaleX;
		s.cy = (rc.bottom - rc.top) * m_fScaleY;
	}

	return s;
}

int EnemyObjectBase::GetMaskedType()
{
	int nType = m_nType;
	if (m_bPowerUp)
		nType |= ENEMY_MASK_POWERUP;
	if (m_bWinged)
		nType |= ENEMY_MASK_WINGED;
	
	return nType;
}

float EnemyObjectBase::GetStackXOffset()
{
    int nDownCount = GetStackUnderMeCount();
    EnemyObjectBase *pEnemyBeforeLast = GetStackEnemyBeforeLast();

    if (pEnemyBeforeLast == nullptr)
        return 0;

    return sin((pEnemyBeforeLast->m_nStateFrame / 10.0) + (nDownCount * 0.7f)) * 3.0f;
}

int EnemyObjectBase::GetStackUnderMeCount()
{
    int nCount = 0;
    EnemyObjectBase *pObj = this;

    while (pObj != nullptr && pObj->m_pUnderMyFoot != nullptr)
    {
        nCount += 1;
        pObj = pObj->m_pUnderMyFoot;
    }

    return nCount;
}

EnemyObjectBase* EnemyObjectBase::GetStackEnemyBeforeLast()
{
    EnemyObjectBase *pObj = this;

    while (pObj != nullptr)
    {
        if (pObj->GetStackUnderMeCount() == 1)
            return pObj;

        pObj = pObj->m_pUnderMyFoot;
    }

    return nullptr;
}

GameObjectBase * EnemyObjectBase::CreateInstance(Game * pGame, Stage * pStage, int nType)
{
	GameObjectBase *pObj = nullptr;
	switch (nType)
	{
	case ENEMY_GOOMBA:					pObj = new Goomba(pGame, pStage);				break;
	case ENEMY_TURTLE:					pObj = new Turtle(pGame, pStage);				break;
	case ENEMY_REDTURTLE:				pObj = new RedTurtle(pGame, pStage);			break;

	case ENEMY_PIRANHA_PLANT:
	case ENEMY_MUNCHER:
	case ENEMY_FIREPIRANHAPLANT:
		pObj = new PiranhaPlant(pGame, pStage);
		break;

	case ENEMY_FIREBAR:					pObj = new FireBar(pGame, pStage);				break;
	case ENEMY_PODOBOO:					pObj = new Podoboo(pGame, pStage);				break;
	case ENEMY_BOWSER:					pObj = new Bowser(pGame, pStage);				break;
	case ENEMY_BLOOBER:					pObj = new Bloober(pGame, pStage);				break;
	case ENEMY_CHEEPCHEEP:				pObj = new CheepCheep(pGame, pStage);			break;
	case ENEMY_REDCHEEPCHEEP:			pObj = new RedCheepCheep(pGame, pStage);		break;
	case ENEMY_CHEEPCHEEP_FLY:			pObj = new CheepCheepFly(pGame, pStage);		break;
	case ENEMY_REDCHEEPCHEEP_FLY:		pObj = new RedCheepCheepFly(pGame, pStage);		break;
	case ENEMY_HAMMERBROTHERS:			pObj = new HammerBrothers(pGame, pStage);		break;
	case ENEMY_LAKITU:					pObj = new Lakitu(pGame, pStage);				break;
	case ENEMY_SPINY:					pObj = new Spiny(pGame, pStage);				break;
	case ENEMY_BUZZYBEETLE:				pObj = new BuzzyBeetle(pGame, pStage);			break;
	case ENEMY_BULLETBILL:				pObj = new BulletBill(pGame, pStage);			break;

		// Some deprecated object
	case ENEMY_TURTLE_WINGED:
		pObj = new Turtle(pGame, pStage);
		((EnemyObjectBase*)pObj)->m_bWinged = true;
		break;
	case ENEMY_REDTURTLE_WINGED_PATROL: 
		pObj = new RedTurtle(pGame, pStage); 
		((EnemyObjectBase*)pObj)->m_bWinged = true;
		break;

	case ENEMY_CHAINCHOMP:				pObj = new ChainChomp(pGame, pStage);			break;
	case ENEMY_BOBOMB:					pObj = new Bobomb(pGame, pStage);				break;
	case ENEMY_SLEDGEBROTHERS:			pObj = new SledgeBrothers(pGame, pStage);		break;
	case ENEMY_THWOMP:					pObj = new Thwomp(pGame, pStage);				break;
	case ENEMY_DRYBONE:					pObj = new DryBone(pGame, pStage);				break;
	case ENEMY_GRINDER:					pObj = new Grinder(pGame, pStage);				break;
	case ENEMY_CHAINCHOMPSTAKE:			pObj = new ChainChompStake(pGame, pStage);		break;

		// New 2016.12.12
	case ENEMY_BOOMERANGBROTHERS:		pObj = new BoomerangBrothers(pGame, pStage);	break;
	case ENEMY_FIREBROTHERS:			pObj = new FireBrothers(pGame, pStage);			break;
	case ENEMY_BOWSERJUNIOR:			pObj = new BowserJunior(pGame, pStage);			break;

	case ENEMY_BOO:						pObj = new Boo(pGame, pStage);					break;
	case ENEMY_ROCKYWRENCH:				pObj = new RockyWrench(pGame, pStage);			break;
	case ENEMY_CANNONBALL:				pObj = new CannonBall(pGame, pStage);			break;
		/*
	case ENEMY_CHARGINGCHUCK:
	case ENEMY_DRYFISH:
	case ENEMY_MONTYMALE:
	case ENEMY_BLOOBERBABY:
	case ENEMY_WIGGLER:
	case ENEMY_ANGER:
	case ENEMY_MAGIKOOPA:
	*/
	case ENEMY_BOOMBOOM:				pObj = new BoomBoom(pGame, pStage);				break;
	}

	return pObj;
}
