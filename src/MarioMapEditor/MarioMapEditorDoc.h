
// MarioMapEditorDoc.h : CMarioMapEditorDoc 클래스의 인터페이스
//


#pragma once

#include "DesignGame.h"

class CMarioMapEditorDoc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CMarioMapEditorDoc();
	DECLARE_DYNCREATE(CMarioMapEditorDoc)

// 특성입니다.
public:
	bool m_bCreated;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL OnNewDocument();
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 구현입니다.
public:
	virtual ~CMarioMapEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	DesignGame *m_pGame;
	NaString m_strPathName;
	virtual void SetModifiedFlag(BOOL bModified = TRUE);

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 검색 처리기에 대한 검색 콘텐츠를 설정하는 도우미 함수
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
};
