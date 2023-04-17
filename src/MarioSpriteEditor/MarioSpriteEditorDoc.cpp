
// MarioSpriteEditorDoc.cpp : CMarioSpriteEditorDoc 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "MarioSpriteEditor.h"
#endif

#include "MarioSpriteEditorDoc.h"

#include <propkey.h>

#include <gdiplus.h>
#include "NaUrl.h"
#include "JsonManager.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CMainFrame* theMain;

// CMarioSpriteEditorDoc

IMPLEMENT_DYNCREATE(CMarioSpriteEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CMarioSpriteEditorDoc, CDocument)
END_MESSAGE_MAP()


// CMarioSpriteEditorDoc 생성/소멸

CMarioSpriteEditorDoc::CMarioSpriteEditorDoc() :
	m_Sprite(nullptr)
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.
	m_hBitmap = nullptr;
	m_SpriteWork.BuildPropertyMap();
}

CMarioSpriteEditorDoc::~CMarioSpriteEditorDoc()
{
}

BOOL CMarioSpriteEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CMarioSpriteEditorDoc serialization

BOOL CMarioSpriteEditorDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	NaString strSprite = lpszPathName;
	NaString strBmp = m_strBitmapFilename;
	/*
	if (strSprite.Right(7).CompareNoCase(L".sprite") == 0)
	{
		strBmp = strSprite;
		strBmp.ReplaceAll(L".sprite", L".bmp");
	}
	*/

	FILE *fp;
	fopen_s(&fp, strSprite.cstr(), "wb");
	if (fp != nullptr)
	{
		std::vector<SpriteFrame>::iterator it = m_Sprite.m_vecFrames.begin();
		for (; it != m_Sprite.m_vecFrames.end(); ++it)
		{
			SpriteFrame f = *it;
			CStringA str;
			str.Format("%4d %4d %4d %4d %4d %4d -1\r\n",
				f.m_rc.left,
				f.m_rc.top,
				f.m_rc.right,
				f.m_rc.bottom,
				f.m_ptOffset.x,
				f.m_ptOffset.y);

			fprintf(fp, "%s", str.GetBuffer());
		}
		fprintf(fp, "-1");

		fclose(fp);

		LOG(L"Save Sprite: %s, %d frames\n", strSprite.wstr(), (int)m_Sprite.m_vecFrames.size());
		theMain->Output(L"Save Sprite: %s, %d frames", strSprite.wstr(), (int)m_Sprite.m_vecFrames.size());
	}

	NaString strWork;
	strWork = strSprite;
	strWork.ReplaceAll(L".sprite", L".workinfo_");
	if (m_SpriteWork.m_bBitmapChanged)
	{
		CTime t = CTime::GetCurrentTime();
		NaString strDt;
		strDt.Format(L"%04d%02d%02d_%02d%02d%02d",
			t.GetYear(), t.GetMonth(), t.GetDay(),
			t.GetHour(), t.GetMinute(), t.GetSecond()
		);

		NaString strBmpBackup;
		strBmpBackup.Format(L"%s_%s", strBmp.wstr(), strDt.wstr());
		CopyFile(strBmp.wstr(), strBmpBackup.wstr(), FALSE);

		LOG(L"Backup Bitmap to: %s\n", strBmpBackup.wstr());
		theMain->Output(L"Backup Bitmap to: %s", strBmpBackup.wstr());

		// Save bitmap
		{
			// initialize gdi+
			Gdiplus::GdiplusStartupInput gdiplusStartupInput;
			ULONG_PTR gdiplusToken;
			GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

			HPALETTE hPalette = (HPALETTE)GetStockObject(DEFAULT_PALETTE);

			//Gdiplus::Bitmap *pBitmap = new Gdiplus::Bitmap((HBITMAP)m_hBitmap, hPalette);
			Gdiplus::Bitmap *pBitmap = Gdiplus::Bitmap::FromHBITMAP(m_hBitmap, hPalette);

			CLSID clsid;
			NaString strFileName(strBmp);
			if (strFileName.Right(4).CompareNoCase(L".bmp") == 0)
				GetEncoderClsid(L"image/bmp", &clsid);
			/*
			else if (strFileName.Right(4).CompareNoCase(L".jpg") == 0)
				GetEncoderClsid(L"image/jpeg", &clsid);
			else if (strFileName.Right(4).CompareNoCase(L".png") == 0)
				GetEncoderClsid(L"image/png", &clsid);
			else
			{
				// Not supported format
				delete pBitmap;
				return false;
			}
			*/

			// TODO check path exists.

			// Make 24bpp clone
			{
				Gdiplus::Bitmap *pClone = pBitmap->Clone(0, 0, m_sizeBitmap.cx, m_sizeBitmap.cy, PixelFormat24bppRGB);
				delete pBitmap;
				pBitmap = pClone;
			}

			pBitmap->Save(strFileName.wstr(), &clsid, NULL);
			delete pBitmap;
			pBitmap = nullptr;

			theMain->Output(L"Save Bitmap: %s", strFileName);

			// TODO validate result

			// shutdown gdi+
			Gdiplus::GdiplusShutdown(gdiplusToken);
		}

		m_SpriteWork.m_bBitmapChanged = false;
	}

	// Save workinfo
	Json::Value j;
	JsonManager::SaveJsonObjectProperty(&m_SpriteWork, j);
	j["AnimationFrames"] = Json::Value();

	BEGIN_VECTOR_LOOP(SpriteAnimationFrames*, m_SpriteWork.m_vecAnimationFrames) {
		Json::Value jObj;
		jObj["Name"] = pObj->m_strName.cstr();
		jObj["Frames"] = pObj->m_strFrames.cstr();

		j["AnimationFrames"].append(jObj);
	} END_VECTOR_LOOP();

	JsonManager::SaveFile(strWork.cstr(), &m_SpriteWork, j);

	SetModifiedFlag(FALSE);

	return TRUE;
	//return CDocument::OnSaveDocument(lpszPathName);
}

void CMarioSpriteEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
		
		// Not using Serialize
		// Unicode String Issue + File Access Sharing Problem
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
		NaString strSprite = m_strFilename = ar.m_strFileName;
		NaString strBmp;
		if (strSprite.Right(7).CompareNoCase(L".sprite") == 0)
		{
			strBmp = strSprite;
			strBmp.ReplaceAll(L".sprite", L".bmp");
			m_strBitmapFilename = strBmp;
		}

		// initialize gdi+
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR gdiplusToken;
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

		// load image
		{
			Gdiplus::Image *pGdiImage = Gdiplus::Image::FromFile(strBmp.wstr());
			if (pGdiImage->GetWidth() == 0)
			{
				strBmp.ReplaceAll(L".bmp", L"Overworld.bmp");
				pGdiImage = Gdiplus::Image::FromFile(strBmp.wstr());
				m_strBitmapFilename = strBmp;
			}

			// converting to bitmap
			Gdiplus::Bitmap* pGdiBitmap = static_cast<Gdiplus::Bitmap*>(pGdiImage);
			pGdiBitmap->GetHBITMAP(Gdiplus::Color(0, 0, 0), &m_hBitmap);

			m_sizeBitmap.cx = pGdiImage->GetWidth();
			m_sizeBitmap.cy = pGdiImage->GetHeight();
			delete pGdiImage;
		}

		// shutdown gdi+
		Gdiplus::GdiplusShutdown(gdiplusToken);

		theMain->Output(L"Load Bitmap: %ls", m_strBitmapFilename);

		// Load Sprite Frames
		m_Sprite.m_vecFrames.clear();
		
		FILE *fp;
		fopen_s(&fp, strSprite.cstr(), "rb");
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

				m_Sprite.m_vecFrames.push_back(f);
			}
			fclose(fp);

			NaDebugOut(L"Load Sprite: %s, %d frames\n", strSprite.wstr(), (int)m_Sprite.m_vecFrames.size());
			theMain->Output(L"Load Sprite: %s, %d frames", strSprite.wstr(), (int)m_Sprite.m_vecFrames.size());
		}

		NaString strWork;
		strWork = strSprite;
		strWork.ReplaceAll(L".sprite", L".workinfo_");
					
		// Load workinfo
		Json::Value *j = JsonManager::LoadFile(strWork.cstr());
		if (j)
		{
			JsonManager::LoadJsonObjectProperty(&m_SpriteWork, *j);
			SAFE_DELETE_OBJPTR_VECTOR(SpriteAnimationFrames*, m_SpriteWork.m_vecAnimationFrames);

			Json::Value jGroup = (*j)["AnimationFrames"];
			for (int i = 0; i < jGroup.size(); i++)
			{
				Json::Value jObj = jGroup[i];

				SpriteAnimationFrames *f = new SpriteAnimationFrames;
				f->m_strName = jObj["Name"].asString().c_str();
				f->m_strFrames = jObj["Frames"].asString().c_str();

				m_SpriteWork.m_vecAnimationFrames.push_back(f);
			}

			delete j;
		}
	}
}

