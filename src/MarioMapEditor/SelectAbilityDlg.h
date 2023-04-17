#pragma once

#include <vector>

// SelectAbilityDlg 대화 상자입니다.
class AbilityControl
{
public:
	AbilityControl(int c, int a)
	{
		m_nControl = c;
		m_nAbilityFlag = a;
	};
	int m_nControl;
	int m_nAbilityFlag;
};

class SelectAbilityDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SelectAbilityDlg)

public:
	SelectAbilityDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~SelectAbilityDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_ABILITY };
#endif

	int m_nAbilityFlag;
	std::vector<AbilityControl> m_vecAbilityControl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
