// CEditResDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "Circuit_Painter.h"
#include "CEditResDlg.h"
#include "afxdialogex.h"


// CEditResDlg 对话框

IMPLEMENT_DYNAMIC(CEditResDlg, CDialogEx)

CEditResDlg::CEditResDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOGEditRes, pParent)
	, m_ID(0)
	, m_port1(0)
	, m_port2(0)
	, m_res(0)
{

}

CEditResDlg::~CEditResDlg()
{
}

void CEditResDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDITMyID, m_ID);
	DDV_MinMaxInt(pDX, m_ID, 0, INT_MAX);
	DDX_Text(pDX, IDC_EDITPort1ID, m_port1);
	DDV_MinMaxInt(pDX, m_port1, 0, INT_MAX);
	DDX_Text(pDX, IDC_EDITPort2ID, m_port2);
	DDV_MinMaxInt(pDX, m_port2, 0, INT_MAX);
	DDX_Text(pDX, IDC_EDITRes, m_res);
	DDV_MinMaxDouble(pDX, m_res, 0, DBL_MAX);
}


BEGIN_MESSAGE_MAP(CEditResDlg, CDialogEx)
END_MESSAGE_MAP()


// CEditResDlg 消息处理程序
