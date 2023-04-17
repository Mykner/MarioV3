#include "UIWorldStageButton.h"

#include "Common.h"
#include "RenderManager.h"
#include "InputManager.h"
#include "UIManager.h"
#include "FModManager.h"
#include "UIPanel.h"
#include "SpriteManager.h"

UIWorldStageButton::UIWorldStageButton(Game *pGame) :
	UIButton(pGame)
{
	m_nType = UI_WORLDSTAGEBUTTON;

	m_bFocusAccept = true;

	m_strBestTime = L"--:--:--.--";
	m_strBestScore.Format(L"%8d", 0);
	m_pSpriteEtc = SPR_MANAGER->Request(L"Etc");

	m_pMetaInfo = nullptr;

	m_nGameTheme = -1;
	m_bPlayed = false;
}

UIWorldStageButton::~UIWorldStageButton()
{
	if (m_pMetaInfo)
	{
		delete m_pMetaInfo;
		m_pMetaInfo = nullptr;
	}
}

void UIWorldStageButton::Process()
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

void UIWorldStageButton::Render()
{
	NaRect rc = GetScreenRect();

	if (IsFocused())
	{
		RenderRoundBox(rc, 0x70ffbf70, Z_UI_BUTTON_BACKGROUND);
	}

	DWORD dwDefaultColor = 0xffffffff;
	DWORD dwCreatorColor = 0xff888888;

	if (m_pMetaInfo->m_nDeveloperRating == 0)
	{
		dwDefaultColor = 0x88444444;
		dwCreatorColor = 0x88444444;
	}

	NaString str;
	if (m_strText.GetLength() > 22)
		str.Format(L"%ls...", m_strText.Left(22).wstr());
	else
		str.Format(L"%ls", m_strText.wstr());
	RENDER_MANAGER->Text(rc.left + 8, rc.top + 4,
		str, dwDefaultColor, 1.0f, Z_UI_BUTTON_TEXT);

	str.Format(L"by %ls", m_pMetaInfo->m_strCreator.wstr());
	RENDER_MANAGER->Text(rc.left + 8, rc.top + 12,
		str, dwCreatorColor, 1.0f, Z_UI_BUTTON_TEXT);

	{
		float fClearRate;
		if (m_pMetaInfo->m_nCleared == 0 && m_pMetaInfo->m_nFailed == 0)
			fClearRate = -1;
		else
			fClearRate = (m_pMetaInfo->m_nCleared * 100.0f) / (m_pMetaInfo->m_nCleared + m_pMetaInfo->m_nFailed);
		NaString strClearRate;
		if (fClearRate == -1)
			strClearRate.Format(L"-");
		else
			strClearRate.Format(L"%.0f%%", fClearRate);

		NaString strDevRating;
		if (m_pMetaInfo->m_nDeveloperRating == -1)
			strDevRating.Format(L"%ls?", WSZ_NEUTRAL, m_pMetaInfo->m_nDeveloperRating);
		else if (m_pMetaInfo->m_nDeveloperRating == 0)
			strDevRating.Format(L"%ls%d", WSZ_BAD, m_pMetaInfo->m_nDeveloperRating);
		else
			strDevRating.Format(L"%ls%d", WSZ_GOOD, m_pMetaInfo->m_nDeveloperRating);

		NaString strNewLevel = L"";
		if (m_pMetaInfo->m_nCleared == 0 && m_pMetaInfo->m_nFailed == 0)
			strNewLevel.Format(L"%ls ", WSZ_NEW);

		NaString strPlayed = L"";
		if (m_bPlayed)
			strPlayed.Format(L"%ls ", WSZ_PLAYED);

		str.Format(L"%ls%ls%ls%d %ls%ls %ls%d %ls",
			strPlayed.wstr(),
			strNewLevel.wstr(),
			WSZ_DOWNLOAD, m_pMetaInfo->m_nDownload,
			WSZ_CLEAR, strClearRate.wstr(),
			WSZ_LIKE, m_pMetaInfo->m_nLike,
			strDevRating.wstr()
			);

		RENDER_MANAGER->Text(rc.right - 4 - (str.GetLength() * 8), rc.top + 4,
			str, dwDefaultColor, 1.0f, Z_UI_BUTTON_TEXT);

		// Game theme
		if (m_nGameTheme == -1)
			m_nGameTheme = m_pMetaInfo->CalcGameTheme();

		m_pSpriteEtc->RenderToQueue(
			rc.right - 30, rc.top + 12, SPRIDX_ETC_THEME_SMB1 + (m_nGameTheme - 1),
			false, false, dwDefaultColor, 1.0f, 1.0f, Z_UI_BUTTON_TEXT
		);

		// Difficulty
		int nDiff = -1;
		if (fClearRate > 0 && fClearRate <= 10)
			nDiff = SPRIDX_ETC_HELL;
		else if (fClearRate > 10 && fClearRate <= 40)
			nDiff = SPRIDX_ETC_HARD;
		else if (fClearRate > 40 && fClearRate <= 90)
			nDiff = SPRIDX_ETC_NORMAL;
		else if (fClearRate > 90)
			nDiff = SPRIDX_ETC_EASY;

		m_pSpriteEtc->RenderToQueue(
			rc.right - 70, rc.top + 12, nDiff,
			false, false, dwDefaultColor, 1.0f, 1.0f, Z_UI_BUTTON_TEXT
		);
	}

	if (m_pMetaInfo->m_nDeveloperRating == 0)
	{
		RENDER_MANAGER->Text(rc.left + 104, rc.top + 8,
			L"<Not recommended>", 0xffa00000, 1.0f, Z_UI_BUTTON_TEXT);
	}
	
}

NaRect UIWorldStageButton::GetRect()
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
