
// Circuit_PainterView.cpp: CCircuitPainterView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "Circuit_Painter.h"
#endif

#include "Circuit_PainterDoc.h"
#include "Circuit_PainterView.h"
#include "CEditTypeDlg.h"
#include "CEditWireDlg.h"
#include "CEditSrcDlg.h"
#include "CEditResDlg.h"
#include "CEditSldDlg.h"
#include <ctime>
#include <cstdlib>
#include <sstream>
#include <cmath>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MAX_TIMEVAL 1.0
#define RADIUS 5
#define FONT_SIZE 20

// CCircuitPainterView

IMPLEMENT_DYNCREATE(CCircuitPainterView, CView)

BEGIN_MESSAGE_MAP(CCircuitPainterView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CCircuitPainterView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MBUTTONUP()
	ON_COMMAND(ID_32771, &CCircuitPainterView::OnStartCalculate)
END_MESSAGE_MAP()

// CCircuitPainterView 构造/析构

CCircuitPainterView::CCircuitPainterView() noexcept
{
	// TODO: 在此处添加构造代码
	mouseDown = false;
	stroke = NULL;
	part = NULL;
	cur_part = NULL;
	circuit = NULL;
	endTime = 0.0;
	startTime = 0.0;
	r = g = b = 0;
	cur_code = 0;
	newPart = false;
	have_res = false;
	CString fontType("华文楷体");
	font.CreatePointFont(FONT_SIZE * 5, fontType);

	//初始化像素表
	CDC* pDC = GetDC();
	HDC hdc = pDC->m_hDC;
	iScrWidth = GetDeviceCaps(hdc, HORZRES);
	iScrHeight = GetDeviceCaps(hdc, VERTRES);
	key_point_table = new pair<Part*,int> *[iScrWidth];
	all_point_table = new pair<Part*,Dot> *[iScrWidth];
	for (int i = 0; i < iScrWidth; i++)
	{
		key_point_table[i] = new pair<Part*,int> [iScrHeight]();
		all_point_table[i] = new pair<Part*,Dot>  [iScrHeight]();
	}
}

CCircuitPainterView::~CCircuitPainterView()
{
}

BOOL CCircuitPainterView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CCircuitPainterView 绘图

void CCircuitPainterView::OnDraw(CDC* pDC)
{
	CCircuitPainterDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	
	CPen pen(PS_SOLID, 3, RGB(r, g, b));
	pDC->SelectObject(&pen);
	if (stroke != NULL)
	{
		int n = stroke->dots.size();
		if (n >= 2)
		{
			//备用方案：如果效果不好，则自己实现画线代码，需要把经历的每一像素映射到part上
			pDC->MoveTo(stroke->dots[n - 2].first, stroke->dots[n - 2].second);
			pDC->LineTo(stroke->dots[n - 1].first, stroke->dots[n - 1].second);
		}
	}
	if (newPart)
	{
		CString temp_str;
		temp_str.Format(_T("元件%d"), pDoc->graph->all_parts.size());
		pDC->SelectObject(&font);
		pDC->SetBkColor(RGB(200, 200, 200));
		pDC->SetTextColor(RGB(r, g, b));
		pDC->TextOut(30, pDoc->graph->all_parts.size() * FONT_SIZE, temp_str);
	}
	if (have_res)
	{
		pDC->SelectObject(&font);
		pDC->SetBkColor(RGB(0, 0, 0));
		pDC->SetTextColor(RGB(255, 255, 255));
		for (int i = 0; i < res.size(); i++)
		{
			CString temp_str(res[i].c_str());
			pDC->TextOut(30, 325 + i * FONT_SIZE, temp_str);
		}
	}
}


// CCircuitPainterView 打印


void CCircuitPainterView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CCircuitPainterView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CCircuitPainterView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CCircuitPainterView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CCircuitPainterView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CCircuitPainterView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CCircuitPainterView 诊断

#ifdef _DEBUG
void CCircuitPainterView::AssertValid() const
{
	CView::AssertValid();
}

void CCircuitPainterView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCircuitPainterDoc* CCircuitPainterView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCircuitPainterDoc)));
	return (CCircuitPainterDoc*)m_pDocument;
}
#endif //_DEBUG


