#include "RockyWrench.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "SoundManager.h"
#include "Wrench.h"

BEGIN_IMPL_PROPERTYMAP(RockyWrench)
	PROP_BOOL("PowerUp", VT_BOOL, false, nullptr, "RockyWrench")
END_IMPL_PROPERTYMAP()

RockyWrench::RockyWrench(Game *pGame, Stage *pStage) :
	EnemyObjectBase(pGame, pStage)
{
	m_nType = ENEMY_ROCKYWRENCH;
	m_nState = STATE_FALL;

	m_bFlip = true;
	m_nBlockBottom = -1;
	m_nThrowCooltime = 0;
	m_pWrench = nullptr;
	m_nBaseState = STATE_FALL;
}

RockyWrench::~RockyWrench()
{
}

void RockyWrench::Render(int nColor, int nZOrder)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	int nFrame = GetSpriteIndex();

	if (nZOrder == -1)
		nZOrder = GetZOrder();

	if (nFrame != -1)
	{
		bool bShadow = m_pGame->m_bModernStyle && nZOrder != Z_MAP_SHADOW;
		float fAngle = 0;

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
	}
}

void RockyWrench::ChangeState(int nState, ChangeStateInfo *pInfo)
{
	switch (nState)
	{
	case STATE_MOVEUP:
		m_fYS = -0.5f * m_fScaleY;
		break;
	case STATE_MOVEDOWN:
		m_fYS = 1.0f * m_fScaleY;
		break;
	case STATE_JUMPINTO:
		m_fYS = -2.0f;
		m_nBlockBottom = m_fY + (16 * m_fScaleY);
		break;
	case STATE_PRESSED:
		{
			if (m_pWrench)
			{
				m_pWrench->m_pOwner = nullptr;
				m_pWrench->ChangeState(STATE_DEAD);
				m_pWrench = nullptr;
			}

			EnemyObjectBase::ChangeState(nState, pInfo);
			return;
		}
		break;
	case STATE_PUSHEDUP:
	case STATE_BURNT:
	case STATE_HELDBYTONGUE:
		if (m_pWrench)
		{
			m_pWrench->m_pOwner = nullptr;
			m_pWrench->ChangeState(STATE_DEAD);
			m_pWrench = nullptr;
		}
		break;
	default:
		EnemyObjectBase::ChangeState(nState, pInfo);
		return;
		break;
	}

	m_nNextState = nState;
}

void RockyWrench::OnAfterProcess()
{
	EnemyObjectBase::OnAfterProcess();

	if (m_pWrench)
	{
		auto ptHand = GetHandPosition();
		m_pWrench->m_fX = ptHand.x;
		m_pWrench->m_fY = ptHand.y;
	}
}

void RockyWrench::OnStand()
{
	if (m_fX > CUR_PLAYER_OBJ->m_fX)
		m_bFlip = true;
	else
		m_bFlip = false;

	if (m_bCollision[COLLISION_BOTTOM])
	{
		if (m_nStateFrame == 0)
		{
			CreateWrench();
			m_nThrowCooltime = 60;
		}

		if (m_nThrowCooltime == 0 && m_nStateFrame >= 80)
		{
			ChangeState(STATE_MOVEDOWN);
		}
	}
	else
	{
		ChangeState(STATE_FALL);
	}

	if (m_nThrowCooltime > 0)
	{
		m_nThrowCooltime--;
		if (m_nThrowCooltime == 20)
			ThrowWrench();
	}
}

void RockyWrench::OnMoveDown()
{
	if (m_fY >= m_nBlockBottom)
	{
		m_fY = m_nBlockBottom;
		m_fYS = 0;
	}

	if (m_fY == m_nBlockBottom && m_nStateFrame >= 80)
	{
		if (m_fX > CUR_PLAYER_OBJ->m_fX)
			m_bFlip = true;
		else
			m_bFlip = false;
		ChangeState(STATE_MOVEUP);
	}
}

void RockyWrench::OnMoveUp()
{
	if (m_nStateFrame == 3)
		m_fYS = 0;

	if (m_nStateFrame == 100)
		m_fYS = (-0.5f * m_fScaleY);

	int nOffsetFromBlock = (10 + (m_fScaleY > 1 ? (m_fScaleY - 1) * 16.0f : 0));
	if (m_fY <= m_nBlockBottom - nOffsetFromBlock)
	{
		m_fY = m_nBlockBottom - nOffsetFromBlock;
		m_fYS = 0;
	}

	if (m_fY == m_nBlockBottom - nOffsetFromBlock && m_nStateFrame >= 80)
		ChangeState(STATE_STAND);
}

void RockyWrench::OnLanding()
{
	m_fY = m_ptCollision[COLLISION_BOTTOM].y;

	if (m_bStandOnJumpBlock)
		ChangeState(STATE_BLOCKJUMP);
	else
		ChangeState(STATE_JUMPINTO);
}

