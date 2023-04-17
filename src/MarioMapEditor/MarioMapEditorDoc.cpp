
// MarioMapEditorDoc.cpp : CMarioMapEditorDoc 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "MarioMapEditor.h"
#endif

#include "MarioMapEditorDoc.h"

#include <propkey.h>

#include "Game.h"
#include "Stage.h"
#include "DesignStage.h"
#include "MapObjectBase.h"
#include "GameObjectBase.h"
#include "MapObjectPipe.h"
#include "json.h"
#include "MainFrm.h"
#include "ResourceHolder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMarioMapEditorDoc

IMPLEMENT_DYNCREATE(CMarioMapEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CMarioMapEditorDoc, CDocument)
END_MESSAGE_MAP()


// CMarioMapEditorDoc 생성/소멸

CMarioMapEditorDoc::CMarioMapEditorDoc()
{
	m_pGame = new DesignGame;
	m_pGame->m_pResourceHolder = ((CMainFrame*)theApp.GetMainWnd())->m_pResourceHolder;

	m_strPathName = L"";
	m_bCreated = false;
}

CMarioMapEditorDoc::~CMarioMapEditorDoc()
{
	ResourceHolder *pHolder = m_pGame->m_pResourceHolder;
	int nRef = pHolder->m_nRef - 1;
	
	m_pGame->OnExit();
	m_pGame->Release();	
	delete m_pGame;
	m_pGame = nullptr;

	if (nRef == 0)
	{
		((CMainFrame*)theApp.GetMainWnd())->m_pResourceHolder = new ResourceHolder;
		((CMainFrame*)theApp.GetMainWnd())->m_pResourceHolder->m_bUseDirectX = true;
	}
}

BOOL CMarioMapEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.
	m_bCreated = true;

	return TRUE;
}

// CMarioMapEditorDoc serialization

#ifdef SHARED_HANDLERS

// 축소판 그림을 지원합니다.
void CMarioMapEditorDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
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
void CMarioMapEditorDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CMarioMapEditorDoc::SetSearchContent(const CString& value)
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

// CMarioMapEditorDoc 진단

#ifdef _DEBUG
void CMarioMapEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMarioMapEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMarioMapEditorDoc 명령


BOOL CMarioMapEditorDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	DesignStage *pStage = (DesignStage*)m_pGame->m_pCurStage;
	m_strPathName = (lpszPathName);

	//pStage->LoadFile(strPathName.cstr());

	return TRUE;
}


BOOL CMarioMapEditorDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	DesignStage *pStage = (DesignStage*)m_pGame->m_pCurStage;
	pStage = (DesignStage*)pStage->GetTopStage();

	CFile file;
	CFileException Err;
	file.Open(lpszPathName, CFile::modeWrite | CFile::modeCreate | CFile::shareDenyNone, &Err);
	
	Json::Value j;
	{
		pStage->SaveJsonMap(j);

		std::string stdstr = j.toStyledString();
		file.Write(stdstr.c_str(), stdstr.length());

		m_strPathName = (lpszPathName);
	}

	file.Close();
	SetModifiedFlag(FALSE);

	return TRUE;
}


void CMarioMapEditorDoc::SetModifiedFlag(BOOL bModified)
{
	ASSERT(this != NULL); 
	m_bModified = bModified;

	if (bModified == TRUE)
	{
		CString str = GetTitle();
		if (str.Right(2).Compare(L" *") != 0)
		{
			CString strNew;
			strNew.Format(L"%s *", str);
			SetTitle(strNew);
		}
	}
	else
	{
		CString str = GetTitle();
		if (str.Right(2).Compare(L" *") == 0)
		{
			CString strNew = str.Left(str.GetLength() - 2);
			SetTitle(strNew);
		}
	}
}