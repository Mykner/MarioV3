#include "VineLadder.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "FModManager.h"
#include "Stage.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(VineLadder)
	PROP_INT("Length", VT_I4, 3, nullptr, "VineLadder"),
END_IMPL_PROPERTYMAP()

VineLadder::VineLadder(Game *pGame, Stage *pStage) :
	EventObjectBase(pGame, pStage)
{
	m_nType = EVENTOBJECT_VINELADDER;
	m_nState = STATE_IDLE;

	m_nLength = 3;
}

VineLadder::~VineLadder()
{
}

void VineLadder::Process()
{
	if (m_bCollisionPlayer)
	{
		CUR_PLAYER_OBJ->OnApplyItem(this);
	}
}

void VineLadder::OnBeforeProcess()
{
	CheckCollisionPlayer();
}

void VineLadder::OnAfterProcess()
{
}

void VineLadder::Render(int nColor, int nZOrder)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	float _x = x - pt.x;
	float _y = y - pt.y ;
	bool bShadow = (m_pGame->m_bModernStyle);

	SIZE s = GetSize();
	_y -= s.cy;
	_y += 16;

	for (int i = 0; i < m_nLength; i++)
	{
		int nFrame;
		if (i == 0)
			nFrame = SPRIDX_MAPOBJ_VINE_TOP1;
		else if (i == m_nLength - 1)
			nFrame = SPRIDX_MAPOBJ_VINE_BOTTOM1;
		else
			nFrame = SPRIDX_MAPOBJ_VINE_MIDDLE1;

		(*m_ppSprite)->RenderToQueue(_x, _y + (i * 16), nFrame,
			false, false, nColor, 1.0f, 1.0f, Z_BEHIND_MAP, bShadow);
	}
}

NaRect VineLadder::GetRect()
{
	NaRect rc;
	SIZE s = GetSize();

	rc.left = m_fX - (s.cx / 2);
	rc.right = rc.left + s.cx;
	rc.top = m_fY - s.cy;
	rc.bottom = m_fY;

	return rc;
}

NaRectT<float> VineLadder::GetRectFloat()
{
	NaRectT<float> rc;
	SIZE s = GetSize();
	
	rc.left = m_fX - (s.cx / 2);
	rc.right = rc.left + s.cx;
	rc.top = m_fY - s.cy;
	rc.bottom = m_fY;

	return rc;
}

SIZE VineLadder::GetSize()
{
	SIZE s = {
		4,
		16 * m_nLength
	};
	return s;
}
