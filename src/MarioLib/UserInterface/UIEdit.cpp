#include "UIEdit.h"

#include "Game.h"
#include "RenderManager.h"
#include "InputManager.h"
#include "UIManager.h"
#include "FModManager.h"
#include "UIPanel.h"

UIEdit::UIEdit(Game *pGame) :
	UIText(pGame)
{
	m_nType = UI_EDIT;

	m_bFocusAccept = true;
	m_strText = L"";
	m_strEmptyText = L"<EMPTY>";
	m_strTitleText = L"";
	m_bAllowAll = false;
	m_bIsPassword = false;
}

UIEdit::~UIEdit()
{
}

void UIEdit::Process()
{
	UIManager *pUIManager = m_pGame->m_pUIManager;

#define K2C(k, c) if (KE(k) && !KS(VK_SHIFT)) m_strText.AppendFormat("%c", c);    
#define K2C2(k, c) if (KE(k) && KS(VK_SHIFT)) m_strText.AppendFormat("%c", c);    
#ifdef WIN32
	if (m_bAllowAll)
	{
		// #TODO
		K2C(VK_SPACE, ' ');

		K2C(VK_OEM_PERIOD, '.');
		K2C2(VK_OEM_PERIOD, '>');

		K2C(VK_OEM_COMMA, ',');
		K2C2(VK_OEM_COMMA, '?');
		
		K2C(VK_OEM_MINUS, '-');
		K2C2(VK_OEM_MINUS, '_');

		K2C(VK_OEM_1, ';');
		K2C2(VK_OEM_1, ':');

		K2C(VK_OEM_2, '/');
		K2C2(VK_OEM_2, '?');

		K2C(VK_OEM_3, '`');
		K2C2(VK_OEM_3, '~');

		K2C(VK_OEM_5, '\\');
		K2C2(VK_OEM_5, '|');

		K2C(VK_OEM_7, '\'');
		K2C2(VK_OEM_7, '\"');

		K2C2(VK_1, '!');
		K2C2(VK_2, '@');
		K2C2(VK_3, '#');
		K2C2(VK_4, '$');
		K2C2(VK_5, '%');
		K2C2(VK_6, '^');
		K2C2(VK_7, '&');
		K2C2(VK_8, '*');
		K2C2(VK_9, '(');
		K2C2(VK_0, ')');

		if (!KS(VK_SHIFT))
		{
			for (int i = VK_0; i <= VK_9; i++)
			{
				if (KE(i))
					m_strText.AppendFormat("%c", i);
			}
		}

		for (int i = VK_A; i <= VK_Z; i++)
		{
			if (KE(i))
			{
				if (KS(VK_SHIFT))
					m_strText.AppendFormat("%c", i);
				else
					m_strText.AppendFormat("%c", i + ('a' - 'A'));
			}
		}
	}
	else
	{
		for (int i = VK_0; i <= VK_Z; i++)
		{
			if (KE(i))
			{
				m_strText.AppendFormat("%c", i);
			}
		}
	}
#else
	// WTF MacOS KeyCode?! 
    K2C(VK_1, '1'); K2C(VK_2, '2'); K2C(VK_3, '3'); K2C(VK_4, '4'); K2C(VK_5, '5'); 
	K2C(VK_6, '6'); K2C(VK_7, '7'); K2C(VK_8, '8'); K2C(VK_9, '9'); K2C(VK_0, '0');

    K2C(VK_Q, 'Q'); K2C(VK_W, 'W'); K2C(VK_E, 'E'); K2C(VK_R, 'R'); K2C(VK_T, 'T');
    K2C(VK_Y, 'Y'); K2C(VK_U, 'U'); K2C(VK_I, 'I'); K2C(VK_O, 'O'); K2C(VK_P, 'P');
    
    K2C(VK_A, 'A'); K2C(VK_S, 'S'); K2C(VK_D, 'D'); K2C(VK_F, 'F'); K2C(VK_G, 'G');
    K2C(VK_H, 'H'); K2C(VK_J, 'J'); K2C(VK_K, 'K'); K2C(VK_L, 'L');
    
    K2C(VK_Z, 'Z'); K2C(VK_X, 'X'); K2C(VK_C, 'C'); K2C(VK_V, 'V'); K2C(VK_B, 'B');
    K2C(VK_N, 'N'); K2C(VK_M, 'M');
#endif

	if (KE(VK_BACK))
	{
		if (m_strText.GetLength() > 0)
		{
			m_strText = m_strText.Left(m_strText.GetLength() - 1);
		}
	}
	if (KE(VK_RETURN))
	{
		pUIManager->m_pClicked = this;

		PLAYSOUND(L"MenuTak");
	}
}

