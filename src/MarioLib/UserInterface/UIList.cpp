#include "UIList.h"

#include "Common.h"
#include "RenderManager.h"
#include "InputManager.h"
#include "UIManager.h"
#include "FModManager.h"
#include "UIPanel.h"


UIList::UIList(Game *pGame) :
	UIContainer(pGame)
{
	m_nType = UI_LIST;

	m_bFocusAccept = true;

	m_nWidth = 0;
	m_nHeight = 0;

	m_nCurrentScrollPos = 0;
	m_nTargetScrollPos = 0;

	m_nItemHeight = 24;
}

UIList::~UIList()
{
}

void UIList::Process()
{
	InputManager *pInputManager = m_pGame->m_pInputManager;
	UIManager *pUIManager = m_pGame->m_pUIManager;

	int nIdx = 0;
	int nCurIdx = -1;
	BEGIN_VECTOR_LOOP(UIBase*, m_vecUI) {
		if (pObj == m_pFocused)
		{
			pObj->Process();
			nCurIdx = nIdx;
		}
		else if (pObj->CanAcceptFocus() && IsVisibleChild(pObj))
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

	if (pInputManager->m_nWheelMoved != 0)
	{
		int nScrollMax = (m_vecUI.size() * m_nItemHeight) - (GetRect().Height());
		if (nScrollMax < 0)
			nScrollMax = 0;

		m_nTargetScrollPos += (m_nItemHeight * pInputManager->m_nWheelMoved);
		if (-m_nTargetScrollPos < 0)
			m_nTargetScrollPos = 0;
		if (-m_nTargetScrollPos > nScrollMax)
			m_nTargetScrollPos = -nScrollMax;
	}

	if (m_nTargetScrollPos != m_nCurrentScrollPos)
	{
		int nTotalOffset = (m_nTargetScrollPos - m_nCurrentScrollPos);
		int nOffset = nTotalOffset / 4;
		if (nOffset == 0 && nTotalOffset > 0)
			nOffset = 1;
		else if (nOffset == 0 && nTotalOffset < 0)
			nOffset = -1;
		
		m_nCurrentScrollPos += nOffset;
	}

	if (NeedScrollBar())
	{
		// Bar
		NaRect rc = GetScreenRect();
		NaRect rc2;
		rc2.left = rc.right + 1;
		rc2.right = rc2.left + 16;
		rc2.top = rc.top;
		rc2.bottom = rc.bottom;

		if (pInputManager->IsMouseClicked(rc2))
		{
			float fClickRatio = 
				(pInputManager->m_ptMouse.y - rc2.top) * 100.0f / (rc2.Height());
			float fTotalHeight = (m_vecUI.size() * m_nItemHeight) - rc.Height();
			
			m_nTargetScrollPos = -(fTotalHeight * fClickRatio) / 100.0f;
		}
	}
}

void UIList::Render()
{
	if (!m_bVisible)
		return;

	NaRect rc = GetScreenRect();
	RenderRoundBox(rc, 0xc0000000);

	// #TODO need Vector increment protector
	BEGIN_VECTOR_LOOP(UIBase*, m_vecUI) {
		if (IsVisibleChild(pObj))
			pObj->Render();
	} END_VECTOR_LOOP();

	// ScrollBar
	float fTotal = m_vecUI.size();
	float fVisibleCnt = rc.Height() / m_nItemHeight;
	if (fTotal > 0 && fTotal > fVisibleCnt)
	{
		float fTotalHeight = m_vecUI.size() * m_nItemHeight;

		// Bar
		NaRect rc2;
		rc2.left = rc.right + 1;
		rc2.right = rc2.left + 8;
		rc2.top = rc.top;
		rc2.bottom = rc.bottom;
		RenderBox(rc2, 0xc0000000);

		// Button
		NaRect rcButton = rc2;
		fTotalHeight -= rc.Height();

		float fRatio = (fVisibleCnt * 100.0f) / (fTotal); // 0 ~ 100
		rcButton.bottom = rcButton.top + (rc.Height() * fRatio / 100.0f);
		if (rcButton.Height() < 16)
			rcButton.bottom = rcButton.top + 16;

		float fScrolledRatio = m_nCurrentScrollPos * 100.0f / (fTotalHeight);
		float fButtonOffset = (rc.Height() - rcButton.Height()) * (-fScrolledRatio) / 100.0f;
		rcButton.Offset(0, fButtonOffset);

		RenderBox(rcButton, 0xc0ffffff, Z_UI_BOX + 1);
	}
}

UIBase * UIList::MoveFocusPrev()
{
	UIBase *pRet = UIContainer::MoveFocusPrev();
	if (pRet)
		EnsureVisible(pRet);

	return pRet;
}

UIBase * UIList::MoveFocusNext()
{
	UIBase *pRet = UIContainer::MoveFocusNext();
	if (pRet)
		EnsureVisible(pRet);

	return pRet;
}

POINT UIList::GetChildOffset()
{
	POINT pt;
	pt.x = m_rc.left;
	pt.y = m_rc.top;

	pt.y += m_nCurrentScrollPos;

	return pt;
}

bool UIList::NeedScrollBar()
{
	NaRect rc = GetScreenRect();

	float fTotal = m_vecUI.size();
	float fVisibleCnt = rc.Height() / m_nItemHeight;
	if (fTotal > 0 && fTotal > fVisibleCnt)
		return true;
	return false;
}

bool UIList::IsVisibleChild(UIBase * pChild)
{
	NaRect rcList = GetScreenRect();
	NaRect rcItem = pChild->GetScreenRect();

	if (rcList.top > rcItem.top)
	{
		return false;
	}
	else if (rcList.bottom < rcItem.bottom)
	{
		return false;
	}

	return true;
}

void UIList::EnsureVisible(UIBase * pChild)
{
	NaRect rcList = GetScreenRect();
	NaRect rcItem = pChild->GetScreenRect();

	if (rcList.top > rcItem.top)
	{
		// Item is hide at top
		m_nTargetScrollPos = m_nCurrentScrollPos + (rcList.top - rcItem.top);
	}
	else if (rcList.bottom < rcItem.bottom)
	{
		// Item is hide at bottom
		m_nTargetScrollPos = m_nCurrentScrollPos - (rcItem.bottom - rcList.bottom);
	}
}

bool UIList::CanAcceptFocus()
{
	bool bRet = UIContainer::CanAcceptFocus();
	if (bRet)
	{
		if (m_vecUI.size() == 0)
			return false;
	}
	return bRet;
}

NaRect UIList::GetRect()
{
	NaRect rc;
	rc.left = m_nX;
	rc.top = m_nY;
	rc.right = m_nX + m_nWidth;
	rc.bottom = m_nY + m_nHeight;

	return rc;
}