// CCircuitPainterView 消息处理程序

bool CCircuitPainterView::SameArea(Dot a, Dot b)
{
	if (abs(a.first - b.first) <= 2 * RADIUS && abs(a.second - b.second <= 2 * RADIUS))
		return true;
	else
		return false;
}


//TODO：分类线程
void CCircuitPainterView::classification()
{
	while(1)
	{
		while (recognize_queue.empty());	//while(工作队列为空);
		Part* target = recognize_queue.front();	//取队首part
		recognize_queue.pop();	//出队
		int res;
		while(1)
		{
			bool stop=false;
			while(mouseDown);
			double timer = clock();
			while(1)
			{
				if(mouseDown)
					break;
				if((clock()-timer)/CLOCKS_PER_SEC<MAX_TIMEVAL)
					continue;
				else
				{
					//TODO: 开启tensorflow识别	target->type=tensorflow();	
					stop=true;
					break;
				}
			}
			if(stop)
				break;
		}
		CDC* pDC = GetDC();
		CCircuitPainterDoc* pDoc = GetDocument();
		if (target->type == TYPE_WIRE)//如果返回的是导线，
		{
			Dot start_end[2];
			start_end[0] = target->strokes[0]->dots[0];
			start_end[1] = target->strokes.back()->dots.back();
			for (int index = 0; index < 2; index++)
			{
				Part* connect = all_point_table[start_end[index].first][start_end[index].second].first;
				if (connect != NULL)//在所有点表中查找该笔画起点与终点对应的part，如果找到，
				{
					if (connect->type == TYPE_WIRE)//如果对方也是导线，
					{
						if (key_point_table[start_end[index].first][start_end[index].second].first == NULL)//说明在关键点表中没有匹配到，需要新增关键点
						{
							//新增关键点将它的相邻区域点加入到关键点表
							for (int j = start_end[index].first - RADIUS; j <= start_end[index].first + RADIUS; j++)
								for (int k = start_end[index].second - RADIUS; k <= start_end[index].second + RADIUS; k++)
									if (j >= 0 && j < iScrWidth && k >= 0 && k < iScrHeight)
									{
										key_point_table[j][k] = make_pair(part, cur_code);
										pDC->SetPixel(j, k, RGB(255, 0, 0));
									}
							key_point.push_back(start_end[index]);
							//将原导线拆成两半，更新信息，注意此时要同步更新原导线的part
							Dot cut_point = all_point_table[start_end[index].first][start_end[index].second].second;
							Part* new_part = connect->cut(cut_point, cur_code);
							pDoc->graph->all_parts.push_back(new_part);
							//这部分处理原导线端口和新导线端口的对应关系，需要好好理解
							Dot port0 = key_point[connect->ports[0]], port1 = key_point[connect->ports[1]];
							Dot start = key_point[key_point_table[connect->strokes[0]->dots[0].first][connect->strokes[0]->dots[0].second].second], end = key_point[key_point_table[new_part->strokes.back()->dots.back().first][new_part->strokes.back()->dots.back().second].second];
							if (SameArea(port0, start) && SameArea(port1, end))
							{
								new_part->ports[0] = cur_code;
								new_part->ports[1] = connect->ports[1];
								connect->ports[1] = cur_code;
							}
							else if (SameArea(port1, start) && SameArea(port0, end))
							{
								new_part->ports[0] = cur_code;
								new_part->ports[1] = connect->ports[0];
								connect->ports[0] = cur_code;
							}
							else
								assert(0);
							cur_code++;
							//重绘新分出来的导线
							for (int s = 0; s < new_part->strokes.size(); s++)
								for (int i = 0; i < new_part->strokes[s]->dots.size(); i++)
									for (int j = new_part->strokes[s]->dots[i].first - RADIUS; j <= new_part->strokes[s]->dots[i].first + RADIUS; j++)
										for (int k = new_part->strokes[s]->dots[i].second - RADIUS; k <= new_part->strokes[s]->dots[i].second + RADIUS; k++)
											if (j >= 0 && j < iScrWidth && k >= 0 && k < iScrHeight)
											{
												all_point_table[j][k] = make_pair(new_part, stroke->dots[i]);
												pDC->SetPixel(j, k, RGB(0, 0, 255));
											}
							int old_code = key_point_table[new_part->strokes.back()->dots.back().first][new_part->strokes.back()->dots.back().second].second;
							for (int j = new_part->strokes.back()->dots.back().first - RADIUS; j <= new_part->strokes.back()->dots.back().first + RADIUS; j++)
								for (int k = new_part->strokes.back()->dots.back().second - RADIUS; k <= new_part->strokes.back()->dots.back().second + RADIUS; k++)
									if (j >= 0 && j < iScrWidth && k >= 0 && k < iScrHeight)
									{
										key_point_table[j][k] = make_pair(new_part, old_code);
										pDC->SetPixel(j, k, RGB(255, 0, 0));
									}
						}
					}
					//否则（对方不是导线）
						//如果对方的引脚满了，
							//报错，
						//否则，
							//加入该引脚，更新对方的引脚信息和这条导线的信息
				}
				else//否则（没找到对应Part）
				{
					//新增关键点将它的相邻区域点加入到关键点表
					for (int j = start_end[index].first - RADIUS; j <= start_end[index].first + RADIUS; j++)
						for (int k = start_end[index].second - RADIUS; k <= start_end[index].second + RADIUS; k++)
							if (j >= 0 && j < iScrWidth && k >= 0 && k < iScrHeight)
							{
								key_point_table[j][k] = make_pair(part, cur_code);
								pDC->SetPixel(j, k, RGB(255, 0, 0));
							}
					key_point.push_back(start_end[index]);
					cur_code++;
				}
			}
		}
		//处理该元件的临时节点空间，
		//如果返回的是导线
			//将原导线拆成两半，注意此时要同步更新原导线的part、stroke并重绘
		//否则
			//如果引脚满了，
				//报错，
			//否则，
				//加入该引脚，该元件的引脚和与之相连的导线信息都将更新
		//把该笔画所有的点相邻区域的点加入到所有点阵哈希表并映射到相应part
		for(int s=0;s<target->strokes.size();s++)
			for (int i = 0; i < target->strokes[s]->dots.size(); i++)
				for (int j = target->strokes[s]->dots[i].first - RADIUS; j <= target->strokes[s]->dots[i].first + RADIUS; j++)
					for (int k = target->strokes[s]->dots[i].second - RADIUS; k <= target->strokes[s]->dots[i].second + RADIUS; k++)
						if (j >= 0 && j < iScrWidth && k >= 0 && k < iScrHeight)
						{
							all_point_table[j][k] = make_pair(part, stroke->dots[i]);
							pDC->SetPixel(j, k, RGB(0, 0, 255));
						}
	}
}



