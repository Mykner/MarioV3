
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "MarioMapEditor.h"

#include "MainFrm.h"

#include "../NaLib/NaCommon.h"

#include "MarioMapEditorDoc.h"
#include "MarioMapEditorView.h"

#include "Game.h"
#include "Stage.h"
#include "DesignStage.h"

#include "PathManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_WM_SETTINGCHANGE()
	ON_UPDATE_COMMAND_UI(ID_FILE_UPLOAD, &CMainFrame::OnUpdateUpload)
	// Toolbars
	ON_COMMAND_RANGE(ID_BTN__________________________BLOCKS_START, ID_BTN__________________________VEHICLES_END, &CMainFrame::OnToolBars)
	ON_UPDATE_COMMAND_UI_RANGE(ID_BTN__________________________BLOCKS_START, ID_BTN__________________________VEHICLES_END, &CMainFrame::OnUpdateToolBars)
	ON_COMMAND(ID_BTN_TOGGLE_PLAYEDSHADOW, &CMainFrame::OnTogglePlayedShadow)
	ON_UPDATE_COMMAND_UI(ID_BTN_TOGGLE_PLAYEDSHADOW, &CMainFrame::OnUpdateTogglePlayedShadow)
	// Design Play/Stop
	ON_COMMAND(ID_BTN_PLAY, &CMainFrame::OnPlay)
	ON_COMMAND(ID_BTN_STOP, &CMainFrame::OnStop)
	ON_COMMAND(ID_BTN_PLAYONGAME, &CMainFrame::OnPlayOnGame)
	ON_UPDATE_COMMAND_UI(ID_BTN_PLAY, &CMainFrame::OnUpdatePlay)
	ON_UPDATE_COMMAND_UI(ID_BTN_STOP, &CMainFrame::OnUpdateStop)
	ON_UPDATE_COMMAND_UI(ID_BTN_PLAYONGAME, &CMainFrame::OnUpdatePlayOnGame)
	ON_WM_CLOSE()
	// Tab Change
	ON_REGISTERED_MESSAGE(AFX_WM_CHANGING_ACTIVE_TAB, &CMainFrame::OnChangingActiveTab)
	ON_REGISTERED_MESSAGE(AFX_WM_CHANGE_ACTIVE_TAB, &CMainFrame::OnChangeActiveTab)
	// StatusBar
	ON_COMMAND_RANGE(ID_INDICATOR_TYPENAME, ID_INDICATOR_EDITMODE, &CMainFrame::OnStatusBar)
	ON_UPDATE_COMMAND_UI_RANGE(ID_INDICATOR_TYPENAME, ID_INDICATOR_EDITMODE, &CMainFrame::OnUpdateStatusBar)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_EDITMODE,
	ID_INDICATOR_TYPENAME,
	ID_INDICATOR_CURSOR,
	//ID_INDICATOR_POSITION,
	//ID_INDICATOR_SIZE,
	//ID_INDICATOR_CAMERAPOS,

	/*
	// Don't need
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
	*/
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);

	m_nCurTool = -1;
	m_pActiveView = nullptr;
	m_pDesignPlayThread = nullptr;
	m_pResourceHolder = nullptr;

	m_bOnClosing = false;
}

