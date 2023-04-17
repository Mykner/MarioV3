// AnimationTestDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MarioSpriteEditor.h"
#include "AnimationTestDlg.h"
#include "afxdialogex.h"

#include "MarioSpriteEditorView.h"
#include "MarioSpriteEditorDoc.h"

// AnimationTestDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(AnimationTestDlg, CDialogEx)

AnimationTestDlg::AnimationTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ANIMATION_TEST, pParent)
{
	m_pMainView = nullptr;
	m_nCurFrame = 0;
	m_nZoomFactor = 4;
}

AnimationTestDlg::~AnimationTestDlg()
{
}

void AnimationTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
}


BEGIN_MESSAGE_MAP(AnimationTestDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_ADD, &AnimationTestDlg::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_DELETE, &AnimationTestDlg::OnBnClickedBtnDelete)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &AnimationTestDlg::OnNMDblclkList1)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &AnimationTestDlg::OnNMClickList1)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST1, &AnimationTestDlg::OnLvnKeydownList1)
END_MESSAGE_MAP()


// AnimationTestDlg 메시지 처리기입니다.


void AnimationTestDlg::OnBnClickedBtnAdd()
{
	CMarioSpriteEditorDoc* pDoc = m_pMainView->GetDocument();

	SpriteAnimationFrames *f = new SpriteAnimationFrames;
	pDoc->m_SpriteWork.m_vecAnimationFrames.push_back(f);

	int nIdx = m_List.GetItemCount();
	m_List.InsertItem(nIdx, L"AnyName");
	m_List.SetItemText(nIdx, 1, L"1,2,3");
	//m_List.SetItemData(nIdx, (DWORD)&f);
	//UpdateListItem(nIdx);
	SelectListItem(nIdx);
}


void AnimationTestDlg::OnBnClickedBtnDelete()
{
	CMarioSpriteEditorDoc* pDoc = m_pMainView->GetDocument();
	int nIdx = GetSelectedListIndex();

	{
		std::vector<SpriteAnimationFrames*>::iterator it = pDoc->m_SpriteWork.m_vecAnimationFrames.begin();
		for (int i = 0; ; ++i, ++it)
		{
			if (i == nIdx)
			{
				pDoc->m_SpriteWork.m_vecAnimationFrames.erase(it);
				break;
			}
		}
	}

	// TODO Select = -1
	m_List.DeleteItem(nIdx);
	SelectListItem(nIdx);
}

void AnimationTestDlg::Init()
{
	static bool bFirstInit = true;

	m_List.DeleteAllItems();

	m_List.ModifyStyle(0, LVS_SHOWSELALWAYS);
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERINALLVIEWS);
	//m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERINALLVIEWS);
	RECT rcList;
	m_List.GetClientRect(&rcList);
	if (bFirstInit)
	{
		m_List.InsertColumn(0, L"Name", 0, 80, -1);
		m_List.InsertColumn(1, L"Frames", 0, rcList.right - rcList.left - 80, -1);
		bFirstInit = false;
	}

	CMarioSpriteEditorDoc* pDoc = m_pMainView->GetDocument();
	if (pDoc->m_SpriteWork.m_vecAnimationFrames.size() > 0)
	{
		BEGIN_VECTOR_LOOP(SpriteAnimationFrames*, pDoc->m_SpriteWork.m_vecAnimationFrames) {
			int nIdx = m_List.GetItemCount();
			m_List.InsertItem(nIdx, L".");
			m_List.SetItemData(nIdx, (DWORD)&(*it));

			m_List.SetItemText(nIdx, 0, pObj->m_strName.wstr());
			m_List.SetItemText(nIdx, 1, pObj->m_strFrames.wstr());
		} END_VECTOR_LOOP();

		SelectListItem(0);
	}

	SetTimer(0, 500, nullptr);
}

int AnimationTestDlg::GetSelectedListIndex()
{
	POSITION pos = m_List.GetFirstSelectedItemPosition();
	int nIdx = m_List.GetNextSelectedItem(pos);

	return nIdx;
}

