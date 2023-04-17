
// MarioMapEditorView.h : CMarioMapEditorView 클래스의 인터페이스
//

#pragma once

#include "MarioMapEditorDoc.h"

#include "MainFrm.h"

extern CMarioMapEditorApp theApp;

#define WM_USER_LOADDIALOGCOMPLETE	(WM_USER + 1)

class Game;
class DesignStage;
class DesignPlayGame;
class CMarioMapEditorView : public CScrollView
{
protected: // serialization에서만 만들어집니다.
	CMarioMapEditorView();
	DECLARE_DYNCREATE(CMarioMapEditorView)

// 특성입니다.
public:
	CMarioMapEditorDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CMarioMapEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	Game *GetGame();

	CPoint ClientToGame(CPoint &pt);
	CPoint GameToTile(CPoint &pt);
	CPoint ClientToTile(CPoint &pt);

	void RecalcScrollSize();
	void SyncScrollPos();

	void InvalidateGame();

	void SetEditMode(int nMode);
	void SetCurTool(int nId);
	int GetCurToolID();
	void CancelTool();
	void RemoveSelected();
	void MakeDuplicate(int nCount);
	void QuickChangeProperty();
	void QuickPowerUp();
	void QuickToggleWing();
	bool OnDragMoveComplete();

	int m_nMode;
	int m_nEditType;
	BOOL m_bLButtonDown;

	DesignStage* GetStage();
	void RefreshProperties();
	void OnPropertyChange(NaString strName);

	DesignPlayGame* m_pDesignPlayGame;
	BOOL IsPlaying();
	void StopPlaying();
	void DoPlaying(int nX, int nY);
	static UINT PlayGameThreadProc(LPVOID pArg);

	void OnActivateView();
	bool IsActiveView();

	void ActivateSubStage(int nIdx, bool bRedraw = true);

	void UpdateStatusBar(int nID);
	CMainFrame* GetMainFrame();

	void ShowResourceList();

protected:

// 생성된 메시지 맵 함수
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	LRESULT OnLoadDialogComplete(WPARAM wParam, LPARAM lParam);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnClearGroundBlocks();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnAutobuildingUndergroundbonusstage();
	afx_msg void OnCreateBalancedLift();
	afx_msg void OnCreatePiranhaplantAllpipe();
	afx_msg void OnInsertEmptypage();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCheckValid();
	afx_msg void OnToggleModernstyle();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSubstagesAddsubstage();
	afx_msg void OnSubstagesMainstage();
	afx_msg void OnSubstagesSubstage1();
	afx_msg void OnSubstagesSubstage2();
	afx_msg void OnSubstagesSubstage3();
	afx_msg void OnSubstagesSubstage4();
	afx_msg void OnQuickChangeProperty();
	afx_msg void OnQuickPowerUp();
	afx_msg void OnQuickToggleWing();
	afx_msg void OnSelectAbilityFlags();
	afx_msg void OnFileUpload();
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
};

#ifndef _DEBUG  // MarioMapEditorView.cpp의 디버그 버전
inline CMarioMapEditorDoc* CMarioMapEditorView::GetDocument() const
   { return reinterpret_cast<CMarioMapEditorDoc*>(m_pDocument); }
#endif