CMainFrame::~CMainFrame()
{
	if (m_pResourceHolder)
	{
		delete m_pResourceHolder;
		m_pResourceHolder = nullptr;
	}
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // 사용할 수 있는 다른 스타일...
	mdiTabParams.m_bActiveTabCloseButton = TRUE;      // FALSE로 설정하여 탭 영역 오른쪽에 닫기 단추를 배치합니다.
	mdiTabParams.m_bTabIcons = FALSE;    // TRUE로 설정하여 MDI 탭의 문서 아이콘을 활성화합니다.
	mdiTabParams.m_bAutoColor = TRUE;    // FALSE로 설정하여 MDI 탭의 자동 색 지정을 비활성화합니다.
	mdiTabParams.m_bDocumentMenu = TRUE; // 탭 영역의 오른쪽 가장자리에 문서 메뉴를 활성화합니다.
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("메뉴 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// 메뉴 모음을 활성화해도 포커스가 이동하지 않게 합니다.
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// 사용자 정의 도구 모음 작업을 허용합니다.
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);
	
	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
	//m_wndStatusBar.SetPaneInfo(0, 0, ~SBPS_STRETCH, 0);

	// TODO: 도구 모음 및 메뉴 모음을 도킹할 수 없게 하려면 이 다섯 줄을 삭제하십시오.
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);

	InitToolbars();

	// Visual Studio 2005 스타일 도킹 창 동작을 활성화합니다.
	CDockingManager::SetDockingMode(DT_SMART);
	// Visual Studio 2005 스타일 도킹 창 자동 숨김 동작을 활성화합니다.
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 메뉴 항목 이미지를 로드합니다(표준 도구 모음에 없음).
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// 도킹 창을 만듭니다.
	if (!CreateDockingWindows())
	{
		TRACE0("도킹 창을 만들지 못했습니다.\n");
		return -1;
	}

	if (m_wndFileView.m_hWnd)
	{
		m_wndFileView.EnableDocking(CBRS_ALIGN_ANY);
		DockPane(&m_wndFileView);

		CDockablePane* pTabbedBar = NULL;
		m_wndClassView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
	}
	if (m_wndClassView.m_hWnd)
		m_wndClassView.EnableDocking(CBRS_ALIGN_ANY);

	if (m_wndOutput.m_hWnd)
	{
		m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
		DockPane(&m_wndOutput);
	}

	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProperties);

	// 보관된 값에 따라 비주얼 관리자 및 스타일을 설정합니다.
	OnApplicationLook(theApp.m_nAppLook);

	// 향상된 창 관리 대화 상자를 활성화합니다.
	EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

	// 도구 모음 및 도킹 창 메뉴 바꾸기를 활성화합니다.
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// 빠른(<Alt> 키를 누른 채 끌기) 도구 모음 사용자 지정을 활성화합니다.
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == NULL)
	{
		// 사용자 정의 도구 모음 이미지를 로드합니다.
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	// 메뉴 개인 설정을 활성화합니다(가장 최근에 사용한 명령).
	// TODO: 사용자의 기본 명령을 정의하여 각 풀다운 메뉴에 하나 이상의 기본 명령을 포함시킵니다.
	/*
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_FILE_PRINT);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_WINDOWS_7);
	lstBasicCommands.AddTail(ID_SORTING_SORTALPHABETIC);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYTYPE);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYACCESS);
	lstBasicCommands.AddTail(ID_SORTING_GROUPBYTYPE);

	CMFCToolBar::SetBasicCommands(lstBasicCommands);
	*/

	// 창 제목 표시줄에서 문서 이름 및 응용 프로그램 이름의 순서를 전환합니다.
	// 문서 이름이 축소판 그림과 함께 표시되므로 작업 표시줄의 기능성이 개선됩니다.
	ModifyStyle(0, FWS_PREFIXTITLE);

	// for Hold Resource 
	m_pResourceHolder = new ResourceHolder;
	m_pResourceHolder->m_bUseDirectX = true;

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// Not use yet
	/*
	// 클래스 뷰를 만듭니다.
	CString strClassView;
	bNameValid = strClassView.LoadString(IDS_CLASS_VIEW);
	ASSERT(bNameValid);
	if (!m_wndClassView.Create(strClassView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_CLASSVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("클래스 뷰 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}

	// 파일 뷰를 만듭니다.
	CString strFileView;
	bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
	ASSERT(bNameValid);
	if (!m_wndFileView.Create(strFileView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	{
		TRACE0("파일 뷰 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}

	// 출력 창을 만듭니다.
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("출력 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}
	*/

	// 속성 창을 만듭니다.
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("속성 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	if (m_wndFileView.m_hWnd)
	{
		HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
		m_wndFileView.SetIcon(hFileViewIcon, FALSE);
	}

	if (m_wndClassView.m_hWnd)
	{
		HICON hClassViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_CLASS_VIEW_HC : IDI_CLASS_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
		m_wndClassView.SetIcon(hClassViewIcon, FALSE);
	}

	if (m_wndOutput.m_hWnd)
	{
		HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
		m_wndOutput.SetIcon(hOutputBarIcon, FALSE);
	}

	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

	UpdateMDITabbedBarsIcons();
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 메뉴를 검색합니다. */);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CMDIFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	if (m_wndOutput.m_hWnd)
		m_wndOutput.UpdateFonts();
	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}


BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// 기본 클래스가 실제 작업을 수행합니다.
	if (!CMDIFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}

	// 모든 사용자 도구 모음에 사용자 지정 단추를 활성화합니다.
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}


void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CMDIFrameWndEx::OnSettingChange(uFlags, lpszSection);
	//m_wndOutput.UpdateFonts();
}


BOOL CMainFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	return CMDIFrameWndEx::OnNotify(wParam, lParam, pResult);
}

LRESULT CMainFrame::OnChangingActiveTab(WPARAM wparam, LPARAM lparam)
{
	if (m_pResourceHolder == nullptr) // after OnClose
		return 0;

	CFrameWnd *pFrame = GetActiveFrame();
	CMarioMapEditorView *pView = (CMarioMapEditorView*)pFrame->GetActiveView();
	if (pView)
	{
		CString str = pView->GetDocument()->GetTitle();

		if (pView->IsPlaying())
		{
			pView->StopPlaying();
		}
	}

	// Wait until stop playing thread
	if (m_pDesignPlayThread)
		WaitForSingleObject(m_pDesignPlayThread->m_hThread, INFINITE);

	return 0;
}