#ifdef SHARED_HANDLERS

// 축소판 그림을 지원합니다.
void CMarioSpriteEditorDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 문서의 데이터를 그리려면 이 코드를 수정하십시오.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 검색 처리기를 지원합니다.
void CMarioSpriteEditorDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CMarioSpriteEditorDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CMarioSpriteEditorDoc 진단

#ifdef _DEBUG
void CMarioSpriteEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMarioSpriteEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

// CMarioSpriteEditorDoc 명령


CRect CMarioSpriteEditorDoc::FindSpriteArea(int x, int y)
{
	CRect rc;
	if (!m_hBitmap)
		return rc;

	DWORD ck = SpriteDefaults::dwColorKey;
	ck = 0x00ff48;

	CDC memdc;
	memdc.CreateCompatibleDC(NULL);
	HGDIOBJ hOldObj = memdc.SelectObject(m_hBitmap);
	
	COLORREF col = ::GetPixel(memdc.m_hDC, x, y);
	//NaDebugOut(L"%d, %d - Color: %d,%d,%d\n", x, y, GetRValue(col), GetGValue(col), GetBValue(col));
	if (col != ck)
	{
		int _x, _y;

		// First Phase
		NaDebugOut(L"SpriteRecognize - First Phase\n");
		{
			// Find Left
			_x = x, _y = y;
			for (; _x >= 0; _x--)
			{
				if (GetPixel(memdc.m_hDC, _x, _y) == ck) break;
			}
			rc.left = _x;

			// Find Top
			_x = x, _y = y;
			for (; _y >= 0; _y--)
			{
				if (GetPixel(memdc.m_hDC, _x, _y) == ck) break;
			}
			rc.top = _y;

			// Find Right
			_x = x, _y = y;
			for (; _x < m_sizeBitmap.cx; _x++)
			{
				if (GetPixel(memdc.m_hDC, _x, _y) == ck) break;
			}
			rc.right = _x;

			// Find Bottom
			_x = x, _y = y;
			for (; _y < m_sizeBitmap.cy; _y++)
			{
				if (GetPixel(memdc.m_hDC, _x, _y) == ck) break;
			}
			rc.bottom = _y;
		}
		NaDebugOut(L"-> %d,%d,%d,%d\n", rc.left, rc.top, rc.right, rc.bottom);

		// Second Phase
		NaDebugOut(L"SpriteRecognize - Second Phase\n");
		while (true)
		{
			NaDebugOut(L"Trying...\n");
			int nErrorCheck = 0;

			// Check Left
			_x = rc.left, _y = rc.top;
			for (; _x >= 0; _x--)
			{
				bool bFound = false;
				for (; _y < rc.bottom; _y++)
				{
					if (_x >=0 && _y >= 0 && GetPixel(memdc.m_hDC, _x, _y) != ck)
					{
						bFound = true;
						break;
					}
				}
				if (!bFound)
					break;
				nErrorCheck++;
			}
			rc.left = _x;

			// Check Top
			_x = rc.left, _y = rc.top;
			for (; _y >= 0; _y--)
			{
				bool bFound = false;
				for (; _x < rc.right; _x++)
				{
					if (_x >= 0 && _y >= 0 && GetPixel(memdc.m_hDC, _x, _y) != ck)
					{
						bFound = true;
						break;
					}
				}
				if (!bFound)
					break;
				nErrorCheck++;
			}
			rc.top = _y;

			// Check Right
			_x = rc.right, _y = rc.top;
			for (; _x < m_sizeBitmap.cx; _x++)
			{
				bool bFound = false;
				for (; _y < rc.bottom; _y++)
				{
					if (_x >= 0 && _y >= 0 && GetPixel(memdc.m_hDC, _x, _y) != ck)
					{
						bFound = true;
						break;
					}
				}
				if (!bFound)
					break;
				nErrorCheck++;
			}
			rc.right = _x;

			// Check Bottom
			_x = rc.left, _y = rc.bottom;
			for (; _y < m_sizeBitmap.cy; _y++)
			{
				bool bFound = false;
				for (; _x < rc.right; _x++)
				{
					if (_x >= 0 && _y >= 0 && GetPixel(memdc.m_hDC, _x, _y) != ck)
					{
						bFound = true;
						break;
					}
				}
				if (!bFound)
					break;
				nErrorCheck++;
			}
			rc.bottom = _y;

			if (nErrorCheck == 0)
				break;
			NaDebugOut(L"-> %d,%d,%d,%d\n", rc.left, rc.top, rc.right, rc.bottom);
		}

		rc.left++;
		rc.top++;
		//NaDebugOut(L"-> %d,%d,%d,%d\n", rc.left, rc.top, rc.right, rc.bottom);
	}

	memdc.SelectObject(hOldObj);
	memdc.DeleteDC();

	theMain->Output(L"Recognize SpritePiece: Success");
	return rc;
}

