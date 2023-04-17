#include "Princess.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(Princess)
	PROP_BOOL("IsReal",		VT_BOOL,		false,	0, "Princess"),
	PROP_STR("ThankWord",	VT_ETC_NASTR,	"",		0, "Princess"),
END_IMPL_PROPERTYMAP()

Princess::Princess(Game *pGame, Stage *pStage) :
	EventObjectBase(pGame, pStage)
{
	m_nType = EVENTOBJECT_PRINCESS;
	m_nState = STATE_IDLE;

	m_bIsReal = false;
	m_strThankWord = L"";
}

Princess::~Princess()
{
}

void Princess::CheckCollisionPlayer()
{
	m_bCollisionPlayer = false;
	if ((GameObjectBase*)CUR_PLAYER_OBJ == this)
		return;

	NaRect rcPlayer = CUR_PLAYER_OBJ->GetRect();
	if (rcPlayer.ptInRect(m_fX - (TILE_XS / 2) - 4, m_fY - 1))
	{
		m_bCollisionPlayer = true;
	}
}

void Princess::Process()
{
	if (CUR_PLAYER_OBJ->m_fX > m_fX)
		m_bFlip = true;
	else
		m_bFlip = false;

	if (CanApplied() && m_bCollisionPlayer)
	{
		CUR_PLAYER_OBJ->OnApplyEvent(this);
		ChangeState(STATE_EVENTDEAD);
	}
}

void Princess::OnBeforeProcess()
{
	CheckCollisionPlayer();
}

void Princess::Render(int nColor, int nZOrder)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	float _x = x - pt.x;
	float _y = y - pt.y;
	
	int nFrame = m_bIsReal ? SPRIDX_MAPOBJ_PEACH : SPRIDX_MAPOBJ_TOAD;
	bool bShadow = (m_pGame->m_bModernStyle);

	(*m_ppSprite)->RenderToQueue(_x, _y, nFrame,
		m_bFlip, false, nColor, m_fScaleX, m_fScaleY, Z_COMMON_OBJ, bShadow);
}

bool Princess::CanApplied()
{
	if (m_nState != STATE_IDLE)
		return false;

	return true;
}

SIZE Princess::GetSize()
{
	SIZE s = { 16, 24 };
	return s;
}
