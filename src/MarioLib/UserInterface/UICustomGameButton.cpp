#include "UICustomGameButton.h"

#include "Common.h"
#include "RenderManager.h"
#include "InputManager.h"
#include "UIManager.h"
#include "FModManager.h"
#include "UIPanel.h"


UICustomGameButton::UICustomGameButton(Game *pGame) :
	UIButton(pGame)
{
	m_nType = UI_CUSTOMGAMEBUTTON;

	m_bFocusAccept = true;
	m_strCreator = L"";
	m_strLastStage = L"";
	m_nLife = -1;
	m_nCoin = -1;
	m_nShape = -1;
}

UICustomGameButton::~UICustomGameButton()
{
}

void UICustomGameButton::Process()
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

void UICustomGameButton::Render()
{
	NaRect rc = GetScreenRect();

	if (IsFocused())
	{
		RenderRoundBox(rc, 0x70ffbf70, Z_UI_BUTTON_BACKGROUND);
	}

	RENDER_MANAGER->Text(rc.left + 8, rc.top + 4,
		m_strText, 0xffffffff, 1.0f, Z_UI_BUTTON_TEXT);

	NaString str;
	if (m_strCreator.GetLength() > 0)
		str.Format(L"Created by %ls", m_strCreator.wstr());
	else
		str.Format(L"Created by Unknown");

	RENDER_MANAGER->Text(rc.left + 16, rc.top + 16,
		str, 0xff999999, 1.0f, Z_UI_BUTTON_TEXT);

	if (m_strLastStage.GetLength() > 0)
	{
		NaString str;
		str.Format(L"Last Stage: %ls", m_strLastStage.wstr());
		RENDER_MANAGER->Text(rc.left + 16, rc.top + 26,
			str, 0xff999999, 1.0f, Z_UI_BUTTON_TEXT);

		Sprite *pSprite = SPR_MANAGER->Request(L"MarioSmall");
		pSprite->RenderToQueue(
			rc.right - 52, rc.top + 20, 0,
			false, false, -1L, 1.0f, 1.0f, Z_UI_BUTTON_TEXT, true);

		str.Format(L"x");
		RENDER_MANAGER->Text(rc.right - 44, rc.top + 14,
			str, 0xff999999, 1.0f, Z_UI_BUTTON_TEXT);

		str.Format(L"%d", m_nLife);
		RENDER_MANAGER->Text(rc.right - 36, rc.top + 6,
			str, 0xff999999, 2.0f, Z_UI_BUTTON_TEXT);

		str.Format(L"$x%d", m_nCoin);
		RENDER_MANAGER->Text(rc.right - 52, rc.top + 26,
			str, 0xff999999, 1.0f, Z_UI_BUTTON_TEXT);
	}
}

NaRect UICustomGameButton::GetRect()
{
	int len = m_strText.GetLength();
	int nWidth = m_pParent ? m_pParent->GetRect().Width() : 224;

	NaRect rc;
	rc.left = m_nX;
	rc.top = m_nY;
	rc.right = m_nX + nWidth;
	rc.bottom = m_nY + 48;

	return rc;
}
