#include "UIBase.h"

#include "Game.h"
#include "NaRect.h"
#include "UIContainer.h"
#include "Sprite.h"
#include "SpriteManager.h"

UIBase::UIBase(Game *pGame)
{
	m_nID = -1;
	m_nX = -1;
	m_nY = -1;
	m_bFocusAccept = false;
	m_bEnable = true;
	m_bVisible = true;
	m_pParent = nullptr;
	m_pGame = pGame;

	m_nType = UI_BASE;

	m_pSprite = SPR_MANAGER->Find(L"Etc");
}

UIBase::~UIBase()
{
}

void UIBase::Process()
{
}

void UIBase::Render()
{
}

NaRect UIBase::GetRect()
{
	NaRect rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = 0;
	rc.bottom = 0;
	return rc;
}

NaRect UIBase::GetMaxRect()
{
	return GetRect();
}

NaRect UIBase::GetScreenRect()
{
	NaRect rc = GetRect();

	POINT ptOffset = GetScreenOffset();
	rc.Offset(ptOffset.x, ptOffset.y);

	return rc;
}

bool UIBase::CanAcceptFocus()
{
	if (m_bEnable)
		return m_bFocusAccept;
	return false;
}

bool UIBase::IsFocused()
{
	if (!m_bFocusAccept)
		return false;

	UIContainer *pParent = m_pParent;
	UIBase *pChild = this;
	while (pParent)
	{
		if (pParent->m_pFocused != pChild)
			return false;
		
		// Panel is Layer
		if (pParent->m_nType == UI_PANEL)
			break;

		// List is component
		pChild = pParent;
		pParent = pParent->m_pParent;
	}
	return true;
}

POINT UIBase::GetScreenOffset()
{
	POINT pt = { 0, 0 };
	UIContainer *pParent = m_pParent;
	while (pParent)
	{
		POINT ptParentOffset = pParent->GetChildOffset();
		pt.x += ptParentOffset.x;
		pt.y += ptParentOffset.y;

		// Panel is Layer
		if (pParent->m_nType == UI_PANEL)
			break;
		
		// List is component
		pParent = pParent->m_pParent;
	}

	return pt;
}

void UIBase::RenderBox(NaRect rc, int nColor, int nZOrder)
{
	if (!m_bVisible)
		return;

	int x = rc.left;
	int y = rc.top;
	float fScaleX = rc.Width() / 8.0f;
	float fScaleY = rc.Height() / 8.0f;

	if (nZOrder == -1)
		nZOrder = Z_UI_BOX;

	m_pSprite->RenderToQueue(x, y, SPRIDX_ETC_UI_BOX_C,
		false, false, nColor, fScaleX, fScaleY, nZOrder);
}

void UIBase::RenderRoundBox(NaRect rc, int nColor, int nZOrder)
{
	if (!m_bVisible)
		return;

	int x = rc.left;
	int y = rc.top;

	if (nZOrder == -1)
		nZOrder = Z_UI_BOX;

	/*
	// top
	m_pSprite->RenderToQueue(x, y, SPRIDX_ETC_UI_BOX_LT,
		false, false, nColor, 1.0f, 1.0f, nZOrder);

	for (x += 8; x < rc.right - 8; x += 8)
	{
		m_pSprite->RenderToQueue(x, y, SPRIDX_ETC_UI_BOX_C,
			false, false, nColor, 1.0f, 1.0f, nZOrder);
	}

	m_pSprite->RenderToQueue(x, y, SPRIDX_ETC_UI_BOX_RT,
		false, false, nColor, 1.0f, 1.0f, nZOrder);

	// middle
	for (y += 8; y < rc.bottom - 8; y += 8)
	{
		x = rc.left;
		for (; x < rc.right; x += 8)
		{
			m_pSprite->RenderToQueue(x, y, SPRIDX_ETC_UI_BOX_C,
				false, false, nColor, 1.0f, 1.0f, nZOrder);
		}
	}

	// bottom
	x = rc.left;
	m_pSprite->RenderToQueue(x, y, SPRIDX_ETC_UI_BOX_LB,
		false, false, nColor, 1.0f, 1.0f, nZOrder);

	for (x += 8; x < rc.right - 8; x += 8)
	{
		m_pSprite->RenderToQueue(x, y, SPRIDX_ETC_UI_BOX_C,
			false, false, nColor, 1.0f, 1.0f, nZOrder);
	}

	m_pSprite->RenderToQueue(x, y, SPRIDX_ETC_UI_BOX_RB,
		false, false, nColor, 1.0f, 1.0f, nZOrder);
		*/

	// top
	m_pSprite->RenderToQueue(x, y, SPRIDX_ETC_UI_BOX_LT,
		false, false, nColor, 1.0f, 1.0f, nZOrder);

	if (rc.Width() > 16)
	{
		NaRect rc2;
		rc2.left = x + 8;
		rc2.right = x + rc.Width() - 8;
		rc2.top = y;
		rc2.bottom = y + 8;
		RenderBox(rc2, nColor, nZOrder);
	}

	m_pSprite->RenderToQueue(rc.right - 8, y, SPRIDX_ETC_UI_BOX_RT,
		false, false, nColor, 1.0f, 1.0f, nZOrder);

	// middle
	y += 8;
	if (rc.Height() > 16)
	{
		NaRect rc2;
		rc2.left = x;
		rc2.right = x + rc.Width();
		rc2.top = y;
		rc2.bottom = y + rc.Height() - 16;
		RenderBox(rc2, nColor, nZOrder);
	}

	// bottom
	y += (rc.Height() - 16);
	x = rc.left;
	m_pSprite->RenderToQueue(x, y, SPRIDX_ETC_UI_BOX_LB,
		false, false, nColor, 1.0f, 1.0f, nZOrder);

	if (rc.Width() > 16)
	{
		NaRect rc2;
		rc2.left = x + 8;
		rc2.right = x + rc.Width() - 8;
		rc2.top = y;
		rc2.bottom = y + 8;
		RenderBox(rc2, nColor, nZOrder);
	}

	m_pSprite->RenderToQueue(rc.right - 8, y, SPRIDX_ETC_UI_BOX_RB,
		false, false, nColor, 1.0f, 1.0f, nZOrder);
}
