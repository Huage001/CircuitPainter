#pragma once


// CEditTypeDlg 对话框

class CEditTypeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEditTypeDlg)

public:
	CEditTypeDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CEditTypeDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOGEditType };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_type;
	virtual BOOL OnInitDialog();
};
