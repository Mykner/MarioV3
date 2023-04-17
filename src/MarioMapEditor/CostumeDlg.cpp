// CostumeDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MarioMapEditor.h"
#include "CostumeDlg.h"
#include "afxdialogex.h"

#include "Game.h"
#include "Sprite.h"
#include "SpriteIndex.h"
#include "PathManager.h"

//  Define min max macros required by GDI+ headers.
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#else
#error max macro is already defined
#endif
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#else
#error min macro is already defined
#endif

#include <gdiplus.h>

//  Undefine min max macros so they won't collide with <limits> header content.
#undef min
#undef max

// CostumeDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CostumeDlg, CDialogEx)

CostumeDlg::CostumeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_COSTUME, pParent)
{
	m_pGame = nullptr;
	
	m_pSpriteItem = nullptr;

	m_hBitmapCostume = nullptr;
	m_hBitmapItem = nullptr;

	m_nType = 0;
}

CostumeDlg::~CostumeDlg()
{
}

void CostumeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CostumeDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CostumeDlg 메시지 처리기입니다.


BOOL CostumeDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL CostumeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (m_pGame)
	{
		// initialize gdi+
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR gdiplusToken;
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

		NaString strTex;

		// load image
		{
			strTex.Format(L"%ls\\Costumes\\CostumeSummary.bmp", PathManager::GetSpritePath(m_pGame));

			Gdiplus::Image *pGdiImage = Gdiplus::Image::FromFile(strTex.wstr());
			Gdiplus::Status status = pGdiImage->GetLastStatus();
			if (status != Gdiplus::Ok)
			{
				/*
				GenericError = 1,
				InvalidParameter = 2,
				OutOfMemory = 3,
				ObjectBusy = 4,
				InsufficientBuffer = 5,
				NotImplemented = 6,
				Win32Error = 7,
				WrongState = 8,
				Aborted = 9,
				FileNotFound = 10,
				ValueOverflow = 11,
				AccessDenied = 12,
				UnknownImageFormat = 13,
				FontFamilyNotFound = 14,
				FontStyleNotFound = 15,
				NotTrueTypeFont = 16,
				UnsupportedGdiplusVersion = 17,
				GdiplusNotInitialized = 18,
				PropertyNotFound = 19,
				PropertyNotSupported = 20,
				ProfileNotFound = 21
				*/

				CString strError;
				strError.Format(L"Load image failed: Costume, ErrorCode: %d\nPlease report to developer.", (int)status);
				::MessageBox(nullptr, strError, L"Failed", MB_OK);
			}

			// converting to bitmap
			Gdiplus::Bitmap* pGdiBitmap = static_cast<Gdiplus::Bitmap*>(pGdiImage);
			pGdiBitmap->GetHBITMAP(Gdiplus::Color(0, 0, 0), &m_hBitmapCostume);

			m_sizeBitmapCostume.cx = pGdiImage->GetWidth();
			m_sizeBitmapCostume.cy = pGdiImage->GetHeight();
			delete pGdiImage;
		}

		{
			strTex.Format(L"%ls\\ItemOverworld.bmp", PathManager::GetSpritePath(m_pGame));

			Gdiplus::Image *pGdiImage = Gdiplus::Image::FromFile(strTex.wstr());

			// converting to bitmap
			Gdiplus::Bitmap* pGdiBitmap = static_cast<Gdiplus::Bitmap*>(pGdiImage);
			pGdiBitmap->GetHBITMAP(Gdiplus::Color(0, 0, 0), &m_hBitmapItem);

			m_sizeBitmapItem.cx = pGdiImage->GetWidth();
			m_sizeBitmapItem.cy = pGdiImage->GetHeight();
			delete pGdiImage;
		}

		// shutdown gdi+
		Gdiplus::GdiplusShutdown(gdiplusToken);
	}

	int frame = 0;
	for (int i = 0; i < SPRIDX_COSTUME_LAST + 1; i++)
	{
		if (i == m_nType)
		{
		}

		if (i == 0)
		{
			m_mapIndex.insert(
				std::pair<int, int>(i, -1)
			);
		}
		else
		{
			m_mapIndex.insert(
				std::pair<int, int>(i, frame)
			);

			frame++;
			switch (frame)
			{
			case 6:
			case 150:
			case 152:
				frame++;
				break;
			}
		}

		if (frame > SPRIDX_COSTUME_LAST)
			break;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CostumeDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.

	// Draw Bitmap
	CRect rcClient;
	if (m_hBitmapCostume)
	{
		CDC memdc;
		memdc.CreateCompatibleDC(&dc);
		HGDIOBJ hOldObj;

		GetDlgItem(IDC_STATIC_LIST)->GetWindowRect(rcClient);
		ScreenToClient(rcClient);

		CRgn rgn;
		rgn.CreateRectRgn(rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);
		dc.SelectClipRgn(&rgn);

		int frame = -1;
		for (int i = 0; i < SPRIDX_COSTUME_LAST + 1; i++)
		{
			int x = rcClient.left + ((i % 20) * 50);
			int y = rcClient.top + ((i / 20) * 40);

			if (frame == m_nType - 2800 - 1)
			{
				CBrush br(0xff0000);
				CRect rc(x - 2, y - 2, x + 26, y + 26);
				dc.FillRect(&rc, &br);
			}

			CString str;
			if (i == 0)
			{
				hOldObj = memdc.SelectObject(m_hBitmapItem);
				DrawSprite(
					m_pSpriteItem, x, y, SPRIDX_MYSTERYMUSHROOM, dc, memdc);
				memdc.SelectObject(hOldObj);

				str.Format(L"-1");

				frame++;
			}
			else
			{
				hOldObj = memdc.SelectObject(m_hBitmapCostume);

				TransparentBlt(dc.GetSafeHdc(),
					x, y,
					24, 24,
					memdc.GetSafeHdc(),
					(frame * 25) % 250,
					(frame / 10) * 25,
					25, 25,
					RGB(0x48, 0xff, 0x00));

				memdc.SelectObject(hOldObj);

				str.Format(L"%3d", frame);

				frame++;
				switch (frame)
				{
				case 6:
				case 150:
				case 152:
					frame++;
					break;
				}
			}

			RECT rcText;
			rcText.left = x;
			rcText.top = y + 23;
			rcText.right = x + 256;
			rcText.bottom = y + 256;
			dc.SetBkMode(TRANSPARENT);
			dc.DrawText(str, &rcText, 0);

			if (frame > SPRIDX_COSTUME_LAST)
				break;
		}

		dc.SelectClipRgn(NULL);
		memdc.DeleteDC();
	}
}

