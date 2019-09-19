
// Circuit_PainterView.h: CCircuitPainterView 类的接口
//

#pragma once
#include "Circuit.h"
#include<queue>
using namespace std;

class CCircuitPainterView : public CView
{
protected: // 仅从序列化创建
	CCircuitPainterView() noexcept;
	DECLARE_DYNCREATE(CCircuitPainterView)

// 特性
public:
	CCircuitPainterDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CCircuitPainterView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void classification();
	afx_msg bool SameArea(Dot a, Dot b);
	void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	bool mouseDown, newPart, have_res;
	Stroke* stroke;
	CPoint cur_point, last_point;
	double endTime;
	double startTime;
	Part *part, *cur_part;
	unsigned char r, g, b;
	CFont font;
	Circuit* circuit;
	vector<string> res;
	vector<Dot> key_point;
	pair<Part*,int>** key_point_table;
	pair<Part*, Dot>** all_point_table;
	queue<Part*> recognize_queue;
	int iScrWidth, iScrHeight, cur_code;
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnStartCalculate();
};

#ifndef _DEBUG  // Circuit_PainterView.cpp 中的调试版本
inline CCircuitPainterDoc* CCircuitPainterView::GetDocument() const
   { return reinterpret_cast<CCircuitPainterDoc*>(m_pDocument); }
#endif

