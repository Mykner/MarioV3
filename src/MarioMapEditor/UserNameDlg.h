#pragma once
#include "afxwin.h"


// UserNameDlg 대화 상자입니다.

class UserNameDlg : public CDialogEx
{
	DECLARE_DYNAMIC(UserNameDlg)

public:
	UserNameDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~UserNameDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_USERNAME };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strName;
};
