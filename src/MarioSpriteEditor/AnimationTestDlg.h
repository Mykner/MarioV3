#pragma once
#include "afxcmn.h"

#include "Sprite.h"
#include "afxwin.h"

// AnimationTestDlg 대화 상자입니다.

class CMarioSpriteEditorView;
class AnimationTestDlg : public CDialogEx
{
	DECLARE_DYNAMIC(AnimationTestDlg)

public:
	AnimationTestDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~AnimationTestDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ANIMATION_TEST };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnDelete();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnKeydownList1(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	void Init();
	int GetSelectedListIndex();
	void SelectListItem(int nIdx);
	SpriteFrame GetCurSpriteFrame();
	int GetCurAnimationFrameCount();

	CMarioSpriteEditorView *m_pMainView;
	CListCtrl m_List;
	CEdit m_Edit;
	int m_nCurFrame;
	int m_nZoomFactor;
	int m_nCurRow, m_nCurCol;
};
