#include "UIRectangle.h"

#include "Common.h"
#include "RenderManager.h"
#include "InputManager.h"
#include "UIManager.h"
#include "FModManager.h"

#include "SpeechBubble.h"

UIRectangle::UIRectangle(Game *pGame) :
	UIText(pGame)
{
	m_nType = UI_RECTANGLE;

	m_bFocusAccept = false;

	m_bShadow = false;
	m_bRenderBox = false;
	m_pSprite = nullptr;
	m_nSpriteIndex = -1;
	m_nWidth = 100;
	m_nHeight = 100;
	m_dwTextColor = 0xffffffff;
	m_dwFillColor = 0xb0000000;
}

UIRectangle::~UIRectangle()
{
}

void UIRectangle::Process()
{
}

void UIRectangle::Render()
{
	if (!m_bVisible)
		return;

	NaRect rc = GetScreenRect();
	if (m_bRenderBox)
	{
		/*
		m_pSprite->RenderToQueue(rc.left - 4, rc.top - 4, SPRIDX_ETC_DOT, false, false,
			m_dwFillColor, rc.Width() + 8, rc.Height() + 8, Z_UI_BOX);
			*/

		/*
		SpeechBubble b(m_pGame);
		b.m_nX = rc.left - 4;
		b.m_nY = rc.top - 4;
		b.m_nWidth = rc.Width();
		b.m_nHeight = rc.Height();
		b.Render();
		*/

		NaRect rc2 = rc;
		rc2.left -= 4;
		rc2.top -= 4;
		rc2.right += 4;
		rc2.bottom += 4;
		RenderRoundBox(rc2, m_dwFillColor, Z_UI_BOX);
	}

	if (m_pSprite && m_nSpriteIndex >= 0)
	{
		m_pSprite->RenderToQueue(rc.left, rc.top, m_nSpriteIndex, false, false,
			0xffffffff, 1.0f, 1.0f, Z_UI_BOX + 1);
	}

	if (m_bEnable)
	{
		RENDER_MANAGER->Text(rc.left, rc.top,
			m_strText, m_dwTextColor, 1.0f, Z_UI_TEXT, m_bShadow);
	}
	else
	{
		RENDER_MANAGER->Text(rc.left, rc.top,
			m_strText, 0xff707070, 1.0f, Z_UI_TEXT, m_bShadow);
	}
}

NaRect UIRectangle::GetRect()
{
	//int len = m_strText.GetLength();

	NaRect rc;
	rc.left = m_nX;
	rc.top = m_nY;
	rc.right = m_nX + m_nWidth;
	rc.bottom = m_nY + m_nHeight;

	return rc;
}

NaRect UIRectangle::GetScreenRect()
{
	NaRect rc = GetRect();

	POINT ptOffset = GetScreenOffset();
	rc.Offset(ptOffset.x, ptOffset.y);

	return rc;
}