void CostumeDlg::DrawSprite(Sprite * pSprite, int x, int y, int nFrame, CPaintDC & dc, CDC & memdc)
{
	// #NOTE Useless function but leave it for reference.

	if (pSprite == nullptr)
		return;

	auto f = pSprite->m_vecFrames[nFrame];
	auto rc = f.m_rc;
	int w = rc.right - rc.left;
	int h = rc.bottom - rc.top;

	/*
	dc.StretchBlt(
		x, y,
		24, 24, //w, h, 
		&memdc,
		rc.left,
		rc.top,
		w, h, 
		SRCCOPY);
	*/

	TransparentBlt(dc.GetSafeHdc(),
		x, y,
		24, 24,
		memdc.GetSafeHdc(),
		rc.left,
		rc.top,
		w, h,
		RGB(0x48, 0xff, 0x00));

	// #TODO Double buffering.
}


 void CostumeDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rcClient;
	GetDlgItem(IDC_STATIC_LIST)->GetWindowRect(rcClient);

	POINT pt;
	GetCursorPos(&pt);

	pt.x -= rcClient.left;
	pt.y -= rcClient.top;

	int x = pt.x / 50;
	int y = pt.y / 40;

	int index = (y * 20) + x;

	std::map<int, int>::iterator it = m_mapIndex.find(index);
	if (it != m_mapIndex.end())
	{
		m_nType = it->second + 2800 + 1;

		NaDebugOut(L"index: %d / type: %d / costume: %d\n",
			index,
			it->second + 2800 + 1,//ITEM_MYSTERYMUSHROOM,
			it->second
		);

		CString strName = GetCostumeName(it->second);
		CString strText;
		strText.Format(L"Select Costume: %s (%d)", strName, it->second);
		SetWindowText(strText);
	}

	Invalidate();

	CDialogEx::OnLButtonDown(nFlags, point);
}