int CMarioSpriteEditorDoc::GetEncoderClsid(const wchar_t * format, CLSID * pClsid)
{
	UINT nNumEncoders = 0;
	UINT nSize = 0;
	Gdiplus::ImageCodecInfo *pImageCodecInfo = NULL;
	Gdiplus::GetImageEncodersSize(&nNumEncoders, &nSize);

	int nRet = -1;
	do
	{
		if (nSize == 0)
			break;

		pImageCodecInfo = (Gdiplus::ImageCodecInfo *)(malloc(nSize));
		if (pImageCodecInfo == nullptr)
			break;

		GetImageEncoders(nNumEncoders, nSize, pImageCodecInfo);
		for (UINT i = 0; i < nNumEncoders; ++i)
		{
			if (wcscmp(pImageCodecInfo[i].MimeType, format) == 0)
			{
				*pClsid = pImageCodecInfo[i].Clsid;
				free(pImageCodecInfo);
				pImageCodecInfo = nullptr;

				nRet = i;
				break;
			}
		}
	} while (false);

	if (pImageCodecInfo != nullptr)
	{
		free(pImageCodecInfo);
		pImageCodecInfo = nullptr;
	}

	return nRet;
}

void CMarioSpriteEditorDoc::RearrangeSprites(CDC *dc, int nMode)
{
	// Make New bitmap
	CDC memdc;
	memdc.CreateCompatibleDC(dc);

	int nWidth = m_sizeBitmap.cx;
	int nHeight = m_sizeBitmap.cy;
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
	int nLine = 0;
	int nCurrentX = 0;
	int nCurrentY = 0;

	// Force newline
	NaStrArray arNewlines = m_SpriteWork.m_strNewlines.Split(L",");
	std::map<int, bool> mapNewlines;
	for (int i=0; i<arNewlines.GetCount(); i++)
	{
		mapNewlines.insert(
			std::pair<int, bool>(arNewlines[i].ToInt(), true)
		);
	}

	// Mario?
	if (m_strFilename.Find(L"Mario.") >= 0)
	{
		// Copy from org
		CDC memdc2;
		memdc2.CreateCompatibleDC(&memdc);
		HGDIOBJ hOldBmp2 = memdc2.SelectObject(m_hBitmap);

		BitBlt(memdc,
			0,
			0,
			15,
			3,
			memdc2,
			0,
			0,
			SRCCOPY);

		memdc2.DeleteDC();

		nCurrentY = 5;
	}

	SpritePatchDataVector vecPatchData;
	int nFrames = m_Sprite.m_vecFrames.size();
	for (int i=0; i<nFrames; )
	{
		int nLineWidth = 0;
		int nLineHeight = 0;

		// Calc LineHeight
		int j = i;
		for (; j<nFrames; j++)
		{
			int nFrameWidth = m_Sprite.m_vecFrames[j].m_rc.right - m_Sprite.m_vecFrames[j].m_rc.left;
			int nFrameHeight = m_Sprite.m_vecFrames[j].m_rc.bottom - m_Sprite.m_vecFrames[j].m_rc.top;

			if (nMode == 1)
			{
				if (nFrameWidth % 16 != 0)
					nFrameWidth = ((nFrameWidth / 16) + 1) * 16;
					
				if (nFrameHeight %  16 != 0)
					nFrameHeight = ((nFrameHeight / 16) + 1) * 16;
			}

			if (nLineWidth + nFrameWidth >= nWidth)
				break;

			if (j != i && mapNewlines.find(j - 1) != mapNewlines.end())
				break;

			nLineWidth += nFrameWidth;
			nLineWidth += 1;

			if (nLineHeight < nFrameHeight)
				nLineHeight = nFrameHeight;
		}

		NaDebugOut(L"Line %d: %d x %d (Frame: %d ~ %d)\n", nLine, nLineWidth, nLineHeight, i, j-1);

		// Copy from org
		CDC memdc2;
		memdc2.CreateCompatibleDC(&memdc);
		HGDIOBJ hOldBmp2 = memdc2.SelectObject(m_hBitmap);

		for (int k=i; k<j; k++)
		{
			SpriteFrame *f = &m_Sprite.m_vecFrames[k];
			int nFrameWidth = f->m_rc.right - f->m_rc.left;
			int nFrameHeight = f->m_rc.bottom - f->m_rc.top;
			int nNewX = nCurrentX;
			int nNewY = nCurrentY + nLineHeight - nFrameHeight;

			NaDebugOut(L"    Frame %03d: %d, %d\n", k, nNewX, nNewY);
			int nBoxWidth = nFrameWidth;
			int nBoxHeight = nFrameHeight;
			if (nMode == 1)
			{
				if (nBoxWidth % 16 != 0)
				{
					nBoxWidth = ((nBoxWidth / 16) + 1) * 16;
					nNewX += (nBoxWidth / 2) - (nFrameWidth / 2);
				}

				if (nBoxHeight % 16 != 0)
				{
					nBoxHeight = ((nBoxHeight / 16) + 1) * 16;
				}
			}

			BitBlt(memdc,
				nNewX,
				nNewY,
				nFrameWidth,
				nFrameHeight,
				memdc2,
				f->m_rc.left,
				f->m_rc.top,
				SRCCOPY);

			SpritePatchData data;
			data.ptFrom.x = f->m_rc.left;
			data.ptFrom.y = f->m_rc.top;
			data.ptTo.x = nNewX;
			data.ptTo.y = nNewY;
			data.nWidth = nFrameWidth;
			data.nHeight = nFrameHeight;
			vecPatchData.push_back(data);

			f->m_rc.left = nNewX;
			f->m_rc.top = nNewY;
			f->m_rc.right = nNewX + nFrameWidth;
			f->m_rc.bottom = nNewY + nFrameHeight;

			nCurrentX += nBoxWidth;
			nCurrentX += 1;
		}

		memdc2.DeleteDC();

		// Prepare next line...
		nCurrentX = 0;
		nCurrentY += nLineHeight;
		nCurrentY += 1;
		nLine++;
		i = j;
	}

	// Replace original
	memdc.SelectObject(hOldBmp);
	memdc.DeleteDC();

	DeleteObject(br);
	DeleteObject(m_hBitmap);

	m_hBitmap = hTmpBmp;
	m_SpriteWork.m_bBitmapChanged = true;

	NaDebugOut(L"Done.\n");
	theMain->Output(L"ReArrange Sprites: Done");

	PatchThemed(vecPatchData, dc);
}