LRESULT CMainFrame::OnChangeActiveTab(WPARAM wparam, LPARAM lparam)
{
	if (m_pResourceHolder == nullptr) // after OnClose
		return 0;

	CFrameWnd *pFrame = GetActiveFrame();
	CMarioMapEditorView *pView = (CMarioMapEditorView*)pFrame->GetActiveView();
	if (pView)
	{
		m_pActiveView = pView;
		pView->OnActivateView();

		Invalidate();
	}

	return 0;
}

int CMainFrame::InitToolbars()
{
#define INIT_TOOLBAR(var, idr, _id) \
	while (true) { \
	if (!var.LoadToolBar(idr)) \
	{ \
		TRACE("Fail to LoadToolbar %s\n", #var); \
		break; \
	} \
	if (!var.CreateEx( \
		this, \
		TBSTYLE_FLAT, \
		WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, \
		CRect(1,1,1,1), \
		_id)) \
	{ \
		TRACE("Fail to CreateEx Toolbar %s\n", #var); \
		break; \
	} \
	::SetWindowText(var.m_hWnd, NaString(#var).wstr()); \
	var.EnableDocking(CBRS_ALIGN_ANY); \
	DockPane(&var); \
	break; \
	}

	INIT_TOOLBAR(m_wndToolBarBlocks, IDR_TOOLBAR_BLOCKS, (AFX_IDW_TOOLBAR + 7)); 
	INIT_TOOLBAR(m_wndToolBarItems, IDR_TOOLBAR_ITEMS, (AFX_IDW_TOOLBAR + 8));
	INIT_TOOLBAR(m_wndToolBarMapObjects, IDR_TOOLBAR_MAPOBJECTS, (AFX_IDW_TOOLBAR + 9));
	INIT_TOOLBAR(m_wndToolBarEnemies, IDR_TOOLBAR_ENEMIES, (AFX_IDW_TOOLBAR + 10));
	INIT_TOOLBAR(m_wndToolBarEventObjects, IDR_TOOLBAR_EVENTOBJECTS, (AFX_IDW_TOOLBAR + 11));
	INIT_TOOLBAR(m_wndToolBarVehicles, IDR_TOOLBAR_VEHICLES, (AFX_IDW_TOOLBAR + 12));

	return 0;
}

void CMainFrame::OnUpdateToolBars(CCmdUI * pCmdUI)
{
	switch (pCmdUI->m_nID)
	{
	case ID_BTN_OBJ_LOOPFLAG:
	case ID_BTN_OBJ_LOOPHANDLER:
	case ID_BTN_OBJ_BOOMBOOM:
	case ID_BTN_OBJ_LEAF:
    case ID_BTN_OBJ_VINE:
		pCmdUI->Enable(FALSE);
		return;
		break;
	}

	CFrameWnd *pFrame = GetActiveFrame();
	CMarioMapEditorView *pView = (CMarioMapEditorView*)pFrame->GetActiveView();
	if (pView)
	{
        pCmdUI->Enable(!pView->IsPlaying());

		int nCurTool = (pView)->GetCurToolID();
		pCmdUI->SetCheck(pCmdUI->m_nID == nCurTool);
	}
    else
    {
        pCmdUI->Enable(FALSE);
    }
}

void CMainFrame::OnTogglePlayedShadow()
{
	CFrameWnd *pFrame = GetActiveFrame();
	CMarioMapEditorView *pView = (CMarioMapEditorView*)pFrame->GetActiveView();
	if (pView)
	{
		DesignStage *pStage = pView->GetStage();
		pStage->m_bShowPlayedShadow = !pStage->m_bShowPlayedShadow;

		pView->InvalidateGame();
	}
}

void CMainFrame::OnUpdateTogglePlayedShadow(CCmdUI * pCmdUI)
{
	CFrameWnd *pFrame = GetActiveFrame();
	CMarioMapEditorView *pView = (CMarioMapEditorView*)pFrame->GetActiveView();
	if (pView)
	{
		DesignStage *pStage = pView->GetStage();
		pCmdUI->SetCheck(pStage->m_bShowPlayedShadow);
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->SetCheck(FALSE);
		pCmdUI->Enable(FALSE);
	}
}

void CMainFrame::SetStatusText(int nID, LPCTSTR lpszNewText, BOOL bUpdate)
{
	int nIndex = -1;
	int nCnt = sizeof(indicators) / sizeof(UINT);
	for (int i=0; i<nCnt; i++)
	{
		if (indicators[i] == nID)
		{
			nIndex = i;
			break;
		}
	}

	if (nIndex == -1)
		return;
	
	m_wndStatusBar.SetPaneText(nIndex, lpszNewText, bUpdate);
	m_wndStatusBar.SetPaneStyle(nIndex, ~SBPS_DISABLED);
}

void CMainFrame::OnToolBars(UINT id)
{
	CFrameWnd *pFrame = GetActiveFrame();
	CMarioMapEditorView *pView = (CMarioMapEditorView*)pFrame->GetActiveView();
	if (pView)
	{
		pView->SetCurTool(id);
	}
}

void CMainFrame::OnClose()
{
	m_bOnClosing = true;

	CFrameWnd *pFrame = GetActiveFrame();
	CMarioMapEditorView *pView = (CMarioMapEditorView*)pFrame->GetActiveView();
	if (pView && pView->IsPlaying())
	{
		pView->StopPlaying();
	}

	// Wait until stop playing thread
	if (m_pDesignPlayThread)
		WaitForSingleObject(m_pDesignPlayThread->m_hThread, INFINITE);

	CMDIFrameWndEx::OnClose();
}

void CMainFrame::OnPlay()
{
	CFrameWnd *pFrame = GetActiveFrame();
	CMarioMapEditorView *pView = (CMarioMapEditorView*)pFrame->GetActiveView();
	if (pView && !pView->IsPlaying())
	{
		pView->DoPlaying(-1, -1);
	}
}

void CMainFrame::OnStop()
{
	CFrameWnd *pFrame = GetActiveFrame();
	CMarioMapEditorView *pView = (CMarioMapEditorView*)pFrame->GetActiveView();
	if (pView && pView->IsPlaying())
	{
		pView->StopPlaying();
	}
}

void CMainFrame::OnPlayOnGame()
{
	CFrameWnd *pFrame = GetActiveFrame();
	CMarioMapEditorView *pView = (CMarioMapEditorView*)pFrame->GetActiveView();
	if (pView)
	{
		auto pDoc = pView->GetDocument();
		if (pDoc->IsModified() || pDoc->m_strPathName.GetLength() == 0)
		{
			if (MessageBox(L"You must save before test. proceed?", L"Confirm", MB_OKCANCEL) == IDCANCEL)
				return;

			if (pDoc->m_strPathName.GetLength() == 0)
				pDoc->DoSave(NULL);
			else
				pDoc->DoFileSave();
		}

		if (pView->IsPlaying())
			pView->StopPlaying();

		/*
		auto pGame = pView->GetGame();
		auto pStage = pView->GetStage();
		if (pStage->m_strUniqueId.GetLength() > 0)
		{
			MessageBox(L"You cannot do test Downloaded stage.", L"Error", MB_OK | MB_ICONERROR);
			return;
		}
		*/

		wchar_t szFileName[1024];
		HMODULE hModule = GetModuleHandle(NULL);
		GetModuleFileName(hModule, szFileName, sizeof(szFileName));

		NaString strExePath = szFileName;
		auto ar = strExePath.Split(L"\\");
		ar.Pop();
		strExePath = ar.Join(L"\\");
		strExePath += L"\\Mario.exe";

		NaString strArguments = L"-t ";
		strArguments += pView->GetDocument()->m_strPathName;

		ShellExecute(
			nullptr,
			L"open",
			strExePath.wstr(),
			strArguments.wstr(),
			nullptr,
			SW_SHOW
		);
	}
}

void CMainFrame::OnUpdatePlay(CCmdUI * pCmdUI)
{
	CFrameWnd *pFrame = GetActiveFrame();
	CMarioMapEditorView *pView = (CMarioMapEditorView*)pFrame->GetActiveView();
	if (pView)
		pCmdUI->Enable(!pView->IsPlaying());
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnUpdateStop(CCmdUI * pCmdUI)
{
	CFrameWnd *pFrame = GetActiveFrame();
	CMarioMapEditorView *pView = (CMarioMapEditorView*)pFrame->GetActiveView();
	if (pView)
		pCmdUI->Enable(pView->IsPlaying());
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnUpdatePlayOnGame(CCmdUI * pCmdUI)
{
	CFrameWnd *pFrame = GetActiveFrame();
	CMarioMapEditorView *pView = (CMarioMapEditorView*)pFrame->GetActiveView();
	if (pView)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnUpdateUpload(CCmdUI * pCmdUI)
{
	CFrameWnd *pFrame = GetActiveFrame();
	CMarioMapEditorView *pView = (CMarioMapEditorView*)pFrame->GetActiveView();
	if (pView)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnStatusBar(UINT id)
{
	// Nothing.
}

void CMainFrame::OnUpdateStatusBar(CCmdUI * pCmdUI)
{
	pCmdUI->Enable(TRUE);
}
