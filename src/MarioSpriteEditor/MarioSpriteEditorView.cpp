
// MarioSpriteEditorView.cpp : CMarioSpriteEditorView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "MarioSpriteEditor.h"
#endif

#include "MarioSpriteEditorDoc.h"
#include "MarioSpriteEditorView.h"

#include "MainFrm.h"
#include "AnimationTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_USER_LOADDIALOGCOMPLETE	(WM_USER + 1)

// CMarioSpriteEditorView

IMPLEMENT_DYNCREATE(CMarioSpriteEditorView, CFormView)

BEGIN_MESSAGE_MAP(CMarioSpriteEditorView, CFormView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_BN_CLICKED(IDC_BUTTON_ADD_SPRITE, &CMarioSpriteEditorView::OnBnClickedButtonAddSprite)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_SPRITE, &CMarioSpriteEditorView::OnBnClickedButtonDeleteSprite)
	ON_WM_LBUTTONDBLCLK()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SPRITE, &CMarioSpriteEditorView::OnLvnItemchangedListSprite)
	ON_BN_CLICKED(IDC_BUTTON_INSERT_SPRITE, &CMarioSpriteEditorView::OnBnClickedButtonInsertSprite)
	ON_BN_CLICKED(IDC_BTN_OFFSET_CENTERBOTTOM, &CMarioSpriteEditorView::OnBnClickedBtnOffsetCenterbottom)
	ON_BN_CLICKED(IDC_BTN_OFFSET_LEFTTOP, &CMarioSpriteEditorView::OnBnClickedBtnOffsetLefttop)
	ON_BN_CLICKED(IDC_BTN_OFFSET_CENTER, &CMarioSpriteEditorView::OnBnClickedBtnOffsetCenter)
	ON_BN_CLICKED(IDC_BTN_OFFSET_RIGHTMIDDLE, &CMarioSpriteEditorView::OnBnClickedBtnOffsetRightmiddle)
	ON_COMMAND(ID_REARRANGE, &CMarioSpriteEditorView::OnRearrange)
	ON_COMMAND(ID_REARRANGE_8X, &CMarioSpriteEditorView::OnRearrange8x)
	ON_MESSAGE(WM_USER_LOADDIALOGCOMPLETE, OnLoadDialogComplete)
	ON_BN_CLICKED(IDC_BTN_SHOW_ANIMATION_TEST, &CMarioSpriteEditorView::OnBnClickedBtnShowAnimationTest)
END_MESSAGE_MAP()

// CMarioSpriteEditorView 생성/소멸

CMarioSpriteEditorView::CMarioSpriteEditorView() :
	CFormView(IDD)
{
	m_nZoomFactor = 2;
	m_bLoadCompleted = false;
	m_pAnimationTestDlg = nullptr;
}

CMarioSpriteEditorView::~CMarioSpriteEditorView()
{
	if (m_pAnimationTestDlg)
	{
		delete m_pAnimationTestDlg;
		m_pAnimationTestDlg = nullptr;
	}
}

BOOL CMarioSpriteEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CMarioSpriteEditorView 그리기

void CMarioSpriteEditorView::DoDataExchange(CDataExchange * pDX)
{
	CFormView::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_SPRITE, m_listSprite);
}

