#include "UIButton.h"

#include "Common.h"
#include "RenderManager.h"
#include "InputManager.h"
#include "UIManager.h"
#include "FModManager.h"

UIButton::UIButton(Game *pGame) :
	UIText(pGame)
{
	m_nType = UI_BUTTON;

	m_bFocusAccept = true;

	m_bShadow = false;
	m_bRenderBox = false;
}

UIButton::~UIButton()
{
}

void UIButton::Process()
{
	InputManager *pInputManager = m_pGame->m_pInputManager;
	UIManager *pUIManager = m_pGame->m_pUIManager;

	NaRect rc = GetScreenRect();

	if (KE(VK_RETURN) || pInputManager->IsMouseClicked(rc))
	{
		pUIManager->m_pClicked = this;

		PLAYSOUND(L"MenuTak");
	}
}

void UIButton::Render()
{
	if (!m_bVisible)
		return;

	NaRect rc = GetScreenRect();
	if (m_bRenderBox)
	{
		m_pSprite->RenderToQueue(rc.left - 4, rc.top - 4, SPRIDX_ETC_DOT, false, false,
			0xb0000000, rc.Width() + 8, rc.Height() + 8, Z_UI_BOX);
	}

	if (m_bEnable)
	{
		RENDER_MANAGER->Text(rc.left, rc.top,
			m_strText, 0xffffffff, 1.0f, Z_UI_TEXT, m_bShadow);
	}
	else
	{
		RENDER_MANAGER->Text(rc.left, rc.top,
			m_strText, 0xff707070, 1.0f, Z_UI_TEXT, m_bShadow);
	}
}
