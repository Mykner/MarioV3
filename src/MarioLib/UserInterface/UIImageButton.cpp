#include "UIImageButton.h"

#include "Common.h"
#include "RenderManager.h"
#include "InputManager.h"
#include "UIManager.h"
#include "FModManager.h"
#include "Sprite.h"
#include "SpeechBubble.h"
#include "UIContainer.h"

UIImageButton::UIImageButton(Game *pGame) :
	UIText(pGame)
{
	m_nType = UI_IMAGEBUTTON;

	m_bFocusAccept = true;
	m_pSprite = nullptr;
	m_nSpriteIndex = -1;
	m_nWidth = 100;
	m_nHeight = 100;
	m_bText = true;
	m_bSpeechBubble = false;
	m_bRenderBox = true;
}

UIImageButton::~UIImageButton()
{
}

void UIImageButton::Process()
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

void UIImageButton::Render()
{
	NaRect rc = GetScreenRect();
	if (m_pSprite && m_nSpriteIndex != -1)
	{
		NaRect rcBox = rc;
		rcBox.left -= 4;
		rcBox.top -= 4;
		rcBox.right += 4;
		rcBox.bottom += 4;
		if (m_bText)
			rcBox.bottom += 8;

		// Background Box
		if (m_bRenderBox)
		{
			m_pSprite->RenderToQueue(rcBox.left, rcBox.top, SPRIDX_ETC_DOT, false, false,
				0xb0000000, rcBox.Width(), rcBox.Height(), Z_UI_BOX);
		}

		// Main Image
		m_pSprite->RenderToQueue(rc.left, rc.top, m_nSpriteIndex, false, false,
			0xffffffff, 1.0f, 1.0f, Z_UI_BOX + 1);

		// Text
		if (m_bText)
		{
			RENDER_MANAGER->Text(rc.left, rc.bottom + 2,
				m_strText, 0xffffffff, 1.0f, Z_UI_TEXT, true);
		}

		// Speech Bubble
		if (m_bSpeechBubble && m_pParent->m_pFocused == this)
		{
			SpeechBubble b(m_pGame);
			b.RenderWithTextAutoSized(rc.Center(), rc.top - 4, m_strText);
		}
	}
	else
	{
		m_pSprite->RenderToQueue(rc.left, rc.top, SPRIDX_ETC_DOT, false, false,
			0xb0000000, rc.Width(), rc.Height(), Z_UI_BOX);

		RENDER_MANAGER->Text(rc.left + 8, rc.top + 8,
			m_strText, 0xffffffff, 1.0f, Z_UI_TEXT);
	}
}

NaRect UIImageButton::GetRect()
{
	int len = m_strText.GetLength();

	NaRect rc;
	rc.left = m_nX;
	rc.top = m_nY;
	rc.right = m_nX + m_nWidth;
	rc.bottom = m_nY + m_nHeight;

	return rc;
}