void CMarioSpriteEditorView::OnDraw(CDC* pDC)
{
	CMarioSpriteEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	if (!m_bLoadCompleted)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
	{
		int nIdx = GetSelectedListIndex();

		// Draw Bitmap
		CRect rcClient;
		if (pDoc->m_hBitmap)
		{
			CDC memdc;
			memdc.CreateCompatibleDC(pDC);
			HGDIOBJ hOldObj = memdc.SelectObject(pDoc->m_hBitmap);
			
			GetDlgItem(IDC_STATIC_BITMAP)->GetWindowRect(rcClient);
			ScreenToClient(rcClient);

			CRgn rgn;
			rgn.CreateRectRgn(rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);
			pDC->SelectClipRgn(&rgn);
			pDC->StretchBlt(
				rcClient.left, rcClient.top,
				pDoc->m_sizeBitmap.cx * m_nZoomFactor,
				pDoc->m_sizeBitmap.cy * m_nZoomFactor,
				&memdc,
				0, 0,
				pDoc->m_sizeBitmap.cx,
				pDoc->m_sizeBitmap.cy,
				SRCCOPY);

			pDC->SelectClipRgn(NULL);
			memdc.SelectObject(hOldObj);
			memdc.DeleteDC();
		}

		// Bitmap Area
		{
			HGDIOBJ hNullBrush = GetStockObject(NULL_BRUSH);
			HGDIOBJ hOldBrush = pDC->SelectObject(hNullBrush);
			pDC->Rectangle(&rcClient);
			pDC->SelectObject(hOldBrush);
		}

		// Draw Area Box
		if (nIdx >= 0)
		{
			GetDlgItem(IDC_STATIC_BITMAP)->GetWindowRect(rcClient);
			ScreenToClient(rcClient);

			CRect rcArea = m_rcCurrentArea;
			rcArea.left *= m_nZoomFactor;
			rcArea.top *= m_nZoomFactor;
			rcArea.right *= m_nZoomFactor;
			rcArea.bottom *= m_nZoomFactor;
			rcArea.OffsetRect(rcClient.left, rcClient.top);

			HGDIOBJ hNullBrush = GetStockObject(NULL_BRUSH);
			HGDIOBJ hOldBrush = pDC->SelectObject(hNullBrush);
			
			/*
			HGDIOBJ hPen;
			static bool bOdd = false;
			if (bOdd)
				hPen = GetStockObject(BLACK_PEN);
			else
				hPen = GetStockObject(WHITE_PEN);
			HGDIOBJ hOldPen = pDC->SelectObject(hPen);
			bOdd = !bOdd;
			*/

			pDC->Rectangle(&rcArea);
			rcArea.InflateRect(1, 1); pDC->Rectangle(&rcArea); 
			rcArea.InflateRect(1, 1); pDC->Rectangle(&rcArea); 
			rcArea.InflateRect(1, 1); pDC->Rectangle(&rcArea);
			rcArea.InflateRect(1, 1); pDC->DrawFocusRect(&rcArea);			

			/*
			pDC->SelectObject(hOldPen);
			*/
			pDC->SelectObject(hOldBrush);

		}

		// Draw Sprite
		if (nIdx >= 0)
		{
			SpriteFrame f = pDoc->m_Sprite.m_vecFrames[nIdx];
			CDC memdc;
			memdc.CreateCompatibleDC(pDC);
			HGDIOBJ hOldObj = memdc.SelectObject(pDoc->m_hBitmap);

			GetDlgItem(IDC_STATIC_PREVIEW)->GetWindowRect(rcClient);
			ScreenToClient(rcClient);

			CRgn rgn;
			rgn.CreateRectRgn(rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);
			pDC->SelectClipRgn(&rgn);

			CPoint ptCenter(
				rcClient.left + (rcClient.Width() / 2),
				rcClient.top + ((rcClient.Height() / 4) * 3)
			);

			CRect rcArea = m_rcCurrentArea;
			pDC->StretchBlt(
				-(f.m_ptOffset.x * m_nZoomFactor) + ptCenter.x, 
				-(f.m_ptOffset.y * m_nZoomFactor) + ptCenter.y,
				rcArea.Width() * m_nZoomFactor,
				rcArea.Height() * m_nZoomFactor,
				&memdc,
				rcArea.left, rcArea.top,
				rcArea.Width(),
				rcArea.Height(),
				SRCCOPY);

			int nArrowLen = 10;
			pDC->MoveTo(ptCenter);
			pDC->LineTo(ptCenter.x - nArrowLen, ptCenter.y + nArrowLen);
			pDC->MoveTo(ptCenter);
			pDC->LineTo(ptCenter.x + nArrowLen, ptCenter.y + nArrowLen);
			pDC->MoveTo(ptCenter);
			pDC->LineTo(ptCenter.x, ptCenter.y + (nArrowLen * 2));

			HGDIOBJ hNullBrush = GetStockObject(NULL_BRUSH);
			HGDIOBJ hOldBrush = pDC->SelectObject(hNullBrush);
			pDC->Rectangle(&rcClient);
			pDC->SelectObject(hOldBrush);

			pDC->SelectClipRgn(NULL);
			memdc.SelectObject(hOldObj);
			memdc.DeleteDC();
		}
	}
}

