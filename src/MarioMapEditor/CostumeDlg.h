#pragma once

#include <map>

// CostumeDlg 대화 상자입니다.
class Game;
class Sprite;
class CostumeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CostumeDlg)

public:
	CostumeDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CostumeDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_COSTUME };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	Game *m_pGame;
	//Sprite *m_pSpriteCostume;
	Sprite *m_pSpriteItem;

	HBITMAP m_hBitmapCostume;
	SIZE m_sizeBitmapCostume;

	HBITMAP m_hBitmapItem;
	SIZE m_sizeBitmapItem;

	int m_nType;
	std::map<int, int> m_mapIndex;

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	void DrawSprite(Sprite *pSprite, int x, int y, int nFrame, CPaintDC &dc, CDC &memdc);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	CString GetCostumeName(int nIdx);
};
