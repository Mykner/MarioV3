//
// MarioApp 
// 2016.06.26 neoarc
//

#include "stdafx.h"
#include "MarioApp.h"

#include "InputManager.h"
#include <time.h>
#include "NaPlatformAPI.h"

#include "GameConfig.h"
#include "NaException.h"

// #ifdef _DEBUG
// #define new new(_CLIENT_BLOCK, __FILE__, __LINE__)
// #endif

// Global Variables
Game *g_pGame = nullptr;

int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // For catch memory leak
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_crtBreakAlloc = 4611;

	// Init random seed
	srand(time(nullptr));

#if !defined(_RELEASE)
	// Minidump Handler
	::SetUnhandledExceptionFilter(CrashHandler);
#endif

	NaString strCurrentDir = NaPlatform::GetCurrentPath();

	Game *pGame = new Game();
	g_pGame = pGame;

	// Must do this before Init Environment
	GameConfig *pConfig = GameConfig::GetGameConfig(pGame);
	pConfig->ApplyOptions(pGame);

	NaString strArg = lpCmdLine;
	if (strArg.GetLength() > 0)
	{
		NaStrArray ar = strArg.Split(L" ");
		for (int i = 0; i < ar.GetCount(); i++)
		{
			NaString strArgTok = ar[i];
			if (strArgTok[0] == L'-')
			{
				NaString strOptionName = strArgTok.Mid(1);
				pGame->m_Option.ParseOption(strOptionName);
			}
			else
			{
				strArgTok.ReplaceAll(L"\"", L"");
				for (int j = i + 1; j < ar.GetCount(); j++)
				{
					strArgTok += L" ";
					strArgTok += ar[j];
				}

				strCurrentDir = L"";
				NaStrArray arPath = strArgTok.Split(L"\\");
				int len = arPath.GetCount();
				for (int i = 0; i < len; i++)
				{
					if (arPath[i] == L"Resources")
					{
						if (len - i == 3)
						{
							pGame->m_strStagePack = L".";
							pGame->m_nStagePackType = STAGEPACK_SINGLE;
						}
						else if (len - i == 4)
						{
							pGame->m_strStagePack = arPath[i + 2];
							if (pGame->m_strStagePack.Compare(L"Classic") == 0)
								pGame->m_nStagePackType = STAGEPACK_CLASSIC;
							else
								pGame->m_nStagePackType = STAGEPACK_CUSTOM;
						}
						break;
					}
					strCurrentDir += arPath[i];
					strCurrentDir += L"\\";
				}

				strCurrentDir = strCurrentDir.Left(strCurrentDir.GetLength() - 1);
				SetCurrentDirectory(strCurrentDir.wstr());

				int idx = arPath[arPath.GetCount() - 1].Find(L".map");
				NaString strStage = arPath[arPath.GetCount() - 1].Left(idx);

				pGame->SetInitialStage(strStage);
				break;
			}
		}
	}

    // Init Environment
	if (!InitEnvironment(hInstance, pGame, strCurrentDir.wstr()))
	{
		return FALSE;
	}

	g_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);

    HACCEL hAccelTable = LoadAccelerators(g_hInstance, MAKEINTRESOURCE(IDC_MARIO));

	MSG msg = {};
    while (msg.message != WM_QUIT)
    {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		pGame->ProcessGameLoop();
    }

	GameConfig::Release();

	pGame->Release();
	delete pGame;
	
	UnhookWindowsHookEx(g_hKeyboardHook);

    return (int) msg.wParam;
}