void CMarioSpriteEditorView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMarioSpriteEditorView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMarioSpriteEditorView 진단

#ifdef _DEBUG
void CMarioSpriteEditorView::AssertValid() const
{
	CView::AssertValid();
}

void CMarioSpriteEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMarioSpriteEditorDoc* CMarioSpriteEditorView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMarioSpriteEditorDoc)));
	return (CMarioSpriteEditorDoc*)m_pDocument;
}
#endif //_DEBUG


void CMarioSpriteEditorView::UpdateListItem(int nIdx)
{
	CMarioSpriteEditorDoc* pDoc = GetDocument();
	if (nIdx < 0)
		return;

	SpriteFrame f = pDoc->m_Sprite.m_vecFrames[nIdx];

	CString str;
	str.Format(L"%03d: %d,%d,%d,%d %d,%d", 
		nIdx,
		f.m_rc.left,
		f.m_rc.top,
		f.m_rc.right,
		f.m_rc.bottom,
		f.m_ptOffset.x,
		f.m_ptOffset.y
	);		
	m_listSprite.SetItemText(nIdx, 1, str);
}

void CMarioSpriteEditorView::SelectListItem(int nIdx)
{
	int nOldIdx = GetSelectedListIndex();
	m_listSprite.SetSelectionMark(-1);
	m_listSprite.SetItemState(nOldIdx, 0, LVIS_SELECTED);

	int nCnt = m_listSprite.GetItemCount();
	if (nIdx < nCnt)
	{
		m_listSprite.SetSelectionMark(nIdx);
		m_listSprite.SetItemState(nIdx, LVIS_SELECTED | LVIS_FOCUSED, 0xFF);
		m_listSprite.SetFocus();
	}
}

void CMarioSpriteEditorView::ClearListItems()
{
	m_listSprite.DeleteAllItems();
}

int CMarioSpriteEditorView::GetSelectedListIndex()
{
	POSITION pos = m_listSprite.GetFirstSelectedItemPosition();
	int nIdx = m_listSprite.GetNextSelectedItem(pos);
	
	return nIdx;
}

SpriteFrame* CMarioSpriteEditorView::GetSelectedSpriteFrame()
{
	POSITION pos = m_listSprite.GetFirstSelectedItemPosition();
	int nIdx = m_listSprite.GetNextSelectedItem(pos);
	if (nIdx < 0)
		return nullptr;

	CMarioSpriteEditorDoc* pDoc = GetDocument();
	return &pDoc->m_Sprite.m_vecFrames[nIdx];
}

void CMarioSpriteEditorView::RefreshProperties()
{
	((CMainFrame*)theApp.m_pMainWnd)->m_wndProperties.RefreshPropValues();
}

bool CMarioSpriteEditorView::RefreshSelection()
{
	SpriteFrame *pF = GetSelectedSpriteFrame();
	if (pF)
	{
		m_rcCurrentArea = pF->m_rc;
		Invalidate();
		return true;
	}

	return false;
}

// CMarioSpriteEditorView 메시지 처리기


