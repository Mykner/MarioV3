
// MarioConfigDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "MarioConfig.h"
#include "MarioConfigDlg.h"
#include "afxdialogex.h"

#include "GameConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMarioConfigDlg 대화 상자



CMarioConfigDlg::CMarioConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MARIOCONFIG_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMarioConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CBO_RENDER_ENGINE, m_cboRenderEngine);
	DDX_Control(pDX, IDC_CHK_SOFTWARE_FRAMESKIP, m_chkSoftwareFrameskip);
	DDX_Control(pDX, IDC_CBO_MAGNIFICATION, m_cboWindowSize);
}

BEGIN_MESSAGE_MAP(CMarioConfigDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMarioConfigDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_UP, &CMarioConfigDlg::OnBnClickedBtnUp)
	ON_BN_CLICKED(IDC_BTN_LEFT, &CMarioConfigDlg::OnBnClickedBtnLeft)
	ON_BN_CLICKED(IDC_BTN_RIGHT, &CMarioConfigDlg::OnBnClickedBtnRight)
	ON_BN_CLICKED(IDC_BTN_DOWN, &CMarioConfigDlg::OnBnClickedBtnDown)
	ON_BN_CLICKED(IDC_BTN_ACCEL, &CMarioConfigDlg::OnBnClickedBtnAccel)
	ON_BN_CLICKED(IDC_BTN_JUMP, &CMarioConfigDlg::OnBnClickedBtnJump)
	ON_BN_CLICKED(IDC_BTN_INPUT_RESET, &CMarioConfigDlg::OnBnClickedBtnInputReset)
	ON_WM_INPUT()
END_MESSAGE_MAP()


// CMarioConfigDlg 메시지 처리기

BOOL CMarioConfigDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// Load Config
	m_cboRenderEngine.SetCurSel(0);
	GameConfig *pConfig = GameConfig::GetGameConfig(nullptr);

	if (pConfig->m_strRenderEngine.Compare(L"DirectX") == 0)
		m_cboRenderEngine.SetCurSel(1);

	if (pConfig->m_bSoftwareFrameSkip)
		m_chkSoftwareFrameskip.SetCheck(true);

	if (pConfig->m_strInputKeyCode.GetLength() == 0)
		pConfig->m_strInputKeyCode = L"37,39,38,40,88,90";
	NaStrArray ar = pConfig->m_strInputKeyCode.Split(L",");
	if (ar.GetCount() < 6)
	{
		pConfig->m_strInputKeyCode = L"37,39,38,40,88,90";
		ar = pConfig->m_strInputKeyCode.Split(L",");
	}

	m_InputInfo[0] = { L"Left",		ar[0].ToInt(), IDC_BTN_LEFT };
	m_InputInfo[1] = { L"Right",	ar[1].ToInt(), IDC_BTN_RIGHT };
	m_InputInfo[2] = { L"Up",		ar[2].ToInt(), IDC_BTN_UP };
	m_InputInfo[3] = { L"Down",		ar[3].ToInt(), IDC_BTN_DOWN };
	m_InputInfo[4] = { L"Jump",		ar[4].ToInt(), IDC_BTN_JUMP };
	m_InputInfo[5] = { L"Accel",	ar[5].ToInt(), IDC_BTN_ACCEL };
	
	UpdateInputButtons();

	m_bCaptureInput = false;
	m_nCaptureInputIndex = -1;

	m_cboWindowSize.SetCurSel(pConfig->m_nZoomFactor - 1);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMarioConfigDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMarioConfigDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMarioConfigDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMarioConfigDlg::OnBnClickedOk()
{
	CDialogEx::OnOK();

	GameConfig *pConfig = GameConfig::GetGameConfig(nullptr);

	// RenderEngine
	if (m_cboRenderEngine.GetCurSel() == 0)
		pConfig->m_strRenderEngine = L"OpenGL";
	else
		pConfig->m_strRenderEngine = L"DirectX";

	// FrameSkip
	if (m_chkSoftwareFrameskip.GetCheck())
		pConfig->m_bSoftwareFrameSkip = true;
	else
		pConfig->m_bSoftwareFrameSkip = false;

	// Input
	NaString str = L"";
	for (int i=0; i<6; i++)
	{
		if (str.GetLength() == 0)
			str.Format(L"%d", m_InputInfo[i].nKeyCode);
		else
		{
			str += L",";

			NaString strTemp;
			strTemp.Format(L"%d", m_InputInfo[i].nKeyCode);
			str += strTemp;
		}
	}
	pConfig->m_strInputKeyCode = str;
	pConfig->m_nZoomFactor = m_cboWindowSize.GetCurSel() + 1;

	// #CRITICAL Avast catches pConfig->Save() as Virus
	pConfig->Save();
}


void CMarioConfigDlg::OnBnClickedBtnUp()
{
	EditInput(INPUT_U);
}

void CMarioConfigDlg::OnBnClickedBtnLeft()
{
	EditInput(INPUT_L);
}

void CMarioConfigDlg::OnBnClickedBtnRight()
{
	EditInput(INPUT_R);
}

void CMarioConfigDlg::OnBnClickedBtnDown()
{
	EditInput(INPUT_D);
}

void CMarioConfigDlg::OnBnClickedBtnAccel()
{
	EditInput(INPUT_F);
}

