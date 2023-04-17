#pragma once


// SubStageDlg 대화 상자입니다.

class SubStageDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SubStageDlg)

public:
	SubStageDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~SubStageDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_SUBSTAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int m_nStage;
	int m_nSubStageCnt;
	virtual BOOL OnInitDialog();
};
