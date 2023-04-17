#pragma once


// CreatorCommentDlg 대화 상자입니다.

class CreatorCommentDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CreatorCommentDlg)

public:
	CreatorCommentDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CreatorCommentDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_CREATOR_COMMENT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strComment;
};