void CMarioConfigDlg::OnBnClickedBtnJump()
{
	EditInput(INPUT_J);
}

void CMarioConfigDlg::OnBnClickedBtnInputReset()
{
	int nDefaultKeyCode[6] = {
		37,39,38,40,88,90
	};

	for (int i=0; i<6; i++)
	{
		m_InputInfo[i].nKeyCode = nDefaultKeyCode[i];
	}

	UpdateInputButtons();
}

void CMarioConfigDlg::RemoveDuplicatedInput(int nNewKeyCode)
{
	for (int i=0; i<6; i++)
	{
		if (m_InputInfo[i].nKeyCode == nNewKeyCode)
		{
			m_InputInfo[i].nKeyCode = -1;
		
			CString str;
			str.Format(L"%s: undefined", m_InputInfo[i].strInputName);
			GetDlgItem(m_InputInfo[i].nButtonID)->SetWindowText(str);
		}
	}
}

void CMarioConfigDlg::EditInput(int nInput)
{
	if (m_bCaptureInput)
		StopCaptureInput();

	m_InputInfo[nInput].nKeyCode = -1;
	UpdateInputButton(nInput);

	m_nCaptureInputIndex = nInput;
	StartCaptureInput();
}

void CMarioConfigDlg::StartCaptureInput()
{
	if (m_bCaptureInput)
		return;

	// Capture Input
	RAWINPUTDEVICE rawInputDev[1];
	ZeroMemory(rawInputDev, 1 * sizeof(RAWINPUTDEVICE));

	// key
	rawInputDev[0].usUsagePage = 0x01;
	rawInputDev[0].usUsage = 0x06;
	rawInputDev[0].dwFlags = RIDEV_INPUTSINK;
	rawInputDev[0].hwndTarget = GetSafeHwnd();


	if (RegisterRawInputDevices(rawInputDev, 1, sizeof(RAWINPUTDEVICE)) == FALSE)
	{
		// Error.
		MessageBox(L"Capturing input failed :(");
		return;
	}

	m_bCaptureInput = true;
}

void CMarioConfigDlg::StopCaptureInput()
{
	if (!m_bCaptureInput)
		return;

	// Stop capture Input
	RAWINPUTDEVICE rawInputDev[1];
	ZeroMemory(rawInputDev, 1 * sizeof(RAWINPUTDEVICE));

	// key
	rawInputDev[0].usUsagePage = 0x01;
	rawInputDev[0].usUsage = 0x06;
	rawInputDev[0].dwFlags = RIDEV_REMOVE;
	rawInputDev[0].hwndTarget = 0;

	RegisterRawInputDevices(rawInputDev, 1, sizeof(RAWINPUTDEVICE));

	m_bCaptureInput = false;
}

void CMarioConfigDlg::OnRawInput(UINT nInputcode, HRAWINPUT hRawInput)
{
	RAWINPUT RawInput;
	unsigned int unSzRawInput = sizeof(RAWINPUT);
	GetRawInputData(hRawInput, RID_INPUT, &RawInput, &unSzRawInput, sizeof(RAWINPUTHEADER));

	RAWKEYBOARD &rk = RawInput.data.keyboard;

	// Raw input to struct
	int nNewKeyCode = rk.VKey;
	StopCaptureInput();

	/*
	switch (nNewKeyCode)
	{
	case VK_RETURN:
	case VK_ESCAPE:
	case VK_SPACE:
		MessageBox(L"You cannot use system key. (Enter, Escape, Space) :p");
		return;
		break;
	}
	*/

	RemoveDuplicatedInput(nNewKeyCode);
	m_InputInfo[m_nCaptureInputIndex].nKeyCode = nNewKeyCode;

	UpdateInputButton(m_nCaptureInputIndex);
}

void CMarioConfigDlg::UpdateInputButtons()
{
	for (int i = 0; i < 6; i++)
		UpdateInputButton(i);
}

void CMarioConfigDlg::UpdateInputButton(int nIndex)
{
	int nKeyCode = m_InputInfo[nIndex].nKeyCode;
	if (nKeyCode >= 0)
	{
		unsigned int nScanCode = MapVirtualKey(nKeyCode, MAPVK_VK_TO_VSC);

		// because MapVirtualKey strips the extended bit for some keys
		switch (nKeyCode)
		{
		case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN:
		case VK_PRIOR: case VK_NEXT:
		case VK_END: case VK_HOME:
		case VK_INSERT: case VK_DELETE:
		case VK_DIVIDE:
		case VK_NUMLOCK:
			nScanCode |= 0x100; // set extended bit
			break;
		}

		wchar_t keyName[50];
		if (GetKeyNameText(nScanCode << 16, keyName, sizeof(keyName)) == 0)
		{
			// error
			MessageBox(L"Cannot recognize key :(");
			return;
		}

		CString str, strKeyName;
		strKeyName.Format(L"%s", keyName);
		str.Format(L"%s: %s", m_InputInfo[nIndex].strInputName, strKeyName);
		GetDlgItem(m_InputInfo[nIndex].nButtonID)->SetWindowText(str);
	}
	else
	{
		CString str, strKeyName;
		str.Format(L"%s: ??", m_InputInfo[nIndex].strInputName);
		GetDlgItem(m_InputInfo[nIndex].nButtonID)->SetWindowText(str);
	}
}
