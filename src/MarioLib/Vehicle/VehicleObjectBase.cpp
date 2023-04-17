#include "VehicleObjectBase.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "ThemeManager.h"
#include "SoundManager.h"

#include "Yoshi.h"
#include "Cloud.h"
#include "ClownCar.h"
#include "GoombaShoe.h"

VehicleObjectBase::VehicleObjectBase(Game *pGame, Stage *pStage) :
	ControllableObject(pGame, pStage)
{
	m_nType = VEHICLE_NONE;
	m_nState = STATE_IDLE;
	
	RequestSprites(pStage);

	m_nDamageGroup = DAMAGEGROUP_NEUTRAL;
	m_bFlip = false;

	m_pPassenger = nullptr;
	m_nRideType = STATE_STAND;
}

VehicleObjectBase::~VehicleObjectBase()
{
	if (m_pPassenger)
	{
		m_pPassenger->m_pVehicle = nullptr;
		m_pPassenger = nullptr;
	}
	m_pController = nullptr;
}

void VehicleObjectBase::Process()
{
	switch (m_nState)
	{
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
}

void VehicleObjectBase::Render(int nColor, int nZOrder)
{
	if (m_nDamageCooltime > 0)
	{
		if (m_pGame->m_nGameFrame % 2 == 1)
			return;
	}

	NaPointT<float> pt = CAMERA->GetPosition();
	int nFrame = GetSpriteIndex();

	nZOrder = GetZOrder();
	
	if (nFrame != -1)
	{
		bool bShadow = (m_pGame->m_bModernStyle);
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

		(*m_ppSprite)->RenderToQueue(x - pt.x, y - pt.y, nFrame, m_bFlip, m_bFlipV, nColor,
			fScaleX, fScaleY, nZOrder, bShadow, fAngle);

		if (m_bWinged)
			RenderWing(nColor, nZOrder);
	}
}

void VehicleObjectBase::ChangeState(int nState, ChangeStateInfo *pInfo)
{
	ControllableObject::ChangeState(nState, pInfo);
}

bool VehicleObjectBase::CheckRideVehicle()
{
	return false;
}

void VehicleObjectBase::OnIdle()
{
	if (CheckFalling())
		ChangeState(STATE_STAND);
}

void VehicleObjectBase::OnStand()
{

}

void VehicleObjectBase::OnPipeUp()
{
	if (m_pPassenger)
	{
		ControllableObject::OnPipeUp();
	}
	else
	{
		if (m_nStateFrame > 16)
			ChangeState(m_nBaseState);
	}
}

void VehicleObjectBase::PositionPassenger()
{
	SIZE s = GetSize();

	m_pPassenger->m_fX = m_fX;
	m_pPassenger->m_fY = m_fY - s.cy;
}

void VehicleObjectBase::OnRidePassenger(GameObjectBase * pPassenger)
{
	m_pPassenger = pPassenger;
	m_nDamageGroup = pPassenger->m_nDamageGroup;

	pPassenger->m_fXS = 0;
	pPassenger->m_fYS = 0;

	m_pController = pPassenger->m_pController;

	PositionPassenger();
}

void VehicleObjectBase::OnGetOffPassenger()
{
	if (m_pPassenger)
	{
		if (m_pPassenger->m_nState == STATE_RIDE &&
			m_pPassenger->m_nNextState != STATE_GOTOFFVEHICLE)
			m_pPassenger->ChangeState(STATE_STAND);

		m_nDamageGroup = DAMAGEGROUP_NEUTRAL;
		m_pPassenger->m_pVehicle = nullptr;
		m_pPassenger = nullptr;
	
		CUR_STAGE->m_vecReservedGameObject.push_back(this);
	}

	m_pController = nullptr;
}

void VehicleObjectBase::OnDamagedPassenger()
{
	if (m_pPassenger && m_pPassenger->m_nDamageGroup != DAMAGEGROUP_ENEMY)
	{
		m_pPassenger->m_nDamageCooltime = GameDefaults::nDamageCooltime;
		m_pPassenger->ChangeState(STATE_GOTOFFVEHICLE);

		m_nDamageCooltime = GameDefaults::nDamageCooltime;
		OnGetOffPassenger();
	}
	else
	{
		OnGetOffPassenger();
	}
}

bool VehicleObjectBase::IsOpponent(GameObjectBase * pObj)
{
	if (m_pPassenger)
		return m_pPassenger->IsOpponent(pObj);
	
	return ControllableObject::IsOpponent(pObj);
}

bool VehicleObjectBase::HasMushroom()
{
	if (m_pPassenger)
	{
		return m_pPassenger->HasMushroom();
	}

	return false;
}

bool VehicleObjectBase::CanRided(GameObjectBase * pByWho)
{
	switch (m_nState)
	{
	case STATE_PIPEUP:
		return false;
		break;
	}

	return true;
}

bool VehicleObjectBase::CanBornFromBlock()
{
	return true;
}

bool VehicleObjectBase::CanCarry()
{
	return false;
}

void VehicleObjectBase::SetPosition(float fX, float fY)
{
	ControllableObject::SetPosition(fX, fY);

	PositionPassenger();
}

int VehicleObjectBase::GetSpriteIndex()
{
	return -1;
}

int VehicleObjectBase::GetZOrder()
{
	switch (m_nState)
	{
	case STATE_THROWN:
		if (m_nStateFrame < 16)
			return Z_BEHIND_MAP;
		break;
	case STATE_ITEMPOP:
	case STATE_ITEMPOPDOWN:
	case STATE_PIPEUP:
	case STATE_PIPEDOWN:
	case STATE_PIPELEFT:
	case STATE_PIPERIGHT:
		return Z_BEHIND_MAP;
		break;
	}

	return Z_VEHICLE;
}

NaRect VehicleObjectBase::GetRect()
{
	NaRect rcMe = GameObjectBase::GetRect();
	if (m_pPassenger == nullptr)
		return rcMe;

	if (m_pPassenger != this)
	{
		NaRect rcP = m_pPassenger->GetRect();

		// Add two rect.
		if (rcP.left < rcMe.left)
			rcMe.left = rcP.left;
		if (rcP.right > rcMe.right)
			rcMe.right = rcP.right;
		if (rcP.top < rcMe.top)
			rcMe.top = rcP.top;
		if (rcP.bottom > rcMe.bottom)
			rcMe.bottom = rcP.bottom;
	}
	return rcMe;
}

int VehicleObjectBase::GetMaskedType()
{
	int nType = m_nType;
	if (m_bPowerUp)
		nType |= ENEMY_MASK_POWERUP;
	if (m_bWinged)
		nType |= ENEMY_MASK_WINGED;

	return nType;
}

GameObjectBase * VehicleObjectBase::CreateInstance(Game * pGame, Stage * pStage, int nType)
{
	GameObjectBase *pObj = nullptr;
	switch (nType)
	{
	case VEHICLE_YOSHI:			pObj = new Yoshi(pGame, pStage);		break;
	case VEHICLE_CLOUD:			pObj = new Cloud(pGame, pStage);		break;
	case VEHICLE_CLOWNCAR:		pObj = new ClownCar(pGame, pStage);		break;
	case VEHICLE_GOOMBASHOE:	pObj = new GoombaShoe(pGame, pStage);	break;
	}

	return pObj;
}
