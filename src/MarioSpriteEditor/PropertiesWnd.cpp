
#include "stdafx.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "MarioSpriteEditor.h"

#include "Common.h"
#include "Sprite.h"
#include "MarioSpriteEditorView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd::CPropertiesWnd()
{
	m_nComboHeight = 0;
}

CPropertiesWnd::~CPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_COMMAND(ID_PROPERTIES1, OnProperties1)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
	ON_COMMAND(ID_PROPERTIES2, OnProperties2)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES2, OnUpdateProperties2)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar 메시지 처리기

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd () == NULL || (AfxGetMainWnd() != NULL && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	//m_wndObjectCombo.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), m_nComboHeight, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top + m_nComboHeight, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + m_nComboHeight + cyTlb, rectClient.Width(), rectClient.Height() -(m_nComboHeight+cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 콤보 상자를 만듭니다.
	/*
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndObjectCombo.Create(dwViewStyle, rectDummy, this, 1))
	{
		TRACE0("속성 콤보 상자를 만들지 못했습니다. \n");
		return -1;      // 만들지 못했습니다.
	}

	m_wndObjectCombo.AddString(_T("응용 프로그램"));
	m_wndObjectCombo.AddString(_T("속성 창"));
	m_wndObjectCombo.SetCurSel(0);

	CRect rectCombo;
	m_wndObjectCombo.GetClientRect (&rectCombo);

	m_nComboHeight = rectCombo.Height();
	*/

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("속성 표를 만들지 못했습니다. \n");
		return -1;      // 만들지 못했습니다.
	}

	InitPropList();
	RefreshPropValues();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* 잠금 */);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* 잠금 */);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// 모든 명령은 부모 프레임이 아닌 이 컨트롤을 통해 라우팅됩니다.
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* /* pCmdUI */)
{
}

void CPropertiesWnd::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnProperties1()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	int i = 0;
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}

void CPropertiesWnd::OnProperties2()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	int i = 0;
}

void CPropertiesWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}

void CPropertiesWnd::InitPropList()
{
	SetPropListFont();
	
	static bool bInit = false;
	if (bInit)
		return;

	m_PropMap.Clear();

	if (theApp.m_pMainWnd)
	{
		CFrameWnd *pFrame = ((CMainFrame*)theApp.m_pMainWnd)->GetActiveFrame();
		CMarioSpriteEditorDoc *pDoc = (CMarioSpriteEditorDoc*)pFrame->GetActiveDocument();
		if (pDoc)
		{
			m_PropMap.AddProperties(&pDoc->m_SpriteWork);

			/*
			BEGIN_VECTOR_LOOP(SpriteAnimationFrames*, pDoc->m_SpriteWork.m_vecAnimationFrames) {
				m_PropMap.AddProperty(
					pObj->m_strName,
					&pObj->m_strFrames,
					VT_ETC_NASTR,
					L"WorkInfo");
			} END_VECTOR_LOOP();
			*/

			bInit = true;
		}
	}

	m_PropMap.AddProperty(L"Left", nullptr, VT_I4, L"Sprite");
	m_PropMap.AddProperty(L"Top", nullptr, VT_I4, L"Sprite");
	m_PropMap.AddProperty(L"Right", nullptr, VT_I4, L"Sprite");
	m_PropMap.AddProperty(L"Bottom", nullptr, VT_I4, L"Sprite");
	m_PropMap.AddProperty(L"OffsetX", nullptr, VT_I4, L"Sprite");
	m_PropMap.AddProperty(L"OffsetY", nullptr, VT_I4, L"Sprite");

	m_PropMap.AddProperty(L"Width", nullptr, VT_I4, L"SpriteInfo", false);
	m_PropMap.AddProperty(L"Height", nullptr, VT_I4, L"SpriteInfo", false);

	m_PropMap.BindControl(&m_wndPropList);
	m_PropMap.InitControl();
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
	//m_wndObjectCombo.SetFont(&m_fntPropList);
}

void CPropertiesWnd::RefreshPropValues()
{
	if (!theApp.m_pMainWnd)
		return;

	CMarioSpriteEditorView *pView = (CMarioSpriteEditorView*)((CMainFrame*)theApp.m_pMainWnd)->GetActiveView();
	if (!pView)
		return;

	SpriteFrame *pFrame = pView->GetSelectedSpriteFrame();
	if (!pFrame)
		return;

	//SetRedraw(FALSE);

	InitPropList();
	m_PropMap.BindProperty(L"Left", (void*)&pFrame->m_rc.left);
	m_PropMap.BindProperty(L"Top", (void*)&pFrame->m_rc.top);
	m_PropMap.BindProperty(L"Right", (void*)&pFrame->m_rc.right);
	m_PropMap.BindProperty(L"Bottom", (void*)&pFrame->m_rc.bottom);
	m_PropMap.BindProperty(L"OffsetX", (void*)&pFrame->m_ptOffset.x);
	m_PropMap.BindProperty(L"OffsetY", (void*)&pFrame->m_ptOffset.y);

	VARIANT var;
	var.intVal = pFrame->m_rc.right - pFrame->m_rc.left;
	var.vt = VT_I4;
	m_PropMap.SetControlValue(L"Width", &var);

	var.intVal = pFrame->m_rc.bottom - pFrame->m_rc.top;
	m_PropMap.SetControlValue(L"Height", &var);

	m_PropMap.RefreshControl();
	m_wndPropList.ExpandAll();

	//SetRedraw(TRUE);
	//Invalidate();
}

LRESULT CPropertiesWnd::OnPropertyChange(WPARAM wParam, LPARAM lParam)
{
	CMFCPropertyGridProperty * pProp = (CMFCPropertyGridProperty *)lParam;
	int nID = pProp->GetData();

	NaDebugOut(L"PropertyChange: 0x%08x, %d\n", nID, lParam);
	
	if (!theApp.m_pMainWnd)
		return 0;

	CMarioSpriteEditorView *pView = (CMarioSpriteEditorView*)((CMainFrame*)theApp.m_pMainWnd)->GetActiveView();
	if (!pView)
		return 0;

	SpriteFrame *pFrame = pView->GetSelectedSpriteFrame();
	if (!pFrame)
		return 0;

	COleVariant var = pProp->GetValue();
	m_PropMap.SetPropertyByID(nID, var);
	
	int nIdx = pView->GetSelectedListIndex();
	pView->UpdateListItem(nIdx);
	pView->Invalidate();
	
	return LRESULT();
}

void CPropertiesWnd::SetProperty(LPCTSTR lpszName, COleVariant var)
{
	int nCnt = m_wndPropList.GetPropertyCount();
}
