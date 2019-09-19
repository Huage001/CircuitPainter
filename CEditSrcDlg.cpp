// CEditSrcDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "Circuit_Painter.h"
#include "CEditSrcDlg.h"
#include "afxdialogex.h"


// CEditSrcDlg 对话框

IMPLEMENT_DYNAMIC(CEditSrcDlg, CDialogEx)

CEditSrcDlg::CEditSrcDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOGEditSrc, pParent)
	, m_NegID(0)
	, m_NegNab(_T(""))
	, m_NegNum(0)
	, m_PosID(0)
	, m_PosNab(_T(""))
	, m_PosNum(0)
	, m_SrcVol(0.0)
{

}

CEditSrcDlg::~CEditSrcDlg()
{
}

void CEditSrcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDITNegID, m_NegID);
	DDV_MinMaxInt(pDX, m_NegID, 0, INT_MAX);
	DDX_Text(pDX, IDC_EDITNegNab, m_NegNab);
	DDX_Text(pDX, IDC_EDITNegNum, m_NegNum);
	DDV_MinMaxInt(pDX, m_NegNum, 0, INT_MAX);
	DDX_Text(pDX, IDC_EditPosID, m_PosID);
	DDV_MinMaxInt(pDX, m_PosID, 0, INT_MAX);
	DDX_Text(pDX, IDC_EDITPosNab, m_PosNab);
	DDX_Text(pDX, IDC_EDITPosNum, m_PosNum);
	DDV_MinMaxInt(pDX, m_PosNum, 0, INT_MAX);
	DDX_Text(pDX, IDC_EDITSrcVol, m_SrcVol);
	DDV_MinMaxDouble(pDX, m_SrcVol, 0, INT_MAX);
}


BEGIN_MESSAGE_MAP(CEditSrcDlg, CDialogEx)
END_MESSAGE_MAP()


// CEditSrcDlg 消息处理程序