void AnimationTestDlg::SelectListItem(int nIdx)
{
	int nOldIdx = GetSelectedListIndex();
	m_List.SetSelectionMark(-1);
	m_List.SetItemState(nOldIdx, 0, LVIS_SELECTED);

	int nCnt = m_List.GetItemCount();
	if (nIdx < nCnt)
	{
		m_List.SetSelectionMark(nIdx);
		m_List.SetItemState(nIdx, LVIS_SELECTED | LVIS_FOCUSED, 0xFF);
		m_List.SetFocus();
	}
}

void AnimationTestDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.

	int nIdx = GetSelectedListIndex();
	CMarioSpriteEditorDoc* pDoc = m_pMainView->GetDocument();
	CDC *pDC = CDC::FromHandle(dc.m_hDC);
	CRect rcClient;

	// Draw Sprite
	if (nIdx >= 0)
	{
		SpriteFrame f = GetCurSpriteFrame();

		CDC memdc;
		memdc.CreateCompatibleDC(pDC);
		HGDIOBJ hOldObj = memdc.SelectObject(pDoc->m_hBitmap);

		GetDlgItem(IDC_STATIC_PREVIEW)->GetWindowRect(rcClient);
		ScreenToClient(rcClient);

		CRgn rgn;
		rgn.CreateRectRgn(rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);
		pDC->SelectClipRgn(&rgn);

		CPoint ptCenter(
			rcClient.left + (rcClient.Width() / 2),
			rcClient.top + ((rcClient.Height() / 4) * 3)
		);

		//CRect rcArea = m_rcCurrentArea;
		CRect rcArea = f.m_rc;
		pDC->StretchBlt(
			-(f.m_ptOffset.x * m_nZoomFactor) + ptCenter.x,
			-(f.m_ptOffset.y * m_nZoomFactor) + ptCenter.y,
			rcArea.Width() * m_nZoomFactor,
			rcArea.Height() * m_nZoomFactor,
			&memdc,
			rcArea.left, rcArea.top,
			rcArea.Width(),
			rcArea.Height(),
			SRCCOPY);

		int nArrowLen = 10;
		pDC->MoveTo(ptCenter);
		pDC->LineTo(ptCenter.x - nArrowLen, ptCenter.y + nArrowLen);
		pDC->MoveTo(ptCenter);
		pDC->LineTo(ptCenter.x + nArrowLen, ptCenter.y + nArrowLen);
		pDC->MoveTo(ptCenter);
		pDC->LineTo(ptCenter.x, ptCenter.y + (nArrowLen * 2));

		HGDIOBJ hNullBrush = GetStockObject(NULL_BRUSH);
		HGDIOBJ hOldBrush = pDC->SelectObject(hNullBrush);
		pDC->Rectangle(&rcClient);
		pDC->SelectObject(hOldBrush);

		pDC->SelectClipRgn(NULL);
		memdc.SelectObject(hOldObj);
		memdc.DeleteDC();
	}
}


void AnimationTestDlg::OnTimer(UINT_PTR nIDEvent)
{
	CRect rcClient;
	GetDlgItem(IDC_STATIC_PREVIEW)->GetWindowRect(rcClient);
	ScreenToClient(rcClient);
	InvalidateRect(rcClient);

	m_nCurFrame++;
	int nMax = GetCurAnimationFrameCount();
	if (m_nCurFrame >= nMax)
		m_nCurFrame = 0;

	CDialogEx::OnTimer(nIDEvent);
}

SpriteFrame AnimationTestDlg::GetCurSpriteFrame()
{
	CMarioSpriteEditorDoc* pDoc = m_pMainView->GetDocument();
	int nIdx = GetSelectedListIndex();
	
	CString cstrFrame = m_List.GetItemText(nIdx, 1);
	NaString strFrame = cstrFrame;
	
	auto ar = strFrame.Split(L",");
	int nCurFrame = m_nCurFrame;
	if (nCurFrame >= ar.GetCount())
		nCurFrame = ar.GetCount();

	int nRealIndex = 0;
	try
	{
		nRealIndex = ar[nCurFrame].ToInt();
	}
	catch (...)
	{
		
	}

	SpriteFrame f = pDoc->m_Sprite.m_vecFrames[nRealIndex];

	return f;
}