bool InitEnvironment(HINSTANCE hInstance, Game *pGame, const wchar_t* szCurrentDir)
{
	g_hInstance = hInstance;

	WCHAR szTitle[MAX_LOADSTRING];
	WCHAR szWindowClass[MAX_LOADSTRING];
	LoadStringW(g_hInstance, IDS_MARIO_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(g_hInstance, IDC_MARIO_APP, szWindowClass, MAX_LOADSTRING);

	// Register Class
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MARIO));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	//wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MARIO);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	ATOM ret = RegisterClassExW(&wcex);
	if (!ret)
		return false;

	// Create main window
	RECT rcWindow = { 0, 0, pGame->Width(), pGame->Height() };
	AdjustWindowRect(&rcWindow, WS_OVERLAPPEDWINDOW, FALSE);

	HWND hWnd = CreateWindowW(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		0, // x
		0, // y
		rcWindow.right - rcWindow.left, // width
		rcWindow.bottom - rcWindow.top, // height
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	if (!hWnd)
		return false;

	ShowWindow(hWnd, SW_HIDE);
	UpdateWindow(hWnd);

	try
	{
		pGame->Init(hWnd, szCurrentDir);
	}
	catch (NaException& e)
	{
		//::MessageBox(nullptr, NaString(e.m_szWhat).wstr(), L"Exception!", MB_OK | MB_ICONERROR);

		NaString strPath = NaPlatform::GetCurrentPath();

		NaString strFileName;
		SYSTEMTIME t;
		::GetSystemTime(&t);
		strFileName.Format(L"\\Mario_Exception_%04d%02d%02d_%02d%02d%02d.log",
			t.wYear, t.wMonth, t.wDay,
			t.wHour, t.wMinute, t.wSecond
		);
		strPath += strFileName;

		FILE *fp;
		NaPlatform::FileOpen(&fp, strPath.cstr(), "wb");
		if (fp)
		{
			NaString strErr;
			strErr.Format(L"%ls(%d): %ls",
				e.m_szFunc, e.m_nLine, NaString(e.m_szWhat).wstr()
				);
			fwrite(strErr.cstr(), strErr.GetLength(), 1, fp);
			fclose(fp);
		}

		::DestroyWindow(hWnd);
		return false;
	}

	return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case IDM_ABOUT:
				// Do not need
                //DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_ERASEBKGND:
    case WM_PAINT:
        {
			// Using DirectX
            //PAINTSTRUCT ps;
            //HDC hdc = BeginPaint(hWnd, &ps);
            //EndPaint(hWnd, &ps);
        }
        break;
	case WM_CLOSE:
		{
			g_pGame->OnExit();

			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
	case WM_ACTIVATEAPP:
		// g_bWindowActive is used to control if the Windows key is filtered by the keyboard hook or not.
		if (wParam == TRUE)
			g_bWindowActive = true;
		else
			g_bWindowActive = false;
		break;
	case WM_MOUSEMOVE:
		if (g_pGame && g_pGame->m_pInputManager)
		{
			POINT pt = { LOWORD(lParam), HIWORD(lParam) };
			HWND hWnd = g_pGame->m_hWindow;

			NaRect rcClient;
			GetClientRect(hWnd, (LPRECT)&rcClient);

			float fScaleX = (float)rcClient.Width() / (float)(g_pGame->m_nWidth);
			float fScaleY = (float)rcClient.Height() / (float)(g_pGame->m_nHeight);

			POINT ptNew = { pt.x / fScaleX, pt.y / fScaleY };

			g_pGame->m_pInputManager->m_ptMouse = ptNew;
		}
		break;
	case WM_LBUTTONDOWN:
		if (g_pGame && g_pGame->m_pInputManager)
		{
			POINT pt = { LOWORD(lParam), HIWORD(lParam) };
			HWND hWnd = g_pGame->m_hWindow;
			
			NaRect rcClient;
			GetClientRect(hWnd, (LPRECT)&rcClient);

			float fScaleX = (float)rcClient.Width() / (float)(g_pGame->m_nWidth);
			float fScaleY = (float)rcClient.Height() / (float)(g_pGame->m_nHeight);

			POINT ptNew = { pt.x / fScaleX, pt.y / fScaleY };

			g_pGame->m_pInputManager->m_ptLButtonDown = ptNew;
		}
		break;
	case WM_LBUTTONUP:
		if (g_pGame && g_pGame->m_pInputManager)
		{
			POINT pt = { LOWORD(lParam), HIWORD(lParam) };
			HWND hWnd = g_pGame->m_hWindow;

			NaRect rcClient;
			GetClientRect(hWnd, (LPRECT)&rcClient);

			float fScaleX = (float)rcClient.Width() / (float)(g_pGame->m_nWidth);
			float fScaleY = (float)rcClient.Height() / (float)(g_pGame->m_nHeight);

			POINT ptNew = { pt.x / fScaleX, pt.y / fScaleY };

			g_pGame->m_pInputManager->m_ptLButtonUp = ptNew;
			g_pGame->m_pInputManager->OnClick();
		}
		break;
	case WM_MOUSEWHEEL:
		{
			short nValue = (short)HIWORD(wParam);
			nValue = nValue / WHEEL_DELTA;

			// nValue < 0 : Scroll Down
			// nValue > 0 : Scroll Up
			//NaDebugOut(L"MouseWheel: %d\n", nValue);
			g_pGame->m_pInputManager->m_nWheelMoved = nValue;
		}
		break;
	case WM_SIZE:
		if (g_pGame && g_pGame->m_pRenderManager)
		{
			g_pGame->m_pRenderManager->OnSize(LOWORD(lParam), HIWORD(lParam));
		}
		break;
	case WM_EXITSIZEMOVE:
		break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0 || nCode != HC_ACTION)  // do not process message 
		return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);

	bool bEatKeystroke = false;
	/*
	KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
	switch (wParam)
	{
	case WM_KEYDOWN:
	case WM_KEYUP:
		{
			bEatKeystroke = (
				g_bWindowActive && 
				((p->vkCode == VK_LWIN) || 
				 (p->vkCode == VK_RWIN) ||
				 (p->vkCode == VK_LMENU))
				);
			break;
		}
	}
	*/

	if (bEatKeystroke)
		return 1;
	else
		return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
}

LONG WINAPI CrashHandler(EXCEPTION_POINTERS * pException)
{
	//::MessageBox(0, L"Exception!", L"", MB_OK);

	NaString strPath = NaPlatform::GetCurrentPath();

	NaString strFileName;
	SYSTEMTIME t;
	::GetSystemTime(&t);
	strFileName.Format(L"\\Mario_CrashDump.dmp",
		t.wYear, t.wMonth, t.wDay,
		t.wHour, t.wMinute, t.wSecond
	);

	strPath += strFileName;

	HANDLE hDumpFile = CreateFile(strPath.wstr(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
	dumpInfo.ExceptionPointers = pException;
	dumpInfo.ThreadId = ::GetCurrentThreadId();
	dumpInfo.ClientPointers = TRUE;

	MiniDumpWriteDump(
		::GetCurrentProcess(),
		::GetCurrentProcessId(),
		hDumpFile,
		MiniDumpNormal,
		&dumpInfo,
		NULL,
		NULL);

	CloseHandle(hDumpFile);

	NaString strMsg;
	strMsg.Format(L"Unhandled exception has occurred.\nPlease report Crash dump to neoarc.\n\n-Crash dump created: %ls", strPath.wstr());
	::MessageBox(nullptr, strMsg.wstr(), L"Crashed :P (But our developer is safe)", MB_ICONERROR | MB_OK);

	return EXCEPTION_EXECUTE_HANDLER;
}
