// CEditTypeDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "Circuit_Painter.h"
#include "CEditTypeDlg.h"
#include "afxdialogex.h"


// CEditTypeDlg 对话框

IMPLEMENT_DYNAMIC(CEditTypeDlg, CDialogEx)

CEditTypeDlg::CEditTypeDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOGEditType, pParent)
	, m_type("")
{

}

CEditTypeDlg::~CEditTypeDlg()
{
}

void CEditTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_CBString(pDX, IDC_COMBOtype, m_type);
}


BEGIN_MESSAGE_MAP(CEditTypeDlg, CDialogEx)
END_MESSAGE_MAP()


// CEditTypeDlg 消息处理程序

BOOL CEditTypeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_type = "导线";

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