int AnimationTestDlg::GetCurAnimationFrameCount()
{
	CMarioSpriteEditorDoc* pDoc = m_pMainView->GetDocument();
	int nIdx = GetSelectedListIndex();

	CString cstrFrame = m_List.GetItemText(nIdx, 1);
	NaString strFrame = cstrFrame;

	auto ar = strFrame.Split(L",");
	return ar.GetCount();
}


void AnimationTestDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	LPNMITEMACTIVATE pNMITEM = (LPNMITEMACTIVATE)pNMHDR;

	m_nCurRow = pNMITEM->iItem;
	m_nCurCol = pNMITEM->iSubItem;

	if (pNMITEM->iItem != -1)
	{
		CRect rect;

		if (pNMITEM->iSubItem == 0)
		{
			m_List.GetItemRect(pNMITEM->iItem, rect, LVIR_BOUNDS);
			rect.right = rect.left + m_List.GetColumnWidth(0);
		}
		else
		{
			m_List.GetSubItemRect(pNMITEM->iItem, pNMITEM->iSubItem, LVIR_BOUNDS, rect);
		}

		m_List.ClientToScreen(rect);
		this->ScreenToClient(rect);

		GetDlgItem(IDC_EDIT1)->SetWindowText(m_List.GetItemText(pNMITEM->iItem, pNMITEM->iSubItem));
		GetDlgItem(IDC_EDIT1)->SetWindowPos(NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW);
		GetDlgItem(IDC_EDIT1)->SetFocus();
	}

	*pResult = 0;
}


void AnimationTestDlg::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	LPNMITEMACTIVATE pNMITEM = (LPNMITEMACTIVATE)pNMHDR;

	m_nCurRow = m_nCurCol = -1;
	GetDlgItem(IDC_EDIT1)->SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW);

	if (pNMITEM->iItem != -1)
	{
		m_nCurRow = pNMITEM->iItem;
		m_nCurCol = pNMITEM->iSubItem;
	}

	*pResult = 0;
}


void AnimationTestDlg::OnLvnKeydownList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	
	if (pLVKeyDow->wVKey == VK_F2)
	{
		if (m_nCurRow != -1)
		{
			CRect rect;

			if (m_nCurCol == 0)
			{
				m_List.GetItemRect(m_nCurRow, rect, LVIR_BOUNDS);
				rect.right = rect.left + m_List.GetColumnWidth(0);
			}
			else
			{
				m_List.GetSubItemRect(m_nCurRow, m_nCurCol, LVIR_BOUNDS, rect);
			}

			m_List.ClientToScreen(rect);
			this->ScreenToClient(rect);

			GetDlgItem(IDC_EDIT1)->SetWindowText(m_List.GetItemText(m_nCurRow, m_nCurCol));

			GetDlgItem(IDC_EDIT1)->SetWindowPos(NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW);
			GetDlgItem(IDC_EDIT1)->SetFocus();
		}
	}

	*pResult = 0;
}


BOOL AnimationTestDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) 
	{
		if (pMsg->wParam == VK_RETURN) 
		{

			if (pMsg->hwnd == GetDlgItem(IDC_EDIT1)->GetSafeHwnd())
			{
				CString str;
				GetDlgItemText(IDC_EDIT1, str);
				m_List.SetItemText(m_nCurRow, m_nCurCol, str);

				GetDlgItem(IDC_EDIT1)->SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW);

				// Apply to workinfo
				CMarioSpriteEditorDoc* pDoc = m_pMainView->GetDocument();
				if (m_nCurCol == 0)
				{
					pDoc->m_SpriteWork.m_vecAnimationFrames[m_nCurRow]->m_strName = str;
				}
				else
				{
					pDoc->m_SpriteWork.m_vecAnimationFrames[m_nCurRow]->m_strFrames = str;
				}
			}

			return TRUE;
		}

		if (pMsg->wParam == VK_ESCAPE)
		{
			return TRUE;
		}
	}
	
	return CDialogEx::PreTranslateMessage(pMsg);
}
