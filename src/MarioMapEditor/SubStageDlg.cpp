// SubStageDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MarioMapEditor.h"
#include "SubStageDlg.h"
#include "afxdialogex.h"


// SubStageDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(SubStageDlg, CDialogEx)

SubStageDlg::SubStageDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_SUBSTAGE, pParent)
	, m_nStage(0)
{
	m_nSubStageCnt = -1;
}

SubStageDlg::~SubStageDlg()
{
}

void SubStageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_MAIN, m_nStage);
}


BEGIN_MESSAGE_MAP(SubStageDlg, CDialogEx)
END_MESSAGE_MAP()


// SubStageDlg 메시지 처리기입니다.


BOOL SubStageDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	UpdateData(TRUE);

	if (m_nSubStageCnt < 1)
		GetDlgItem(IDC_RADIO_SUBSTAGE2)->EnableWindow(FALSE);
	if (m_nSubStageCnt < 2)
		GetDlgItem(IDC_RADIO_SUBSTAGE3)->EnableWindow(FALSE);
	if (m_nSubStageCnt < 3)
		GetDlgItem(IDC_RADIO_SUBSTAGE4)->EnableWindow(FALSE);

	// Removed the limitation that only the main stage and sub-stages could move to the pipe.

	/*
	if (m_nStage != 0)
	{
		GetDlgItem(IDC_RADIO_SUBSTAGE1)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_SUBSTAGE2)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_SUBSTAGE3)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_SUBSTAGE4)->EnableWindow(FALSE);

		int nCurrentSubStageId = -1;
		switch (m_nStage)
		{
		case 1: nCurrentSubStageId = IDC_RADIO_SUBSTAGE1; break;
		case 2: nCurrentSubStageId = IDC_RADIO_SUBSTAGE2; break;
		case 3: nCurrentSubStageId = IDC_RADIO_SUBSTAGE3; break;
		case 4: nCurrentSubStageId = IDC_RADIO_SUBSTAGE4; break;
		}

		GetDlgItem(nCurrentSubStageId)->EnableWindow(TRUE);
	}
	*/

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
