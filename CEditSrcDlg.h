#pragma once


// CEditSrcDlg 对话框

class CEditSrcDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEditSrcDlg)

public:
	CEditSrcDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CEditSrcDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOGEditSrc };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_NegID;
	CString m_NegNab;
	int m_NegNum;
	int m_PosID;
	CString m_PosNab;
	int m_PosNum;
	double m_SrcVol;
};