void CMarioSpriteEditorDoc::PatchThemed(SpritePatchDataVector & vec, CDC *dc)
{
	wchar_t* szThemes[] = {
		L"Overworld",
		L"Underground",
		L"Underwater",
		L"Castle",
		L"Gray",
		L"Mushroom",
		L"GhostHouse",
		L"CastleUnderwater",
		L"AirShip",
		L"GhostHouse",

		L"Snow",
		L"Desert",
		L"Winter",
		nullptr,
	};

	for (int i = 0; ; i++) 
	{
		if (szThemes[i] == nullptr)
			break;
		PatchThemedBitmap(szThemes[i], vec, dc);
	}
}

void CMarioSpriteEditorDoc::PatchThemedBitmap(wchar_t *szTheme, SpritePatchDataVector & vec, CDC *dc)
{
	NaString strSprite = m_strFilename;
	NaString strBmp;
	if (strSprite.Right(7).CompareNoCase(L".sprite") == 0)
	{
		strBmp = strSprite;
		strBmp.ReplaceAll(L".sprite", L"");
		strBmp += szTheme;
		strBmp += L".bmp";
	}

	// Open Bitmap
	HBITMAP hBitmap;
	{
		// initialize gdi+
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR gdiplusToken;
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

		// load image
		{
			Gdiplus::Image *pGdiImage = Gdiplus::Image::FromFile(strBmp.wstr());
			if (pGdiImage->GetWidth() == 0)
			{
				NaDebugOut(L"File not found: %s\n", strBmp.wstr());
				Gdiplus::GdiplusShutdown(gdiplusToken);
				return;
			}
			NaDebugOut(L"File found: %s\n", strBmp.wstr());

			// converting to bitmap
			Gdiplus::Bitmap* pGdiBitmap = static_cast<Gdiplus::Bitmap*>(pGdiImage);
			pGdiBitmap->GetHBITMAP(Gdiplus::Color(0, 0, 0), &hBitmap);
			delete pGdiImage;
		}

		// shutdown gdi+
		Gdiplus::GdiplusShutdown(gdiplusToken);
	}

	// Make New bitmap
	CDC memdc;
	memdc.CreateCompatibleDC(dc);

	int nWidth = m_sizeBitmap.cx;
	int nHeight = m_sizeBitmap.cy;
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

	// Copy from org
	CDC memdc2;
	memdc2.CreateCompatibleDC(&memdc);
	HGDIOBJ hOldBmp2 = memdc2.SelectObject(hBitmap);

	SpritePatchDataVector::iterator it = vec.begin();
	for (; it != vec.end(); it++)
	{
		SpritePatchData data = *it;
		BitBlt(memdc,
			data.ptTo.x,
			data.ptTo.y,
			data.nWidth,
			data.nHeight,
			memdc2,
			data.ptFrom.x,
			data.ptFrom.y,
			SRCCOPY);
	}
	memdc2.DeleteDC();

	// Replace original
	memdc.SelectObject(hOldBmp);
	memdc.DeleteDC();

	DeleteObject(br);
	DeleteObject(hBitmap);
	hBitmap = hTmpBmp;

	// Save bitmap
	{
		// initialize gdi+
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR gdiplusToken;
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

		HPALETTE hPalette = (HPALETTE)GetStockObject(DEFAULT_PALETTE);

		//Gdiplus::Bitmap *pBitmap = new Gdiplus::Bitmap((HBITMAP)m_hBitmap, hPalette);
		Gdiplus::Bitmap *pBitmap = Gdiplus::Bitmap::FromHBITMAP(hBitmap, hPalette);

		CLSID clsid;
		NaString strFileName(strBmp);
		if (strFileName.Right(4).CompareNoCase(L".bmp") == 0)
			GetEncoderClsid(L"image/bmp", &clsid);

		// Make 24bpp clone
		{
			Gdiplus::Bitmap *pClone = pBitmap->Clone(0, 0, m_sizeBitmap.cx, m_sizeBitmap.cy, PixelFormat24bppRGB);
			delete pBitmap;
			pBitmap = pClone;
		}

		pBitmap->Save(strFileName.wstr(), &clsid, NULL);
		delete pBitmap;
		pBitmap = nullptr;

		theMain->Output(L"Save Themed-Bitmap: %s", strFileName);

		// shutdown gdi+
		Gdiplus::GdiplusShutdown(gdiplusToken);
	}

	DeleteObject(hBitmap);
}