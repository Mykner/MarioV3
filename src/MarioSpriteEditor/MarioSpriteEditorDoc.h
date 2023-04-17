#pragma once

#include <vector>
#include "Sprite.h"

#include "SpriteWork.h"
#include "SpritePatchData.h"

#define LOGBASE			NaDebugOut(L"[%s:%05d] ", __FUNCTIONW__, __LINE__); NaDebugOut
#define LOG(arg, ...)	LOGBASE(arg, __VA_ARGS__);

class CMarioSpriteEditorDoc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CMarioSpriteEditorDoc();
	DECLARE_DYNCREATE(CMarioSpriteEditorDoc)

// 특성입니다.
public:

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 구현입니다.
public:
	virtual ~CMarioSpriteEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	HBITMAP m_hBitmap;
	SIZE m_sizeBitmap;
	Sprite m_Sprite;
	CRect FindSpriteArea(int x, int y);
	SpriteWork m_SpriteWork;
	static int GetEncoderClsid(const wchar_t *format, CLSID *pClsid);
	void RearrangeSprites(CDC *dc, int nMode = 0);
	void PatchThemed(SpritePatchDataVector &vec, CDC *dc);
	void PatchThemedBitmap(wchar_t* szTheme, SpritePatchDataVector &vec, CDC *dc);
	NaString m_strFilename;
	NaString m_strBitmapFilename;

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 검색 처리기에 대한 검색 콘텐츠를 설정하는 도우미 함수
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
};
