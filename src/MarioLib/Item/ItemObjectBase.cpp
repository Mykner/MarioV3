#include "ItemObjectBase.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "Stage.h"
#include "FModManager.h"
#include "ThemeManager.h"
#include "SoundManager.h"

#include "Mushroom.h"
#include "Mushroom1UP.h"
#include "FireFlower.h"
#include "StaticCoin.h"
#include "SuperStar.h"
#include "Vine.h"
#include "GiantMushroom.h"
#include "ThrownCoin.h"
#include "DoorKeyCoin.h"
#include "DoorKey.h"
#include "TurtleShell.h"
#include "MysteryMushroom.h"
#include "VehicleObjectBase.h"

// Will be deprecated
#include "YoshiEgg.h"
#include "CloudItem.h"

ItemObjectBase::ItemObjectBase(Game *pGame, Stage *pStage) :
	GameObjectBase(pGame, pStage)
{
	m_nType = ITEM_NONE;
	m_nState = STATE_ITEMPOP;
	
	RequestSprites(pStage);	

	m_nBaseState = STATE_WALK;
}

ItemObjectBase::~ItemObjectBase()
{
}

void ItemObjectBase::CheckCollisionPlayer()
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
	else if (CUR_PLAYER_OBJ->m_pVehicle)
	{
		NaRect rcMe = GetRect();
		NaRect rcPlayer = CUR_PLAYER_OBJ->m_pVehicle->GetRect();
		if (rcMe.IsOverlapped(rcPlayer))
		{
			m_bCollisionPlayer = true;
		}

	}
}

void ItemObjectBase::Process()
{
	switch (m_nState)
	{
	case STATE_WALK:
		if (m_nStateFrame == 0)
		{
			if (!m_bFlip)
				m_fXS = 1.0f;
			else
				m_fXS = -1.0f;
		}

		OnWalk();
		break;
	case STATE_ITEMPOP:
		if (m_nStateFrame == 0)
		{
			SOUND_MANAGER->PlaySoundEffect(L"ItemPop");
		}

		OnItemPop();
		break;
	case STATE_ITEMPOPDOWN:
		if (m_nStateFrame == 0)
		{
			SOUND_MANAGER->PlaySoundEffect(L"ItemPop");
		}

		OnItemPopDown();
		break;
	default:
		ProcessBase();
		break;
	}

	if (m_bCollisionPlayer && CanApplied())
	{
		CUR_PLAYER_OBJ->OnApplyItem(this);
		SetState(STATE_DEAD);
	}
}

void ItemObjectBase::OnBeforeProcess()
{
	CheckCollision();
	CheckCollisionPlayer();
}

void ItemObjectBase::ChangeState(int nState, ChangeStateInfo *pInfo)
{
	switch (nState)
	{
	case STATE_STAND:
		break;
	case STATE_TRAMPOLINJUMP:
		// Get Speed Bonus
		GameObjectBase::ChangeState(nState, pInfo);
		m_nNextState = -1;

		// Change to Default State
		m_fYS = -6.0f;
		ChangeState(m_nBaseState);
		return;
		break;
	default:
		GameObjectBase::ChangeState(nState, pInfo);
		return;
		break;
	}

	m_nNextState = nState;
}

