// CEditWireDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "Circuit_Painter.h"
#include "CEditWireDlg.h"
#include "afxdialogex.h"


// CEditWireDlg 对话框

IMPLEMENT_DYNAMIC(CEditWireDlg, CDialogEx)

CEditWireDlg::CEditWireDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOGEditWire, pParent)
	, m_port1(0)
	, m_port2(0)
{

}

CEditWireDlg::~CEditWireDlg()
{
}

void CEditWireDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDITPort1, m_port1);
	DDV_MinMaxInt(pDX, m_port1, 0, 65535);
	DDX_Text(pDX, IDC_EDITPort2, m_port2);
	DDV_MinMaxInt(pDX, m_port2, 0, 65535);
}


BEGIN_MESSAGE_MAP(CEditWireDlg, CDialogEx)
END_MESSAGE_MAP()


// CEditWireDlg 消息处理程序
