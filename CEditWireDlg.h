#pragma once


// CEditWireDlg 对话框

class CEditWireDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEditWireDlg)

public:
	CEditWireDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CEditWireDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOGEditWire };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_port1;
	int m_port2;
};