void CMarioSpriteEditorView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	static bool bSetup = false;
	if (!bSetup)
	{
		m_listSprite.ModifyStyle(0, LVS_SHOWSELALWAYS);
		m_listSprite.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERINALLVIEWS);
		RECT rcList;
		m_listSprite.GetClientRect(&rcList);
		m_listSprite.InsertColumn(0, L"Img", 0, 32, -1);
		m_listSprite.InsertColumn(1, L"Info", 0, rcList.right - rcList.left - 32, -1);
		bSetup = true;
	}

	ClearListItems();
	CMarioSpriteEditorDoc* pDoc = GetDocument();
	if (pDoc->m_Sprite.m_vecFrames.size() > 0)
	{
		std::vector<SpriteFrame>::iterator it = pDoc->m_Sprite.m_vecFrames.begin();
		for (; it != pDoc->m_Sprite.m_vecFrames.end(); ++it)
		{
			int nIdx = m_listSprite.GetItemCount();
			m_listSprite.InsertItem(nIdx, L".");
			m_listSprite.SetItemData(nIdx, (DWORD)&(*it));

			UpdateListItem(nIdx);
		}

		m_listSprite.m_pSprite = &pDoc->m_Sprite;
		m_listSprite.m_hBitmap = pDoc->m_hBitmap;

		SelectListItem(0);
	}

	PostMessage(WM_USER + 1, 0, 0);
}


void CMarioSpriteEditorView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CMarioSpriteEditorDoc* pDoc = GetDocument();

	// Sprite detection.
	CPoint pt;
	CRect rcView, rcClient;
	GetWindowRect(&rcView);
	GetDlgItem(IDC_STATIC_BITMAP)->GetWindowRect(&rcClient);
	pt = { point.x - (rcClient.left - rcView.left), point.y - (rcClient.top - rcView.top) };

	pt.x = pt.x / m_nZoomFactor;
	pt.y = pt.y / m_nZoomFactor;
	m_rcCurrentArea = pDoc->FindSpriteArea(pt.x, pt.y);
	
	// Update Info
	SpriteFrame *pF = GetSelectedSpriteFrame();
	if (pF)
	{
		pF->m_rc = m_rcCurrentArea;

		int nIdx = GetSelectedListIndex();

		// Center Bottom is default
		OnBnClickedBtnOffsetCenterbottom();

		UpdateListItem(nIdx);
		Invalidate();

		RefreshProperties();
	}

	CFormView::OnLButtonDblClk(nFlags, point);
}


void CMarioSpriteEditorView::OnLvnItemchangedListSprite(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if (RefreshSelection())
	{
		RefreshProperties();
	}
	
	*pResult = 0;
}

void CMarioSpriteEditorView::OnBnClickedButtonAddSprite()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMarioSpriteEditorDoc* pDoc = GetDocument();

	SpriteFrame f;
	pDoc->m_Sprite.m_vecFrames.push_back(f);

	int nIdx = m_listSprite.GetItemCount();
	m_listSprite.InsertItem(nIdx, L".");
	m_listSprite.SetItemData(nIdx, (DWORD)&f);

	UpdateListItem(nIdx);
	SelectListItem(nIdx);

	GetDocument()->SetModifiedFlag(TRUE);
}


void CMarioSpriteEditorView::OnBnClickedButtonDeleteSprite()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMarioSpriteEditorDoc* pDoc = GetDocument();
	int nIdx = GetSelectedListIndex();

	{
		std::vector<SpriteFrame>::iterator it = pDoc->m_Sprite.m_vecFrames.begin();
		for (int i = 0; ; ++i, ++it)
		{
			if (i == nIdx)
			{
				pDoc->m_Sprite.m_vecFrames.erase(it);
				break;
			}
		}
	}

	// TODO Select = -1
	m_listSprite.DeleteItem(nIdx);
	SelectListItem(nIdx);

	{
		auto it = pDoc->m_Sprite.m_vecFrames.begin();
		SpriteFrame f;
		pDoc->m_Sprite.m_vecFrames.insert(it + nIdx, f);

		auto ar = pDoc->m_SpriteWork.m_strNewlines.Split(L",");
		for (int i = 0; i < ar.GetCount(); i++)
		{
			int nItemIdx = ar[i].ToInt();
			if (nItemIdx >= nIdx)
			{
				nItemIdx--;

				NaString str;
				str.Format(L"%d", nItemIdx);
				ar.Set(i, str);
			}
		}

		pDoc->m_SpriteWork.m_strNewlines = ar.Join(L",");
	}

	((CMainFrame*)theApp.m_pMainWnd)->m_wndProperties.RefreshPropValues();

	GetDocument()->SetModifiedFlag(TRUE);
}

