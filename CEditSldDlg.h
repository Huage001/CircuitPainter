#pragma once


// CEditSldDlg 对话框

class CEditSldDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEditSldDlg)

public:
	CEditSldDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CEditSldDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOGEditSld };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_part1ID;
	double m_part1Res;
	int m_part2ID;
	double m_part2Res;
	int m_port1;
	int m_port2;
	int m_sldID;
};
