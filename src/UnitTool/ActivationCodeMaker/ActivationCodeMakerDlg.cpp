
// ActivationCodeMakerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "ActivationCodeMaker.h"
#include "ActivationCodeMakerDlg.h"
#include "afxdialogex.h"

#include "ServerRequest.h"
#include "JsonManager.h"

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


// CActivationCodeMakerDlg 대화 상자



CActivationCodeMakerDlg::CActivationCodeMakerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ACTIVATIONCODEMAKER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CActivationCodeMakerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CActivationCodeMakerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_CHECK, &CActivationCodeMakerDlg::OnBnClickedBtnCheck)
END_MESSAGE_MAP()


// CActivationCodeMakerDlg 메시지 처리기

BOOL CActivationCodeMakerDlg::OnInitDialog()
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

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CActivationCodeMakerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CActivationCodeMakerDlg::OnPaint()
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
HCURSOR CActivationCodeMakerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CActivationCodeMakerDlg::OnBnClickedBtnCheck()
{
	CString strText;
	GetDlgItem(IDC_EDIT1)->GetWindowText(strText);
	NaString strNum = strText;
	if (strNum.GetLength() == 0)
	{
		MessageBox(L"Enter ticket number", L"Error", MB_OK);
		return;
	}

	int nTicketNumber = strNum.ToInt();
	if (nTicketNumber < 0)
	{
		MessageBox(L"Enter ticket number", L"Error", MB_OK);
		return;
	}

	GetDlgItem(IDC_EDIT2)->SetWindowTextW(L"");

	NaCurl curl;
	NaString strUrl = ServerRequest::s_strBaseUrl;
	strUrl += L"register.json";

	NaString strRet = curl.Post(strUrl);
	Json::Value jRet;
	Json::Reader jReader;
	if (!jReader.parse(strRet.cstr(), jRet))
	{
		MessageBox(L"Parse error.", L"Error", MB_OK);
		return;
	}

	for (auto jIt = jRet.begin(); jIt != jRet.end(); jIt++)
	{
		NaString strKey = jIt.key().toStyledString().c_str();
		NaString strUid = strKey.Mid(1);
		strUid = strUid.Left(strUid.GetLength() - 2);

		if (strUid.CompareNoCase(L"LastTicketNumber") == 0)
			continue;

		ServerRegisterTicket *pTicket = new ServerRegisterTicket;
		pTicket->BuildPropertyMap();

		try
		{
			JsonManager::LoadJsonObjectProperty(pTicket, *jIt);
		}
		catch (Json::LogicError &e)
		{
			NaDebugOut(L"Error: Bad ticket: %d\n", pTicket->m_nTicketNumber);
			delete pTicket;
			continue;
		}

		NaString strTicketHash = pTicket->m_strTicketHash = strUid;
		if (pTicket->m_nTicketNumber == nTicketNumber)
		{
			NaString strRightCode = L"";
			int nTemp = 0;
			for (int i = 0; i < strTicketHash.GetLength(); i++)
			{
				NaDebugOut(L"%c / %d\n", strTicketHash[i], strTicketHash[i]);
				if (i % 5 == 0)
					nTemp += (strTicketHash[i]);
				if (i % 5 == 1)
					nTemp += (strTicketHash[i]) / 2;
				if (i % 5 == 2)
					nTemp += (strTicketHash[i]) / 4;
				if (i % 5 == 3)
					nTemp += (strTicketHash[i]) / 5;
				if (i % 5 == 4)
				{
					nTemp += (strTicketHash[i]) / 3;
					if (nTemp > 255)
						nTemp = nTemp % 255;

					NaString strTemp;
					strTemp.Format(L"%X", nTemp);
					strRightCode += strTemp;

					nTemp = 0;
				}
			}

			GetDlgItem(IDC_EDIT2)->SetWindowTextW(strRightCode.wstr());

			NaString strJson = (*jIt).toStyledString().c_str();
			strJson.ReplaceAll(L",", L",\r\n");

			GetDlgItem(IDC_EDIT3)->SetWindowTextW(strJson.wstr());
		}
	}
}