void CMarioSpriteEditorView::OnBnClickedButtonInsertSprite()
{
	auto pDoc = GetDocument();
	int nIdx = GetSelectedListIndex();
	if (nIdx < 0)
		return;

	auto it = pDoc->m_Sprite.m_vecFrames.begin();
	SpriteFrame f;
	pDoc->m_Sprite.m_vecFrames.insert(it + nIdx, f);

	auto ar = pDoc->m_SpriteWork.m_strNewlines.Split(L",");
	for (int i=0; i<ar.GetCount(); i++)
	{
		int nItemIdx = ar[i].ToInt();
		if (nItemIdx >= nIdx)
		{
			nItemIdx++;

			NaString str;
			str.Format(L"%d", nItemIdx);
			ar.Set(i, str);
		}
	}

	pDoc->m_SpriteWork.m_strNewlines = ar.Join(L",");

	((CMainFrame*)theApp.m_pMainWnd)->m_wndProperties.RefreshPropValues();

	Invalidate();
	GetDocument()->SetModifiedFlag(TRUE);
}

void CMarioSpriteEditorView::OnBnClickedBtnOffsetLefttop()
{
	SpriteFrame *pF = GetSelectedSpriteFrame();
	if (pF)
	{
		pF->m_ptOffset.x = 0;
		pF->m_ptOffset.y = 0;

		Invalidate();
		GetDocument()->SetModifiedFlag(TRUE);
	}
}

void CMarioSpriteEditorView::OnBnClickedBtnOffsetCenterbottom()
{
	SpriteFrame *pF = GetSelectedSpriteFrame();
	if (pF)
	{
		pF->m_ptOffset.x = (pF->m_rc.right - pF->m_rc.left) / 2;
		pF->m_ptOffset.y = (pF->m_rc.bottom - pF->m_rc.top);
		
		Invalidate();
		GetDocument()->SetModifiedFlag(TRUE);
	}
}

void CMarioSpriteEditorView::OnBnClickedBtnOffsetCenter()
{
	SpriteFrame *pF = GetSelectedSpriteFrame();
	if (pF)
	{
		pF->m_ptOffset.x = pF->m_rc.left + (pF->m_rc.right - pF->m_rc.left) / 2;
		pF->m_ptOffset.y = pF->m_rc.top + (pF->m_rc.bottom - pF->m_rc.top) / 2;
		
		Invalidate();
		GetDocument()->SetModifiedFlag(TRUE);
	}
}

void CMarioSpriteEditorView::OnBnClickedBtnOffsetRightmiddle()
{
	SpriteFrame *pF = GetSelectedSpriteFrame();
	if (pF)
	{
		pF->m_ptOffset.x = (pF->m_rc.right - pF->m_rc.left);
		pF->m_ptOffset.y = pF->m_rc.top + (pF->m_rc.bottom - pF->m_rc.top) / 2;
		
		Invalidate();
		GetDocument()->SetModifiedFlag(TRUE);
	}
}

void CMarioSpriteEditorView::OnRearrange()
{
	GetDocument()->RearrangeSprites(GetDC());

	((CMainFrame*)theApp.m_pMainWnd)->m_wndProperties.RefreshPropValues();
	
	Invalidate();
	GetDocument()->SetModifiedFlag(TRUE);
}

void CMarioSpriteEditorView::OnRearrange8x()
{
	GetDocument()->RearrangeSprites(GetDC(), 1);

	((CMainFrame*)theApp.m_pMainWnd)->m_wndProperties.RefreshPropValues();
	
	Invalidate();
	GetDocument()->SetModifiedFlag(TRUE);
}