void ItemObjectBase::OnChangeState(int nState)
{
	ChangeStateInfo *pInfo = nullptr;
	if (m_ChangeStateInfo.nNextState != -1)
		pInfo = &m_ChangeStateInfo;

	switch (nState)
	{
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

void ItemObjectBase::Render(int nColor, int nZOrder)
{
	int nFrame = GetSpriteIndex();

	if (nFrame >= 0)
	{
		if (nZOrder == -1)
			nZOrder = GetZOrder();
		
		bool bShadow = (m_pGame->m_bModernStyle);
		float fScaleX, fScaleY;
		GetScale(fScaleX, fScaleY);

		NaPointT<float> pt = CAMERA->GetPosition();

		(*m_ppSprite)->RenderToQueue(m_fX - pt.x, m_fY - pt.y, nFrame, 
			false, m_bFlipV, nColor, fScaleX, fScaleY, nZOrder, bShadow);
	}
}

void ItemObjectBase::OnJump()
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

	if (!m_bCollision[COLLISION_BOTTOM])
	{
		OnApplyGravity(0.5f, 4.0f);
	}
	else
	{
		if ((m_fYS > 0 && m_fY - m_fYS - 1 <= m_ptCollision[COLLISION_BOTTOM].y) ||
			m_fY <= m_ptCollision[COLLISION_BOTTOM].y)
		{
			m_fYS = 0;
			
			OnLanding();
		}
	}

	CheckReachWall();
}

void ItemObjectBase::OnItemPop()
{
	if (m_nStateFrame == 30)
		m_fYS = -0.5f;

	if (m_nStateFrame == 62)
		OnAfterItemPop(m_nBaseState, STATE_FALL);
}

void ItemObjectBase::OnItemPopDown()
{
	if (m_nStateFrame == 4)
		m_fYS = 2.0f;

	if (m_nStateFrame == 4 + 4)
		OnAfterItemPop(m_nBaseState, STATE_FALL);
}

void ItemObjectBase::OnBlockJump()
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

void ItemObjectBase::OnLanding()
{
	m_fY = m_ptCollision[COLLISION_BOTTOM].y;

	if (m_bStandOnJumpBlock)
		ChangeState(STATE_BLOCKJUMP);
	else 
		ChangeState(m_nBaseState);
}

bool ItemObjectBase::CanApplied()
{
	if (m_nState == STATE_ITEMPOP && m_nStateFrame < 30)
		return false;

	if (m_nState == STATE_ITEMPOPDOWN && m_nStateFrame < 4)
		return false;

	if (m_nState == STATE_DEAD || m_nNextState == STATE_DEAD)
		return false;

	return true;
}

bool ItemObjectBase::CanEat()
{
	return true;
}

bool ItemObjectBase::CanBornFromBlock()
{
	return true;
}

void ItemObjectBase::OnReachWall(bool bIsLeftWall)
{
	GameObjectBase::OnReachWall(bIsLeftWall);

	m_fXS *= -1;
	m_bFlip = !m_bFlip;
}

int ItemObjectBase::GetSpriteIndex()
{
	if (m_nState == STATE_ITEMPOP && m_nStateFrame < 30)
		return -1;

	if (m_nState == STATE_ITEMPOPDOWN && m_nStateFrame < 30)
		return -1;

	return -1;
}

NaRect ItemObjectBase::GetRect()
{
	NaRect rc;
	SIZE s = GetSize();
	rc.left = m_fX - s.cx / 2;
	rc.right = m_fX + s.cx / 2 - 1;
	rc.top = m_fY - s.cy + 1;
	rc.bottom = m_fY;

	if (m_nState == STATE_ITEMPOP)
	{
		if (m_nStateFrame < 30)
			rc.bottom = rc.top;
		else if (m_nStateFrame < 62)
			rc.bottom = rc.top + (-m_fYS * (m_nStateFrame - 30));
	}
	else if (m_nState == STATE_ITEMPOPDOWN)
	{
		if (m_nStateFrame < 30)
			rc.top = rc.bottom - 1;
		else if (m_nStateFrame < 62)
			rc.top = rc.bottom - (m_fYS * (m_nStateFrame - 4));
	}

	return rc;
}

SIZE ItemObjectBase::GetSize()
{
	SIZE s;
	s.cx = 13 * m_fScaleX;
	s.cy = 16 * m_fScaleY;
	return s;
}

int ItemObjectBase::GetZOrder()
{
	int nZOrder = GameObjectBase::GetZOrder();
	if (nZOrder != 0)
		return nZOrder;

	nZOrder = Z_ITEM;

	if (m_nState == STATE_ITEMPOP || m_nState == STATE_ITEMPOPDOWN)
		nZOrder = Z_BEHIND_MAP;
	else if (m_nState == STATE_THROWN && m_nStateFrame < 10)
		nZOrder = Z_BEHIND_MAP;
	else if (m_nState == STATE_IDLE)
		nZOrder = Z_COMMON_OBJ;

	if (m_nState == STATE_THROWNFLY && m_nStateFrame < 16)
		return Z_BEHIND_MAP;

	return nZOrder;
}

GameObjectBase * ItemObjectBase::CreateInstance(Game * pGame, Stage * pStage, int nType)
{
	GameObjectBase *pObj = nullptr;
	switch (nType)
	{
	case ITEM_COIN:						pObj = new StaticCoin(pGame, pStage);			break;
	case ITEM_FIREFLOWER:				pObj = new FireFlower(pGame, pStage);			break;
	case ITEM_SUPERSTAR:				pObj = new SuperStar(pGame, pStage);			break;
	case ITEM_1UP:						pObj = new Mushroom1UP(pGame, pStage);			break;
	case ITEM_MUSHROOM:					pObj = new Mushroom(pGame, pStage);				break;
	case ITEM_VINE:						pObj = new Vine(pGame, pStage);					break;
	case ITEM_GIANTMUSHROOM:			pObj = new GiantMushroom(pGame, pStage);		break;
	case ITEM_YOSHIEGG:					pObj = new YoshiEgg(pGame, pStage);				break;
	case ITEM_CLOUD:					pObj = new CloudItem(pGame, pStage);			break;
	case ITEM_THROWNCOIN:				pObj = new ThrownCoin(pGame, pStage);			break;
	case ITEM_DOORKEYCOIN:				pObj = new DoorKeyCoin(pGame, pStage);			break;
	case ITEM_DOORKEY:					pObj = new DoorKey(pGame, pStage);				break;
	case ITEM_TURTLESHELL:				pObj = new TurtleShell(pGame, pStage);			break;
	default:
		{
			if (nType >= ITEM_MYSTERYMUSHROOM && nType <= ITEM_MYSTERYMUSHROOM_RESERVED_LAST)
				pObj = new MysteryMushroom(pGame, pStage);

		}
		break;
	}

	return pObj;
}