void CCircuitPainterView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	mouseDown = true;
	CCircuitPainterDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	Dot temp(point.x, point.y);

	stroke = new Stroke();
	stroke->dots.push_back(temp);

	startTime = clock();
	if (part == NULL || (startTime - endTime) / CLOCKS_PER_SEC > MAX_TIMEVAL)
	{
		part = new Part();
		newPart = true;
		r = rand() & 0xff;
		g = rand() & 0xff;
		b = rand() & 0xff;
		pDoc->graph->all_parts.push_back(part);
		//将这个part送入工作队列
		recognize_queue.push(part);
	}
	part->strokes.push_back(stroke);

	//在关键点表中查找该点，如果查到，则加入到该元件（此时该元件还未分类）临时节点空间中，等待识别之后处理
	if (key_point_table[point.x][point.y].first != NULL)
		part->temp_pin.insert(key_point_table[point.x][point.y]);
	cur_point = point;
	last_point = point;
	CView::OnLButtonDown(nFlags, point);
}


void CCircuitPainterView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (mouseDown)
	{
		CCircuitPainterDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		if (!pDoc)
			return;
		Dot temp(point.x, point.y);
		stroke->dots.push_back(temp);
		cur_point = point;
		Invalidate(false);
		//在关键点表中查找该点，如果查到，则加入到该元件（此时该元件还未分类）临时节点空间中，等待识别之后处理
		if (key_point_table[point.x][point.y].first != NULL)
			part->temp_pin.insert(key_point_table[point.x][point.y]);
		last_point = point;
	}
	CView::OnMouseMove(nFlags, point);
}


