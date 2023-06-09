#pragma once


// QuestionDlg 대화 상자입니다.

class QuestionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(QuestionDlg)

public:
	QuestionDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~QuestionDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_QNA };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strQuestion;
	CString m_strAnswer;
};
