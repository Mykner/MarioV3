// SelectAbilityDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MarioMapEditor.h"
#include "SelectAbilityDlg.h"
#include "afxdialogex.h"

#include "Common/Common.h"


// SelectAbilityDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(SelectAbilityDlg, CDialogEx)

SelectAbilityDlg::SelectAbilityDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_ABILITY, pParent)
{
	m_nAbilityFlag = 0;
}

SelectAbilityDlg::~SelectAbilityDlg()
{
}

void SelectAbilityDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(SelectAbilityDlg, CDialogEx)
END_MESSAGE_MAP()


// SelectAbilityDlg 메시지 처리기입니다.


BOOL SelectAbilityDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_vecAbilityControl.push_back(AbilityControl(IDC_CHECK_0001, ABILITY_CARRYOBJECT));
	m_vecAbilityControl.push_back(AbilityControl(IDC_CHECK_0002, ABILITY_SPINJUMP));
	m_vecAbilityControl.push_back(AbilityControl(IDC_CHECK_0004, ABILITY_AUTOACCEL));
	m_vecAbilityControl.push_back(AbilityControl(IDC_CHECK_0008, ABILITY_AUTORUN));

	for (const auto &ac : m_vecAbilityControl)
	{
		if (m_nAbilityFlag & ac.m_nAbilityFlag)
			((CButton*)GetDlgItem(ac.m_nControl))->SetCheck(TRUE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void SelectAbilityDlg::OnOK()
{
	m_nAbilityFlag = 0;
	for (const auto &ac : m_vecAbilityControl)
	{
		if (((CButton*)GetDlgItem(ac.m_nControl))->GetCheck())
			m_nAbilityFlag |= ac.m_nAbilityFlag;
	}

	CDialogEx::OnOK();
}