void CCircuitPainterView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	mouseDown = false;
	//在关键点表中查找该点，如果查到，则加入到该元件（此时该元件还未分类）临时节点空间中，等待识别之后处理
	if (key_point_table[point.x][point.y].first != NULL)
		part->temp_pin.insert(key_point_table[point.x][point.y]);
	endTime = clock();
	CView::OnLButtonUp(nFlags, point);
}


void CCircuitPainterView::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CCircuitPainterDoc* pDoc = GetDocument();
	if (!(point.y > FONT_SIZE && point.y < FONT_SIZE * (1 + pDoc->graph->all_parts.size()) && point.x>30 && point.x < 80))
		return;
	cur_part = pDoc->graph->all_parts[point.y / FONT_SIZE - 1];
	CEditTypeDlg dlg;
	if (dlg.DoModal() != IDOK)
		return;
	if (dlg.m_type == "导线")
	{
		cur_part->type = TYPE_WIRE;
		CEditWireDlg dlg2;
		if (dlg2.DoModal() != IDOK)
			return;
		cur_part->ports[0] = dlg2.m_port1;
		cur_part->ports[1] = dlg2.m_port2;
		pDoc->graph->max_id = max(pDoc->graph->max_id, dlg2.m_port1);
		pDoc->graph->max_id = max(pDoc->graph->max_id, dlg2.m_port2);
	}
	else if (dlg.m_type == "电源")
	{
		cur_part->type = TYPE_SOURCE;
		CEditSrcDlg dlg2;
		if (dlg2.DoModal() != IDOK)
			return;
		cur_part->src_node[0] = new SrcNode(true);
		cur_part->src_node[0]->m_id = dlg2.m_PosID;
		pDoc->graph->max_id = max(pDoc->graph->max_id, dlg2.m_PosID);
		cur_part->src_node[0]->num_of_neigh = dlg2.m_PosNum;
		cur_part->src_node[0]->m_volt = dlg2.m_SrcVol;
		cur_part->src_node[0]->m_neighbors.resize(dlg2.m_PosNum);
		string temp = CT2A(dlg2.m_PosNab.GetBuffer());
		stringstream input;
		input << temp;
		for (int i = 0; i < dlg2.m_PosNum; i++)
		{
			input >> cur_part->src_node[0]->m_neighbors[i];
			pDoc->graph->max_id = max(pDoc->graph->max_id, cur_part->src_node[0]->m_neighbors[i]);
		}
		input.clear();
		cur_part->src_node[1] = new SrcNode(false);
		cur_part->src_node[1]->m_id = dlg2.m_NegID;
		pDoc->graph->max_id = max(pDoc->graph->max_id, dlg2.m_NegID);
		cur_part->src_node[1]->num_of_neigh = dlg2.m_NegNum;
		cur_part->src_node[1]->m_volt = dlg2.m_SrcVol;
		cur_part->src_node[1]->m_neighbors.resize(dlg2.m_NegNum);
		temp = CT2A(dlg2.m_NegNab.GetBuffer());
		input << temp;
		for (int i = 0; i < dlg2.m_NegNum; i++)
		{
			input >> cur_part->src_node[1]->m_neighbors[i];
			pDoc->graph->max_id = max(pDoc->graph->max_id, cur_part->src_node[1]->m_neighbors[i]);
		}
	}
	else if (dlg.m_type == "滑动变阻器")
	{
		cur_part->type = TYPE_SLD;
		CEditSldDlg dlg2;
		if (dlg2.DoModal() != IDOK)
			return;
		cur_part->elem_node[0] = new ElemNode();
		cur_part->elem_node[0]->elem_type = RES;
		cur_part->elem_node[0]->in = dlg2.m_port1;
		pDoc->graph->max_id = max(pDoc->graph->max_id, dlg2.m_port1);
		cur_part->elem_node[0]->m_id = dlg2.m_part1ID;
		pDoc->graph->max_id = max(pDoc->graph->max_id, dlg2.m_part1ID);
		cur_part->elem_node[0]->num_of_neigh = 2;
		cur_part->elem_node[0]->m_neighbors.resize(2);
		cur_part->elem_node[0]->m_neighbors[0] = dlg2.m_port1;
		cur_part->elem_node[0]->m_neighbors[1] = dlg2.m_sldID;
		pDoc->graph->max_id = max(pDoc->graph->max_id, dlg2.m_sldID);
		cur_part->elem_node[0]->m_res = dlg2.m_part1Res;
		cur_part->elem_node[0]->out = dlg2.m_sldID;
		cur_part->elem_node[1] = new ElemNode();
		cur_part->elem_node[1]->elem_type = RES;
		cur_part->elem_node[1]->in = dlg2.m_sldID;
		cur_part->elem_node[1]->m_id = dlg2.m_part2ID;
		pDoc->graph->max_id = max(pDoc->graph->max_id, dlg2.m_part2ID);
		cur_part->elem_node[1]->num_of_neigh = 2;
		cur_part->elem_node[1]->m_neighbors.resize(2);
		cur_part->elem_node[1]->m_neighbors[0] = dlg2.m_sldID;
		cur_part->elem_node[1]->m_neighbors[1] = dlg2.m_port2;
		pDoc->graph->max_id = max(pDoc->graph->max_id, dlg2.m_port2);
		cur_part->elem_node[1]->m_res = dlg2.m_part2Res;
		cur_part->elem_node[1]->out = dlg2.m_port2;
	}
	else
	{
		CEditResDlg dlg2;
		if (dlg2.DoModal() != IDOK)
			return;
		cur_part->elem_node[0] = new ElemNode();
		cur_part->elem_node[0]->num_of_neigh = 2;
		cur_part->elem_node[0]->m_neighbors.resize(2);
		cur_part->elem_node[0]->m_neighbors[0] = dlg2.m_port1;
		cur_part->elem_node[0]->m_neighbors[1] = dlg2.m_port2;
		cur_part->elem_node[0]->in = dlg2.m_port1;
		cur_part->elem_node[0]->out = dlg2.m_port2;
		cur_part->elem_node[0]->m_id = dlg2.m_ID;
		cur_part->elem_node[0]->m_res = dlg2.m_res;
		pDoc->graph->max_id = max(pDoc->graph->max_id, dlg2.m_port1);
		pDoc->graph->max_id = max(pDoc->graph->max_id, dlg2.m_port2);
		pDoc->graph->max_id = max(pDoc->graph->max_id, dlg2.m_ID);
		if (dlg.m_type == "电阻")
		{
			cur_part->type = TYPE_RES;
			cur_part->elem_node[0]->elem_type = RES;
		}
		else if (dlg.m_type == "电流表")
		{
			cur_part->type = TYPE_CUR;
			cur_part->elem_node[0]->elem_type = CUR;
		}
		else if (dlg.m_type == "电压表")
		{
			cur_part->type = TYPE_VOL;
			cur_part->elem_node[0]->elem_type = VOL;
		}
	}
	CView::OnMButtonUp(nFlags, point);
}


void CCircuitPainterView::OnStartCalculate()
{
	// TODO: 在此添加命令处理程序代码
	CCircuitPainterDoc* pDoc = GetDocument();
	circuit = new Circuit(pDoc->graph);
	vector<vector<int>>routes;
	circuit->dfs(circuit->m_src_p->m_id, circuit->m_src_n->m_id, routes);
	res = circuit->calculate(routes);
	have_res = true;
	Invalidate(false);
}
