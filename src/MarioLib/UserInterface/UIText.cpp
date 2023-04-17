#include "UIText.h"

#include "Game.h"
#include "RenderManager.h"
#include "UIPanel.h"

UIText::UIText(Game *pGame) :
	UIBase(pGame)
{
	m_nType = UI_TEXT;
	m_dwTextColor = 0xffffffff;
}

UIText::~UIText()
{
}

void UIText::Render()
{
	if (!m_bVisible)
		return;

	NaRect rc = GetScreenRect();
	if (m_bEnable)
	{
		RENDER_MANAGER->Text(rc.left, rc.top, 
			m_strText, m_dwTextColor, 1.0f, Z_UI_TEXT);
	}
	else
	{
		RENDER_MANAGER->Text(rc.left, rc.top,
			m_strText, 0xff707070, 1.0f, Z_UI_TEXT);
	}
}

NaRect UIText::GetRect()
{
	int len = m_strText.GetLength();

	NaRect rc;
	rc.left = m_nX;
	rc.top = m_nY;
	rc.right = m_nX + (len * 8);
	rc.bottom = m_nY + 8;

	return rc;
}

NaRect UIText::GetScreenRect()
{
	NaRect rc = GetRect();

	POINT ptOffset = GetScreenOffset();
	rc.Offset(ptOffset.x, ptOffset.y);

	return rc;
}
