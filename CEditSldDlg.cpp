// CEditSldDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "Circuit_Painter.h"
#include "CEditSldDlg.h"
#include "afxdialogex.h"


// CEditSldDlg 对话框

IMPLEMENT_DYNAMIC(CEditSldDlg, CDialogEx)

CEditSldDlg::CEditSldDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOGEditSld, pParent)
	, m_part1ID(0)
	, m_part1Res(0)
	, m_part2ID(0)
	, m_part2Res(0)
	, m_port1(0)
	, m_port2(0)
	, m_sldID(0)
{

}

CEditSldDlg::~CEditSldDlg()
{
}

void CEditSldDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDITPart1ID, m_part1ID);
	DDV_MinMaxInt(pDX, m_part1ID, 0, INT_MAX);
	DDX_Text(pDX, IDC_EDITPart1Res, m_part1Res);
	DDV_MinMaxDouble(pDX, m_part1Res, 0, DBL_MAX);
	DDX_Text(pDX, IDC_EDITPart2ID, m_part2ID);
	DDV_MinMaxInt(pDX, m_part2ID, 0, INT_MAX);
	DDX_Text(pDX, IDC_EDITPart2Res, m_part2Res);
	DDV_MinMaxDouble(pDX, m_part2Res, 0, DBL_MAX);
	DDX_Text(pDX, IDC_EDITPort1, m_port1);
	DDV_MinMaxInt(pDX, m_port1, 0, INT_MAX);
	DDX_Text(pDX, IDC_EDITPort2, m_port2);
	DDV_MinMaxInt(pDX, m_port2, 0, INT_MAX);
	DDX_Text(pDX, IDC_EDITSldID, m_sldID);
	DDV_MinMaxInt(pDX, m_sldID, 0, INT_MAX);
}


BEGIN_MESSAGE_MAP(CEditSldDlg, CDialogEx)
END_MESSAGE_MAP()


// CEditSldDlg 消息处理程序
