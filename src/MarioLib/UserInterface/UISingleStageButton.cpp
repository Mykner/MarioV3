#include "UISingleStageButton.h"

#include "Common.h"
#include "RenderManager.h"
#include "InputManager.h"
#include "UIManager.h"
#include "FModManager.h"
#include "UIPanel.h"


UISingleStageButton::UISingleStageButton(Game *pGame) :
	UIButton(pGame)
{
	m_nType = UI_SINGLESTAGEBUTTON;

	m_bFocusAccept = true;

	m_strBestTime = L"--:--:--.--";
	m_strBestScore.Format(L"%8d", 0);

	m_strFolder = L"";
	m_strFile = L"";
	m_strPathName = L"";
}

UISingleStageButton::~UISingleStageButton()
{
}

void UISingleStageButton::Process()
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

void UISingleStageButton::Render()
{
	NaRect rc = GetScreenRect();

	if (IsFocused())
	{
		RenderRoundBox(rc, 0x70ffbf70, Z_UI_BUTTON_BACKGROUND);
	}

	RENDER_MANAGER->Text(rc.left + 8, rc.top + 8,
		m_strText, 0xffffffff, 1.0f, Z_UI_BUTTON_TEXT);
	
	RENDER_MANAGER->Text(rc.right - 92, rc.top + 2, 
		m_strBestTime, 0xffffffff, 1.0f, Z_UI_BUTTON_TEXT);

	RENDER_MANAGER->Text(rc.right - 68, rc.top + 16 - 2, 
		m_strBestScore, 0xffffffff, 1.0f, Z_UI_BUTTON_TEXT);
}

NaRect UISingleStageButton::GetRect()
{
	int len = m_strText.GetLength();
	int nWidth = m_pParent ? m_pParent->GetRect().Width() : 224;

	NaRect rc;
	rc.left = m_nX;
	rc.top = m_nY;
	rc.right = m_nX + nWidth;
	rc.bottom = m_nY + 24;

	return rc;
}
