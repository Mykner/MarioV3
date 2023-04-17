
// ChildFrm.cpp : CChildFrame 클래스의 구현
//

#include "stdafx.h"
#include "MarioMapEditor.h"

#include "ChildFrm.h"

#include "MarioMapEditorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWndEx)
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

// CChildFrame 생성/소멸

CChildFrame::CChildFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CChildFrame::~CChildFrame()
{
}


BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서 Window 클래스 또는 스타일을 수정합니다.
	if( !CMDIChildWndEx::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

// CChildFrame 진단

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}
#endif //_DEBUG

// CChildFrame 메시지 처리기


void CChildFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_CLOSE)
	{
		CMarioMapEditorView *pView = (CMarioMapEditorView*)GetActiveView();
		if (pView)
		{
			if (pView->IsPlaying())
			{
				pView->StopPlaying();

				// Wait until stop playing thread
				while (pView->IsPlaying())
				{
					Sleep(500);
				}
			}
		}
	}

	CMDIChildWndEx::OnSysCommand(nID, lParam);
}


void CChildFrame::OnClose()
{
	CMarioMapEditorView *pView = (CMarioMapEditorView*)GetActiveView();
	if (pView)
	{
		if (pView->IsPlaying())
		{
			pView->StopPlaying();
			
			// Wait until stop playing thread
			while (pView->IsPlaying())
			{
				Sleep(500);
			}			
		}
	}

	CMDIChildWndEx::OnClose();
}

