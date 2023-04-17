// CreatorCommentDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MarioMapEditor.h"
#include "CreatorCommentDlg.h"
#include "afxdialogex.h"


// CreatorCommentDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CreatorCommentDlg, CDialogEx)

CreatorCommentDlg::CreatorCommentDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_CREATOR_COMMENT, pParent)
	, m_strComment(_T(""))
{

}

CreatorCommentDlg::~CreatorCommentDlg()
{
}

void CreatorCommentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_COMMENT, m_strComment);
}


BEGIN_MESSAGE_MAP(CreatorCommentDlg, CDialogEx)
END_MESSAGE_MAP()


// CreatorCommentDlg 메시지 처리기입니다.