void RockyWrench::OnJumpInto()
{
	int nOffsetFromBlock = 10;
	if (m_fY <= m_nBlockBottom - nOffsetFromBlock)
	{
		OnApplyGravity(0.5f);
	}
	else
	{
		OnApplyGravity(1.0f);

		if (m_fY >= m_nBlockBottom)
		{
			CreateParameter param;
			param.nType = EFFECT_DUST;
			param.fXS = -0.5f;
			param.fYS = -0.5f;
			param.fScaleX = 1.0f * m_fScaleX;
			param.fScaleY = 1.0f * m_fScaleY;
			CUR_STAGE->CreateEffect(m_fX - (8 * m_fScaleX), m_fY - (16 * m_fScaleY), EFFECT_DUST, &param);

			param.fXS = 0.5f;
			param.fYS = -0.5f;
			CUR_STAGE->CreateEffect(m_fX + (8 * m_fScaleX), m_fY - (16 * m_fScaleY), EFFECT_DUST, &param);

			m_fY = m_nBlockBottom;
			m_fYS = 0;

			if (m_fX > CUR_PLAYER_OBJ->m_fX)
				m_bFlip = true;
			else
				m_bFlip = false;
			ChangeState(STATE_MOVEUP);
		}
	}
}

void RockyWrench::OnStacked()
{
	if (m_nStateFrame % 200 == 0 && m_pWrench == nullptr)
	{
		CreateWrench();
		m_nThrowCooltime = 60;
	}

	if (m_nThrowCooltime > 0)
	{
		m_nThrowCooltime--;
		if (m_nThrowCooltime == 20)
			ThrowWrench();
	}
}

void RockyWrench::CreateWrench()
{
	if (IsOutOfCamera() || m_pWrench != nullptr)
		return;

	POINT ptHand = GetHandPosition();

	CreateParameter param;
	param.pFromPlayer = nullptr;
	param.pFromObject = this;
	param.nType = -1;

	param.fXS = 0;
	param.fYS = 0;
	param.pFromObject = this;

	if (m_bPowerUp)
	{
		param.fScaleX = 2.0f;
		param.fScaleY = 2.0f;
	}

	m_pWrench = (Wrench*)CUR_STAGE->CreateBullet(
		ptHand.x,
		ptHand.y, 
		//m_fX, m_fY - (12.0f * m_fScaleY), 
		BULLET_WRENCH, &param);
}

void RockyWrench::ThrowWrench()
{
	if (m_pWrench == nullptr)
		return;

	m_pWrench->ChangeState(STATE_WALK);
	m_pWrench = nullptr;
}

bool RockyWrench::IsAlive()
{
	return EnemyObjectBase::IsAlive();
}

bool RockyWrench::CanSpit()
{
	return false;
}

bool RockyWrench::CanPressed(int nPower)
{
	if (CanCollide(COLLISION_TOP))
		return false;

	return true;
}

bool RockyWrench::CanBurnt(int nType)
{
	return true;
}

bool RockyWrench::CanBornFromBlock()
{
	return false;
}

bool RockyWrench::CanDamageEnemy()
{
	if (CanCollide(COLLISION_TOP))
		return false;

	return false;
}

bool RockyWrench::CanCollide(int nDir)
{
	if (m_nBlockBottom != -1)
	{
		if (m_nBlockBottom - m_fY < 10)
			return true;
	}

	return false;
}

int RockyWrench::GetSpriteIndex()
{
	if (EnemyObjectBase::GetSpriteIndex() == -1)
		return -1;

	switch (m_nState)
	{
	case STATE_FALL:
		if (m_fYS > 0)
			return SPRIDX_ROCKYWRENCH2;
		else
			return SPRIDX_ROCKYWRENCH1;
		break;
	case STATE_STAND:
	case STATE_STACKED:
		if (m_nThrowCooltime > 20)
			return SPRIDX_ROCKYWRENCH3;

		return SPRIDX_ROCKYWRENCH1;
		break;
	case STATE_BURNT:
	case STATE_PUSHEDUP:
		if (m_nStateFrame % 16 > 8)
			return SPRIDX_ROCKYWRENCH_DIE2;

		return SPRIDX_ROCKYWRENCH_DIE1;
		break;
	}

	return SPRIDX_ROCKYWRENCH1;
}

int RockyWrench::GetZOrder()
{
	switch(m_nState)
	{
	case STATE_MOVEDOWN:
	case STATE_MOVEUP:
	case STATE_STAND:
	case STATE_JUMPINTO:
		return Z_BEHIND_MAP;
		break;
	}

	return Z_AFTER_MAP;
}

SIZE RockyWrench::GetSize()
{
	SIZE s;
	s.cx = 16 * m_fScaleX;
	s.cy = 20 * m_fScaleY;

	return s;
}

POINT RockyWrench::GetHandPosition()
{
	POINT pt;
	pt.x = m_fX + (m_fScaleX * (m_bFlip ? 2 : -2));
	pt.y = m_fY - (m_fScaleY * 12);

	return pt;
}