LRESULT CMarioSpriteEditorView::OnLoadDialogComplete(WPARAM wParam, LPARAM lParam)
{
	m_bLoadCompleted = true;

	// #TEMP Costume Sprite Rearrange Function (Costume.bmp to CostumeXXX.bmp)
	if (0)
	{
		// Load Main Costume
		SIZE sizeBitmap = { 0, 0 };
		HBITMAP hBitmap = 0;
		Sprite SpriteCostume(nullptr);
		{
			// Bitmap
			NaString strBmp = L"C:\\MyProjects\\Game\\MarioV3\\Resources\\Sprite\\Costume.bmp";
			{
				// initialize gdi+
				Gdiplus::GdiplusStartupInput gdiplusStartupInput;
				ULONG_PTR gdiplusToken;
				GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

				// load image
				{
					Gdiplus::Image *pGdiImage = Gdiplus::Image::FromFile(strBmp.wstr());
					Gdiplus::Bitmap* pGdiBitmap = static_cast<Gdiplus::Bitmap*>(pGdiImage);
					pGdiBitmap->GetHBITMAP(Gdiplus::Color(0, 0, 0), &hBitmap);

					sizeBitmap.cx = pGdiImage->GetWidth();
					sizeBitmap.cy = pGdiImage->GetHeight();
					delete pGdiImage;
				}

				// shutdown gdi+
				Gdiplus::GdiplusShutdown(gdiplusToken);
			}

			// Sprite
			NaString strCostume = L"C:\\MyProjects\\Game\\MarioV3\\Resources\\Sprite\\Costume.sprite";

			FILE *fp;
			fopen_s(&fp, strCostume.cstr(), "rb");
			if (fp != nullptr)
			{
				for (; ; )
				{
					int nTemp;
					SpriteFrame f;
					int nRet = fscanf_s(fp, "%d %d %d %d %d %d %d",
						&f.m_rc.left,
						&f.m_rc.top,
						&f.m_rc.right,
						&f.m_rc.bottom,
						&f.m_ptOffset.x,
						&f.m_ptOffset.y,
						&nTemp);

					if (nRet < 7 || nRet == EOF)
						break;

					SpriteCostume.m_vecFrames.push_back(f);
				}
				fclose(fp);

				NaDebugOut(L"Load Sprite: Costume, %d frames\n", (int)SpriteCostume.m_vecFrames.size());
			}
		}

		CDC *dc = GetDC();

		// Make New bitmap
		CDC memdc;
		memdc.CreateCompatibleDC(dc);

		int nWidth = sizeBitmap.cx;
		int nHeight = 25;
		HBITMAP hTmpBmp = CreateCompatibleBitmap(dc->GetSafeHdc(), nWidth, nHeight);
		HGDIOBJ hOldBmp = memdc.SelectObject(hTmpBmp);

		// Fill colorkey
		CBrush br;
		br.CreateSolidBrush(0x00ff48);

		RECT rc;
		rc.left = 0;
		rc.top = 0;
		rc.right = nWidth;
		rc.bottom = nHeight;
		memdc.FillRect(&rc, &br);

		// Rearrange all
		int nCostumes = SpriteCostume.m_vecFrames.size() / 23;
		for (int i = 0; i < nCostumes; i++)
		{
			// Copy from org
			CDC memdc2;
			memdc2.CreateCompatibleDC(&memdc);
			HGDIOBJ hOldBmp2 = memdc2.SelectObject(hBitmap);

			for (int k = 0; k<23; k++)
			{
				SpriteFrame *f = &SpriteCostume.m_vecFrames[i*23 + k];
				int nFrameWidth = f->m_rc.right - f->m_rc.left;
				int nFrameHeight = f->m_rc.bottom - f->m_rc.top;
				int nNewX = f->m_rc.left;
				int nNewY = f->m_rc.top % 25;

				int nBoxWidth = nFrameWidth;
				int nBoxHeight = nFrameHeight;

				BitBlt(memdc,
					nNewX,
					nNewY,
					nFrameWidth,
					nFrameHeight,
					memdc2,
					f->m_rc.left,
					f->m_rc.top,
					SRCCOPY);
			}

			// Save bitmap
			{
				// initialize gdi+
				Gdiplus::GdiplusStartupInput gdiplusStartupInput;
				ULONG_PTR gdiplusToken;
				GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

				HPALETTE hPalette = (HPALETTE)GetStockObject(DEFAULT_PALETTE);

				//Gdiplus::Bitmap *pBitmap = new Gdiplus::Bitmap((HBITMAP)m_hBitmap, hPalette);
				Gdiplus::Bitmap *pBitmap = Gdiplus::Bitmap::FromHBITMAP(hTmpBmp, hPalette);

				CLSID clsid;
				NaString strFileName;
				strFileName.Format(L"C:\\MyProjects\\Game\\MarioV3\\Resources\\Sprite\\Costumes\\Costume%03d.bmp", i);
				GetDocument()->GetEncoderClsid(L"image/bmp", &clsid);

				// Make 24bpp clone
				{
					Gdiplus::Bitmap *pClone = pBitmap->Clone(0, 0, nWidth, nHeight, PixelFormat24bppRGB);
					delete pBitmap;
					pBitmap = pClone;
				}

				pBitmap->Save(strFileName.wstr(), &clsid, NULL);
				delete pBitmap;
				pBitmap = nullptr;

				// shutdown gdi+
				Gdiplus::GdiplusShutdown(gdiplusToken);
			}

			memdc2.DeleteDC();
		}

		// Replace original
		memdc.SelectObject(hOldBmp);
		memdc.DeleteDC();

		DeleteObject(br);
		DeleteObject(hBitmap);

		hBitmap = hTmpBmp;

		NaDebugOut(L"Done.\n");
	}

	// #TEMP Make Costume Summary from Costume.bmp
	if (0)
	{
		// Load Main Costume
		SIZE sizeBitmap = { 0, 0 };
		HBITMAP hBitmap = 0;
		Sprite SpriteCostume(nullptr);
		{
			// Bitmap
			NaString strBmp = L"C:\\MyProjects\\Game\\MarioV3\\Resources\\Sprite\\Costume.bmp";
			{
				// initialize gdi+
				Gdiplus::GdiplusStartupInput gdiplusStartupInput;
				ULONG_PTR gdiplusToken;
				GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

				// load image
				{
					Gdiplus::Image *pGdiImage = Gdiplus::Image::FromFile(strBmp.wstr());
					Gdiplus::Bitmap* pGdiBitmap = static_cast<Gdiplus::Bitmap*>(pGdiImage);
					pGdiBitmap->GetHBITMAP(Gdiplus::Color(0, 0, 0), &hBitmap);

					sizeBitmap.cx = pGdiImage->GetWidth();
					sizeBitmap.cy = pGdiImage->GetHeight();
					delete pGdiImage;
				}

				// shutdown gdi+
				Gdiplus::GdiplusShutdown(gdiplusToken);
			}

			// Sprite
			NaString strCostume = L"C:\\MyProjects\\Game\\MarioV3\\Resources\\Sprite\\Costume.sprite";

			FILE *fp;
			fopen_s(&fp, strCostume.cstr(), "rb");
			if (fp != nullptr)
			{
				for (; ; )
				{
					int nTemp;
					SpriteFrame f;
					int nRet = fscanf_s(fp, "%d %d %d %d %d %d %d",
						&f.m_rc.left,
						&f.m_rc.top,
						&f.m_rc.right,
						&f.m_rc.bottom,
						&f.m_ptOffset.x,
						&f.m_ptOffset.y,
						&nTemp);

					if (nRet < 7 || nRet == EOF)
						break;

					SpriteCostume.m_vecFrames.push_back(f);
				}
				fclose(fp);

				NaDebugOut(L"Load Sprite: Costume, %d frames\n", (int)SpriteCostume.m_vecFrames.size());
			}
		}

		CDC *dc = GetDC();

		// Make New bitmap
		CDC memdc;
		memdc.CreateCompatibleDC(dc);

		int nWidth = 256;
		int nHeight = 512;
		HBITMAP hTmpBmp = CreateCompatibleBitmap(dc->GetSafeHdc(), nWidth, nHeight);
		HGDIOBJ hOldBmp = memdc.SelectObject(hTmpBmp);

		// Fill colorkey
		CBrush br;
		br.CreateSolidBrush(0x00ff48);

		RECT rc;
		rc.left = 0;
		rc.top = 0;
		rc.right = nWidth;
		rc.bottom = nHeight;
		memdc.FillRect(&rc, &br);

		// Rearrange all
		int nCostumes = SpriteCostume.m_vecFrames.size() / 23;
		for (int i = 0; i < nCostumes; i++)
		{
			// Copy from org
			CDC memdc2;
			memdc2.CreateCompatibleDC(&memdc);
			HGDIOBJ hOldBmp2 = memdc2.SelectObject(hBitmap);
			
			SpriteFrame *f = &SpriteCostume.m_vecFrames[i * 23];
			int nFrameWidth = f->m_rc.right - f->m_rc.left;
			int nFrameHeight = f->m_rc.bottom - f->m_rc.top;
			int nNewX = (i * 25) % 250;
			int nNewY = (i / 10) * 25;
			int nBoxWidth = nFrameWidth;
			int nBoxHeight = nFrameHeight;

			BitBlt(memdc,
				nNewX,
				nNewY,
				nFrameWidth,
				nFrameHeight,
				memdc2,
				f->m_rc.left,
				f->m_rc.top,
				SRCCOPY);
			
			memdc2.DeleteDC();
		}

		// Save bitmap
		{
			// initialize gdi+
			Gdiplus::GdiplusStartupInput gdiplusStartupInput;
			ULONG_PTR gdiplusToken;
			GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

			HPALETTE hPalette = (HPALETTE)GetStockObject(DEFAULT_PALETTE);

			//Gdiplus::Bitmap *pBitmap = new Gdiplus::Bitmap((HBITMAP)m_hBitmap, hPalette);
			Gdiplus::Bitmap *pBitmap = Gdiplus::Bitmap::FromHBITMAP(hTmpBmp, hPalette);

			CLSID clsid;
			NaString strFileName;
			strFileName.Format(L"C:\\MyProjects\\Game\\MarioV3\\Resources\\Sprite\\Costumes\\CostumeSummary.bmp");
			GetDocument()->GetEncoderClsid(L"image/bmp", &clsid);

			// Make 24bpp clone
			{
				Gdiplus::Bitmap *pClone = pBitmap->Clone(0, 0, nWidth, nHeight, PixelFormat24bppRGB);
				delete pBitmap;
				pBitmap = pClone;
			}

			pBitmap->Save(strFileName.wstr(), &clsid, NULL);
			delete pBitmap;
			pBitmap = nullptr;

			// shutdown gdi+
			Gdiplus::GdiplusShutdown(gdiplusToken);
		}

		// Replace original
		memdc.SelectObject(hOldBmp);
		memdc.DeleteDC();

		DeleteObject(br);
		DeleteObject(hBitmap);

		hBitmap = hTmpBmp;

		NaDebugOut(L"Done.\n");
	}

	return 0;
}


void CMarioSpriteEditorView::OnBnClickedBtnShowAnimationTest()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pAnimationTestDlg == nullptr)
	{
		m_pAnimationTestDlg = new AnimationTestDlg;
		m_pAnimationTestDlg->m_pMainView = this;

		m_pAnimationTestDlg->Create(IDD_ANIMATION_TEST, this);
		m_pAnimationTestDlg->Init();
		m_pAnimationTestDlg->ShowWindow(SW_SHOW);
		m_pAnimationTestDlg->CenterWindow();
	}
	else
	{
		if (m_pAnimationTestDlg->IsWindowVisible())
			m_pAnimationTestDlg->ShowWindow(SW_HIDE);
		else
		{
			m_pAnimationTestDlg->Init();
			m_pAnimationTestDlg->ShowWindow(SW_SHOW);
		}
	}
}
