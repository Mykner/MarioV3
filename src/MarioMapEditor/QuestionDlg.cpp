// QuestionDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MarioMapEditor.h"
#include "QuestionDlg.h"
#include "afxdialogex.h"


// QuestionDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(QuestionDlg, CDialogEx)

QuestionDlg::QuestionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_QNA, pParent)
	, m_strQuestion(_T(""))
	, m_strAnswer(_T(""))
{

}

QuestionDlg::~QuestionDlg()
{
}

void QuestionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_QUESTION, m_strQuestion);
	DDX_Text(pDX, IDC_EDIT_ANSWER, m_strAnswer);
}


BEGIN_MESSAGE_MAP(QuestionDlg, CDialogEx)
END_MESSAGE_MAP()


// QuestionDlg 메시지 처리기입니다.
