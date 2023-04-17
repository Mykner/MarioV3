#include "UIContainer.h"

#include "Game.h"
#include "UIManager.h"
#include "InputManager.h"
#include "FModManager.h"
#include "Sprite.h"
#include "SpriteManager.h"

UIContainer::UIContainer(Game *pGame) :
	UIBase(pGame)
{
	m_nType = UI_CONTAINER;

	m_pFocused = nullptr;

	m_rc.left = -1;
	m_rc.top = -1;
	m_rc.right = -1;
	m_rc.bottom = -1;
}

UIContainer::~UIContainer()
{
}

void UIContainer::Process()
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
		if (KE(VK_UP))
		{
			while (true)
			{
				if (--nCurIdx < 0)
					nCurIdx = m_vecUI.size() - 1;
				if (m_vecUI[nCurIdx]->CanAcceptFocus())
				{
					m_pFocused = m_vecUI[nCurIdx];

					PLAYSOUND(L"MenuTok");
					break;
				}

				// Cannot find next focus acceptable
				if (m_vecUI[nCurIdx] == m_pFocused)
					break;
			}
		}
		if (KE(VK_DOWN))
		{
			while (true)
			{
				if (++nCurIdx >= m_vecUI.size())
					nCurIdx = 0;
				if (m_vecUI[nCurIdx]->CanAcceptFocus())
				{
					m_pFocused = m_vecUI[nCurIdx];

					PLAYSOUND(L"MenuTok");
					break;
				}

				// Cannot find next focus acceptable
				if (m_vecUI[nCurIdx] == m_pFocused)
					break;
			}
		}
	}
}

void UIContainer::Render()
{
	if (!m_bVisible)
		return;

	BEGIN_VECTOR_LOOP(UIBase*, m_vecUI) {
		pObj->Render();
	} END_VECTOR_LOOP();
}

void UIContainer::AddUI(UIBase * pUI)
{
	pUI->m_pParent = this;
	m_vecUI.push_back(pUI);

	if (m_pFocused == nullptr && pUI->CanAcceptFocus())
		m_pFocused = pUI;
}

int UIContainer::GetFocusedIndex()
{
	int nIndex = 0;
	int nRet = -1;

	BEGIN_VECTOR_LOOP(UIBase*, m_vecUI) {
		if (pObj == m_pFocused)
		{
			nRet = nIndex;
			break;
		}
		nIndex++;
	} END_VECTOR_LOOP();

	return nRet;
}

POINT UIContainer::GetChildOffset()
{
	POINT pt;
	pt.x = m_rc.left;
	pt.y = m_rc.top;

	return pt;
}

UIBase * UIContainer::MoveFocusPrev()
{
	int nFocus = GetFocusedIndex();

	while (true)
	{
		if (--nFocus < 0)
			return nullptr;

		if (m_vecUI[nFocus]->CanAcceptFocus())
		{
			m_pFocused = m_vecUI[nFocus];

			PLAYSOUND(L"MenuTok");
			break;
		}

		// Cannot find next focus acceptable
		if (m_vecUI[nFocus] == m_pFocused)
			return nullptr;
	}

	return m_pFocused;
}

UIBase * UIContainer::MoveFocusNext()
{
	int nFocus = GetFocusedIndex();

	while (true)
	{
		if (++nFocus >= m_vecUI.size())
			return nullptr;

		if (m_vecUI[nFocus]->CanAcceptFocus())
		{
			m_pFocused = m_vecUI[nFocus];

			PLAYSOUND(L"MenuTok");
			break;
		}

		// Cannot find next focus acceptable
		if (m_vecUI[nFocus] == m_pFocused)
			return nullptr;
	}

	return m_pFocused;
}

UIBase * UIContainer::GetFirstFocusAcceptableUI()
{
	UIBase *pRet = nullptr;
	if (m_vecUI.size() > 0)
	{
		int nIdx = 0;
		while (true)
		{
			pRet = m_vecUI[nIdx];
			if (pRet->CanAcceptFocus())
			{
				return pRet;
			}

			// Cannot find!
			if (++nIdx >= m_vecUI.size())
				return nullptr;
		}
	}

	return pRet;
}

UIBase * UIContainer::GetLastFocusAcceptableUI()
{
	UIBase *pRet = nullptr;
	if (m_vecUI.size() > 0)
	{
		int nIdx = m_vecUI.size() - 1;
		while (true)
		{
			pRet = m_vecUI[nIdx];
			if (pRet->CanAcceptFocus())
			{
				return pRet;
			}

			// Cannot find!
			if (--nIdx < 0)
				return nullptr;
		}
	}

	return pRet;
}

void UIContainer::EnsureVisible(UIBase * pChild)
{
}

void UIContainer::Clear()
{
	BEGIN_VECTOR_LOOP(UIBase*, m_vecUI) {
		if (pObj->m_nType >= UI_CONTAINER)
			((UIContainer*)pObj)->Clear();
		delete pObj;
	} END_VECTOR_LOOP();

	m_vecUI.clear();
}