CString CostumeDlg::GetCostumeName(int nIdx)
{
#define COSTUME(_idx, name)	case _idx:	return CString(L##name); break;
	switch (nIdx)
	{
	COSTUME(-1, "Random")
	COSTUME(0, "Mario")
	COSTUME(1, "Peach")
	COSTUME(2, "Luigi")
	COSTUME(3, "Bowser")
	COSTUME(4, "Goomba")
	COSTUME(5, "Block")
	COSTUME(7, "Trampoline")
	COSTUME(8, "Toad")
	COSTUME(9, "Bowser Jr.")
	COSTUME(10, "Mario(Silver)")
	COSTUME(11, "Mario(Gold)")
	COSTUME(12, "Builder Mario")
	COSTUME(13, "Yoshi")
	COSTUME(14, "Shy Guy")
	COSTUME(15, "Rosalina")
	COSTUME(16, "Wario")
	COSTUME(17, "Mario")
	COSTUME(18, "Shellcreeper")
	COSTUME(19, "Sidestepper")
	COSTUME(20, "Dr.Mario")
	COSTUME(21, "Kart Mario")
	COSTUME(22, "Waluigi")
	COSTUME(23, "Green Yarn Yoshi")
	COSTUME(24, "Pink Yarn Yoshi")
	COSTUME(25, "Blue Yarn Yoshi")
	COSTUME(26, "Mega Yarn Yoshi")
	COSTUME(27, "Donkey Kong Jr.")
	COSTUME(28, "Donkey Kong")
	COSTUME(29, "Diddy Kong")
	COSTUME(30, "Kirby")
	COSTUME(31, "King Dedede")
	COSTUME(32, "Meta Knight")
	COSTUME(33, "Pit")
	COSTUME(34, "Palutena")
	COSTUME(35, "Dark Pit")
	COSTUME(36, "Mega Man")
	COSTUME(37, "Samus")
	COSTUME(38, "Zero Suit Samus")
	COSTUME(39, "R.O.B.")
	COSTUME(40, "R.O.B.")
	COSTUME(41, "Marth")
	COSTUME(42, "Ike")
	COSTUME(43, "Lucina")
	COSTUME(44, "Robin")
	COSTUME(45, "Pikachu")
	COSTUME(46, "Charizard")
	COSTUME(47, "Jigglypuff")
	COSTUME(48, "Mewtwo")
	COSTUME(49, "Lucario")
	COSTUME(50, "Greninja")
	COSTUME(51, "Captain Olimar")
	COSTUME(52, "Pikmin")
	COSTUME(53, "Link")
	COSTUME(54, "Zelda")
	COSTUME(55, "Ganondorf")
	COSTUME(56, "Sheik")
	COSTUME(57, "Tingle")
	COSTUME(58, "Toon Link")
	COSTUME(59, "Shulk")
	COSTUME(60, "Ness")
	COSTUME(61, "Lucas")
	COSTUME(62, "Inkling Squid")
	COSTUME(63, "Inkling Boy")
	COSTUME(64, "Inkling Boy")
	COSTUME(65, "Inkling Girl")
	COSTUME(66, "Inkling Girl")
	COSTUME(67, "Wii Balance Board")
	COSTUME(68, "Wii Fit Trainer")
	COSTUME(69, "Chibi - Robo")
	COSTUME(70, "Villager")
	COSTUME(71, "Tom Nook")
	COSTUME(72, "K.K.Slider")
	COSTUME(73, "Mabel")
	COSTUME(74, "Resetti")
	COSTUME(75, "Rover")
	COSTUME(76, "Blathers")
	COSTUME(77, "Timmy & Tommy")
	COSTUME(78, "Kapp'n")
	COSTUME(79, "Celeste")
	COSTUME(80, "Kicks")
	COSTUME(81, "Isabelle(Summer Outfit)")
	COSTUME(82, "Isabelle(Winter Outfit)")
	COSTUME(83, "Digby")
	COSTUME(84, "Cyrus")
	COSTUME(85, "Reese")
	COSTUME(86, "Lottie")
	COSTUME(87, "Captain Falcon")
	COSTUME(88, "Mr.Game & Watch")
	COSTUME(89, "Sonic")
	COSTUME(90, "Duck Hunt")
	COSTUME(91, "PAC - MAN")
	COSTUME(92, "Foreman Spike")
	COSTUME(93, "Little Mac")
	COSTUME(94, "Fox McCloud")
	COSTUME(95, "Falco Lombardi")
	COSTUME(96, "Slippy Toad")
	COSTUME(97, "Peppy Hare")
	COSTUME(98, "Arwing")
	COSTUME(99, "Mahjong Tile")
	COSTUME(100, "Dr.Lobe")
	COSTUME(101, "Ashley")
	COSTUME(102, "Nikki")
	COSTUME(103, "Fighter Fly")
	COSTUME(104, "Totem Link")
	COSTUME(105, "Arino KACHO")
	COSTUME(106, "SUPER MARIO KUN")
	COSTUME(107, "Cat Mario")
	COSTUME(108, "Cat Peach")
	COSTUME(109, "Necky")
	COSTUME(110, "Frog Mario")
	COSTUME(111, "Mario Trio")
	COSTUME(112, "GLA")
	COSTUME(113, "Felyne")
	COSTUME(114, "Sky Pop")
	COSTUME(115, "Arcade Bunny")
	COSTUME(116, "Master Belch")
	COSTUME(117, "Mr.Saturn")
	COSTUME(118, "Captain Toad")
	COSTUME(119, "Bike")
	COSTUME(120, "Birdo")
	COSTUME(121, "Yamamura")
	COSTUME(122, "Daisy")
	COSTUME(123, "Professor E.Gadd")
	COSTUME(124, "Bulbasaur")
	COSTUME(125, "Charmander")
	COSTUME(126, "Squirtle")
	COSTUME(127, "Chitoge Kirisaki")
	COSTUME(128, "Barbara the Bat")
	COSTUME(129, "Statue Mario")
	COSTUME(130, "Mashiko")
	COSTUME(131, "Toadette")
	COSTUME(132, "Yu Ayasaki")
	COSTUME(133, "Starfy")
	COSTUME(134, "Wolf Link")
	COSTUME(135, "Balloon Fighter")
	COSTUME(136, "Nabbit")
	COSTUME(137, "Tetra")
	COSTUME(138, "Donbe")
	COSTUME(139, "Hikari")
	COSTUME(140, "Ayumi Tachibana")
	COSTUME(141, "Baby Mario")
	COSTUME(142, "Iris Archwell")	
	COSTUME(143, "Mallo")
	COSTUME(144, "Bubbles")
	COSTUME(145, "Diskun")
	COSTUME(146, "Volleyball Player")
	COSTUME(147, "BABYMETAL")
	COSTUME(148, "Popo & Nana")
	COSTUME(149, "Kitty White")
	COSTUME(151, "Melody")
	COSTUME(153, "Shaun the Sheep")
	
	//COSTUME(, "Undodog")
	//COSTUME(, "Dr. Kawashima")
	
	COSTUME(154, "Callie")
	COSTUME(155, "Callie")
	COSTUME(156, "Marie")
	COSTUME(157, "Marie")

	COSTUME(158, "Jumpman by Maga")
	COSTUME(159, "Bendy by JeffersonB")
	default:	return CString(L"Unknown"); break;
	}

	return CString(L"");
}