void UIEdit::Render()
{
	if (!m_bVisible)
		return;

	if (m_pParent->m_pFocused == this)
	{
		if (m_pGame->m_nGameFrame % 32 < 16)
		{
			NaRect rc = GetScreenRect();
			DRAWTEXT(rc.right, rc.top, L"_");
		}
	}

	DWORD dwTextColor;
	if (m_bEnable)
		dwTextColor = m_dwTextColor;
	else
		dwTextColor = 0xff707070;
	
	NaRect rc = GetScreenRect();
	NaString str;
	NaString strText = m_strText;
	if (strText.GetLength() <= 0)
		strText = m_strEmptyText;

	// If Text is too long, cut left side.
	int nMaxVisibleLen = (m_pParent->GetRect().Width()) / 8;
	if (m_strTitleText.GetLength() + strText.GetLength() > nMaxVisibleLen)
	{
		if (nMaxVisibleLen - m_strTitleText.GetLength() - 3 <= 0)
		{
			NaDebugOut(L"Error: UIEdit calculation bug occurred.\n");
		}
		else
		{
			NaString strTemp = strText.Right(
				nMaxVisibleLen - m_strTitleText.GetLength() - 3
			);

			strTemp.Format(L"...%ls", strTemp.wstr());
			strText = strTemp;
		}
	}

	if (m_bIsPassword)
	{
		int nLen = strText.GetLength();
		strText = L"";
		for (int i=0; i<nLen; i++)
			strText += L"*";
	}

	str.Format(L"%ls%ls", m_strTitleText.wstr(), strText.wstr());
	RENDER_MANAGER->Text(rc.left, rc.top,
		str, dwTextColor, 1.0f, Z_UI_TEXT);
}

NaRect UIEdit::GetRect()
{
	NaRect rc;
	bool bDefault = true;
	if (m_pParent->m_pFocused != this)
	{
		if (m_strText.GetLength() <= 0)
		{
			rc.left = m_nX;
			rc.top = m_nY;
			rc.right = m_nX + 
				((m_strTitleText.GetLength()) * 8) +
				((m_strEmptyText.GetLength()) * 8);
			rc.bottom = m_nY + 8;

			bDefault = false;
		}
	}

	if (bDefault)
	{
		rc.left = m_nX;
		rc.top = m_nY;
		rc.right = m_nX +
			((m_strTitleText.GetLength()) * 8) +
			((m_strText.GetLength()) * 8);
		rc.bottom = m_nY + 8;

		// If text too long.
		if (m_pParent->GetRect().Width() <= rc.Width())
			rc.right = rc.left + (m_pParent->GetRect().Width());
	}
	return rc;
}

// Get... expected rect
NaRect UIEdit::GetMaxRect()
{
	NaRect rc;
	bool bDefault = true;
	if (m_pParent->m_pFocused != this)
	{
		if (m_strText.GetLength() <= 0)
		{
			rc.left = m_nX;
			rc.top = m_nY;
			rc.right = m_nX +
				((m_strTitleText.GetLength()) * 8) +
				((m_strEmptyText.GetLength()) * 8);
			rc.bottom = m_nY + 8;

			bDefault = false;
		}
	}

	if (bDefault)
	{
		rc.left = m_nX;
		rc.top = m_nY;
		rc.right = m_nX +
			((m_strTitleText.GetLength()) * 8) +
			((m_strText.GetLength()) * 8);
		rc.bottom = m_nY + 8;
	}
	return rc;
}

NaRect UIEdit::GetScreenRect()
{
	NaRect rc = GetRect();

	POINT ptOffset = GetScreenOffset();
	rc.Offset(ptOffset.x, ptOffset.y);

	return rc;
}

