#include "UIPanel.h"

#include "Game.h"
#include "UIManager.h"
#include "InputManager.h"
#include "FModManager.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "UIList.h"
#include "Stage.h"

UIPanel::UIPanel(Game *pGame) :
	UIContainer(pGame)
{
	m_nType = UI_PANEL;

	m_bAutoAlignX = false;
	m_bAutoAlignY = false;
	m_bAutoSize = true;
	m_nPanelType = PANEL_NORMAL;
}

UIPanel::~UIPanel()
{
}

void UIPanel::Process()
{
	UIManager *pUIManager = m_pGame->m_pUIManager;
	InputManager *pInputManager = m_pGame->m_pInputManager;

	int nIdx = 0;
	int nCurIdx = -1;
	BEGIN_VECTOR_LOOP(UIBase*, m_vecUI) {
		if (pObj == m_pFocused)
		{
			pObj->Process();
			nCurIdx = nIdx;
		}
		else if (pObj->CanAcceptFocus())
		{
			if (pUIManager->m_ptLastMouse.x != pInputManager->m_ptMouse.x ||
				pUIManager->m_ptLastMouse.y != pInputManager->m_ptMouse.y)
			{
				NaRect rc = pObj->GetScreenRect();
				if (rc.ptInRect(pInputManager->m_ptMouse.x, pInputManager->m_ptMouse.y))
				{
					m_pFocused = pObj;

					PLAYSOUND(L"MenuTok");

					// Pass Current Loop
					return;
				}
			}
		}
		nIdx++;
	} END_VECTOR_LOOP();

	if (m_vecUI.size() > 1)
	{
		if (KE(VK_UP) || KE(VK_LEFT))
		{
			if (m_pFocused->m_nType == UI_LIST)
			{
				if (((UIList*)m_pFocused)->MoveFocusPrev() == nullptr)
					MoveFocusPrev();
			}
			else
				MoveFocusPrev();
		}
		if (KE(VK_DOWN) || KE(VK_RIGHT))
		{
			if (m_pFocused->m_nType == UI_LIST)
			{
				if (((UIList*)m_pFocused)->MoveFocusNext() == nullptr)
					MoveFocusNext();
			}
			else
				MoveFocusNext();
		}
		if (KE(VK_HOME))
		{
			if (m_pFocused->m_nType == UI_LIST)
			{
				UIBase *pFocus = ((UIList*)m_pFocused)->GetFirstFocusAcceptableUI();
				if (pFocus)
					PLAYSOUND(L"MenuTok");

				((UIList*)m_pFocused)->m_pFocused = pFocus;
				((UIList*)m_pFocused)->EnsureVisible(pFocus);
			}
			else
				MoveFocusPrev();
		}
		if (KE(VK_END))
		{
			if (m_pFocused->m_nType == UI_LIST)
			{
				UIBase *pFocus = ((UIList*)m_pFocused)->GetLastFocusAcceptableUI();
				if (pFocus)
					PLAYSOUND(L"MenuTok");

				((UIList*)m_pFocused)->m_pFocused = pFocus;
				((UIList*)m_pFocused)->EnsureVisible(pFocus);
			}
			else
				MoveFocusNext();
		}
	}
}

void UIPanel::Render()
{
	switch (m_nPanelType)
	{
	case PANEL_ROUNDSHADOWBOX:
		if (m_vecUI.size() > 0)
		{
			RenderBoxUI();
		}
		break;
	}	

	BEGIN_VECTOR_LOOP(UIBase*, m_vecUI) {
		pObj->Render();
	} END_VECTOR_LOOP();
}

void UIPanel::RenderBoxUI()
{
	NaRect rc = m_rc;
	rc.Inflate(8);

	int nColor = 0xd0000000;
	if (CUR_STAGE->m_lBackgroundColor == 0xff000000)
	{
		//nColor = 0xb0505050;
		nColor = 0xd0151515;
	}

	RenderRoundBox(rc, nColor);
}

void UIPanel::AddUI(UIBase * pUI)
{
	UIContainer::AddUI(pUI);

	RecalcBoxSize();
	RecalcAlignPos();
}

void UIPanel::RecalcBoxSize()
{
	if (m_bAutoSize == false)
		return;

	m_rc.left = -1;
	m_rc.top = -1;
	m_rc.right = -1;
	m_rc.bottom = -1;

	BEGIN_VECTOR_LOOP(UIBase*, m_vecUI) {
		NaRect rc = pObj->GetMaxRect();
		if (m_rc.left == -1)
		{
			m_rc = rc;
		}
		else
		{
			if (rc.left < m_rc.left)
				m_rc.left = rc.left;
			if (rc.top < m_rc.top)
				m_rc.top = rc.top;
			if (rc.right > m_rc.right)
				m_rc.right = rc.right;
			if (rc.bottom > m_rc.bottom)
				m_rc.bottom = rc.bottom;
		}
	} END_VECTOR_LOOP();

	m_rc.Offset(m_nX, m_nY);
}

void UIPanel::RecalcAlignPos()
{
	if (m_bAutoAlignX)
	{
		int nWidth = m_rc.Width();
		m_rc.left = (GameWindowDefaults::nInnerWidth / 2) - (m_rc.Width() / 2);
		m_rc.right = m_rc.left + nWidth;
	}

	if (m_bAutoAlignY)
	{
		int nHeight = m_rc.Height();
		m_rc.top = (GameWindowDefaults::nInnerHeight / 2) - (m_rc.Height() / 2);
		m_rc.bottom = m_rc.top + nHeight;
	}
}

UIBase * UIPanel::MoveFocusPrev()
{
	UIBase *pFocus = UIContainer::MoveFocusPrev();
	if (pFocus == nullptr)
	{
		// Find Last Item
		pFocus = GetLastFocusAcceptableUI();
		if (pFocus)
			PLAYSOUND(L"MenuTok");

		m_pFocused = pFocus;
	}
	
	if (pFocus && pFocus->m_nType >= UI_CONTAINER)
	{
		UIContainer *pCont = ((UIContainer*)pFocus);
		UIBase *pChildFocus = pCont->GetLastFocusAcceptableUI();
		
		if (pChildFocus == nullptr)
			return nullptr;
		
		pCont->m_pFocused = pChildFocus;
		pCont->EnsureVisible(pChildFocus);
	}
	return pFocus;
}

UIBase * UIPanel::MoveFocusNext()
{
	UIBase *pFocus = UIContainer::MoveFocusNext();
	if (pFocus == nullptr)
	{
		// Find First Item
		pFocus = GetFirstFocusAcceptableUI();
		if (pFocus)
			PLAYSOUND(L"MenuTok");

		m_pFocused = pFocus;
	}
	
	if (pFocus && pFocus->m_nType >= UI_CONTAINER)
	{
		UIContainer *pCont = ((UIContainer*)pFocus);
		UIBase *pChildFocus = pCont->GetFirstFocusAcceptableUI();

		if (pChildFocus == nullptr)
			return nullptr;

		pCont->m_pFocused = pChildFocus;
		pCont->EnsureVisible(pChildFocus);
	}

	return pFocus;
}

NaRect UIPanel::GetRect()
{
	return m_rc;
}
