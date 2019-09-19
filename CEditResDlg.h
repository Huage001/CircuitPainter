#pragma once


// CEditResDlg 对话框

class CEditResDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEditResDlg)

public:
	CEditResDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CEditResDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOGEditRes };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_ID;
	int m_port1;
	int m_port2;
	double m_res;
};
