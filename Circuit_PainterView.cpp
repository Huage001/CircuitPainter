
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
#include <Python.h>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MAX_TIMEVAL 0.75
#define RADIUS 5
#define FONT_SIZE 20

CString name[] = { L"导线",L"电源",L"灯泡",L"电阻",L"电流表",L"电压表" };
COLORREF color[] = { RGB(0,0,0),RGB(0,0,255),RGB(255,255,0),RGB(255,100,100),RGB(255,0,255),RGB(0,255,255) };

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
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CCircuitPainterView 构造/析构

CCircuitPainterView::CCircuitPainterView() noexcept
{
	// TODO: 在此处添加构造代码
	Py_SetPythonHome(L"D:\\software\\tensorflow");
	Py_Initialize();
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('./')");
	PyRun_SimpleString("import numpy as np");
	PyRun_SimpleString("import matplotlib.pyplot as plt");
	PyRun_SimpleString("import tensorflow as tf");
	PyRun_SimpleString("from PIL import Image, ImageFilter");
	pModule = NULL;
	pFunc = NULL;
	pModule = PyImport_ImportModule("classification");
	pFunc = PyObject_GetAttrString(pModule, "classification");

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
	//CDC* pDC = GetDC();
	//HDC hdc = pDC->m_hDC;
	iScrWidth = GetSystemMetrics(SM_CXSCREEN);
	iScrHeight = GetSystemMetrics(SM_CYSCREEN);
	//iScrWidth = GetDeviceCaps(hdc, HORZRES);
	//iScrHeight = GetDeviceCaps(hdc, VERTRES);
	//iScrWidth = 1920;
	//iScrHeight = 1080;
	//key_point_table = new pair<Part*, int>* [iScrWidth];
	//all_point_table = new pair<Part*, Dot>* [iScrWidth];
	//for (int i = 0; i < iScrWidth; i++)
	//{
	//	key_point_table[i] = new pair<Part*, int>[iScrHeight]();
	//	all_point_table[i] = new pair<Part*, Dot>[iScrHeight]();
	//}

	//thread* classificator = new thread(&CCircuitPainterView::classification, NULL);
	//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)this->classification, NULL, 0, NULL);
}

CCircuitPainterView::~CCircuitPainterView()
{
	Py_Finalize();
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
	if (have_res)
	{
		CString temp_str;

		pDC->SelectObject(&font);
		pDC->SetBkColor(RGB(0, 0, 0));
		pDC->SetTextColor(RGB(255, 255, 255));

		for (int i = 0; i < res.size(); i++)
		{
			temp_str=res[i].c_str();
			pDC->TextOut(30, 325 + i * FONT_SIZE, temp_str);
		}

		int index = 0;
		for (vector<Part*>::iterator it = pDoc->graph->all_parts.begin(); it != pDoc->graph->all_parts.end(); it++)
			if ((*it)->type == TYPE_RES && (*it)->pin_point.size() > 2)
			{
				Node* nodes[3], *target;
				for (int i = 0; i < 3; i++)
				{
					nodes[i] = key_points[(*it)->pin_point[i].second];
					if (nodes[i]->num_of_neigh == 2)
					{
						if (key_points[nodes[i]->m_neighbors[0]]->m_type == ELEM)
							target = key_points[nodes[i]->m_neighbors[0]];
						else if (key_points[nodes[i]->m_neighbors[1]]->m_type == ELEM)
							target = key_points[nodes[i]->m_neighbors[1]];
						else
							assert(0);
						temp_str.Format(L"%s电流：%.2lfA", name[(*it)->type], ((ElemNode*)target)->m_cur);
						pDC->TextOut((*it)->pin_point[i].first.first, (*it)->pin_point[i].first.second - 50, temp_str);
						temp_str.Format(L"%s电压：%.2lfV", name[(*it)->type], ((ElemNode*)target)->m_volt);
						pDC->TextOut((*it)->pin_point[i].first.first, (*it)->pin_point[i].first.second - 30, temp_str);
					}
				}
			}
			else if ((*it)->type == TYPE_LGT || (*it)->type == TYPE_RES)
			{
				temp_str.Format(L"%s电流：%.2lfA", name[(*it)->type], (*it)->elem_node[0]->m_cur);
				pDC->TextOut((*it)->pin_point[0].first.first, (*it)->pin_point[0].first.second - 50, temp_str);
				temp_str.Format(L"%s电压：%.2lfV", name[(*it)->type], (*it)->elem_node[0]->m_volt);
				pDC->TextOut((*it)->pin_point[0].first.first, (*it)->pin_point[0].first.second - 30, temp_str);
			}
			else if ((*it)->type == TYPE_CUR)
			{
				temp_str.Format(L"%s电流：%.2lfA", name[(*it)->type], (*it)->elem_node[0]->m_cur);
				pDC->TextOut((*it)->pin_point[0].first.first, (*it)->pin_point[0].first.second - 50, temp_str);
			}
			else if ((*it)->type == TYPE_VOL)
			{
				temp_str.Format(L"%s电压：%.2lfV", name[(*it)->type], (*it)->elem_node[0]->m_volt);
				pDC->TextOut((*it)->pin_point[0].first.first, (*it)->pin_point[0].first.second - 30, temp_str);
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

int CCircuitPainterView::tensorflow(Part* part)
{
	PyObject* pArgs = PyTuple_New(2);
	PyObject *pListx = PyList_New(part->strokes.size()), *pListy = PyList_New(part->strokes.size());
	for (int i = 0; i < part->strokes.size(); i++)
	{
		PyObject* pListxi = PyList_New(part->strokes[i]->dots.size()), * pListyi = PyList_New(part->strokes[i]->dots.size());
		for (int j = 0; j < part->strokes[i]->dots.size(); j++)
		{
			PyList_SetItem(pListxi, j, Py_BuildValue("i", part->strokes[i]->dots[j].first));
			PyList_SetItem(pListyi, j, Py_BuildValue("i", part->strokes[i]->dots[j].second));
		}
		PyList_SetItem(pListx, i, pListxi);
		PyList_SetItem(pListy, i, pListyi);
	}
	PyTuple_SetItem(pArgs, 0, pListx);
	PyTuple_SetItem(pArgs, 1, pListy);
	PyObject* pReturn = PyEval_CallObject(pFunc, pArgs);
	int  nResult;
	PyArg_Parse(pReturn, "i", &nResult);
	return nResult;
}

bool CCircuitPainterView::check_gradient_line(Dot p1, Dot p2, Dot p3)
{
	double p1_p2_x = (double)p2.first - (double)p1.first;
	double p1_p2_y = (double)p2.second - (double)p2.second;
	double p2_p3_x = (double)p3.first - (double)p2.first;
	double p2_p3_y = (double)p3.second - (double)p2.second;
	double p1_p2dianchengp2_p3 = p1_p2_x * p2_p3_x + p1_p2_y * p2_p3_y;
	double p1_p2 = sqrt(p1_p2_x * p1_p2_x + p1_p2_y * p1_p2_y);
	double p2_p3 = sqrt(p2_p3_x * p2_p3_x + p2_p3_y * p2_p3_y);
	double res = p1_p2dianchengp2_p3 / (p1_p2 * p2_p3);
	if (res < 0.7)
		return true;
	else
		return false;
}

void CCircuitPainterView::regulation(Part* part)
{
	CDC* pDC = GetDC();
	CPen pen_clear(PS_SOLID, 3, RGB(255, 255, 255));
	pDC->SelectObject(&pen_clear);
	for (int i = 0; i < part->strokes.size(); i++)
		for (int j = 1; j < part->strokes[i]->dots.size(); j++)
		{
			pDC->MoveTo(part->strokes[i]->dots[j - 1].first, part->strokes[i]->dots[j - 1].second);
			pDC->LineTo(part->strokes[i]->dots[j].first, part->strokes[i]->dots[j].second);
		}
	CPen pen_repaint(PS_SOLID, 3, RGB(0, 0, 255));
	pDC->SelectObject(&pen_repaint);
	if (part->type == TYPE_WIRE||part->type==TYPE_SOURCE)
	{
		for (int i = 0; i < part->strokes.size(); i++)
		{
			Dot previous_dot;
			Shape* shape;
			previous_dot = part->strokes[i]->dots[0];
			pDC->MoveTo(part->strokes[i]->dots[0].first, part->strokes[i]->dots[0].second);
			for (int j = 1; j < part->strokes[i]->dots.size() - 1; j++)
				if (check_gradient_line(part->strokes[i]->dots[j-1], part->strokes[i]->dots[j], part->strokes[i]->dots[j+1]))
				{
					shape = new Line(previous_dot, part->strokes[i]->dots[j]);
					shape_table.push_back(make_pair(shape,part));
					previous_dot = part->strokes[i]->dots[j];
					pDC->LineTo(part->strokes[i]->dots[j].first, part->strokes[i]->dots[j].second);
					pDC->MoveTo(part->strokes[i]->dots[j].first, part->strokes[i]->dots[j].second);
				}
			pDC->LineTo(part->strokes[i]->dots.back().first, part->strokes[i]->dots.back().second);
			shape = new Line(previous_dot, part->strokes[i]->dots.back());
			shape_table.push_back(make_pair(shape, part));
		}
	}
	else if(part->type==TYPE_RES)
	{
		int cur_min = INF, cur_max = 0;
		Dot a, b, c, d;
		for (int i = 0; i < part->strokes.size(); i++)
		{
			for (int j = 0; j < part->strokes[i]->dots.size(); j++)
			{
				int temp = part->strokes[i]->dots[j].first + part->strokes[i]->dots[j].second;
				if (temp < cur_min)
				{
					cur_min = temp;
					a = part->strokes[i]->dots[j];
				}
				if (temp > cur_max)
				{
					cur_max = temp;
					b = part->strokes[i]->dots[j];
				}
			}
		}
		pDC->Rectangle(a.first, a.second, b.first, b.second);
		c.first = a.first;
		c.second = b.second;
		d.first = b.first;
		d.second = a.second;
		Shape* shape;
		shape = new Line(a, c);
		shape_table.push_back(make_pair(shape, part));
		shape = new Line(c, b);
		shape_table.push_back(make_pair(shape, part));
		shape = new Line(b, d);
		shape_table.push_back(make_pair(shape, part));
		shape = new Line(d, a);
		shape_table.push_back(make_pair(shape, part));
	}
	else
	{
		int xmax = 0, ymax = 0, xmin = INF, ymin = INF;
		for (int i = 0; i < part->strokes.size(); i++)
		{
			for (int j = 0; j < part->strokes[i]->dots.size(); j++)
			{
				if (part->strokes[i]->dots[j].first > xmax)
					xmax = part->strokes[i]->dots[j].first;
				if (part->strokes[i]->dots[j].first < xmin)
					xmin = part->strokes[i]->dots[j].first;
				if (part->strokes[i]->dots[j].second > ymax)
					ymax = part->strokes[i]->dots[j].second;
				if (part->strokes[i]->dots[j].second < ymin)
					ymin = part->strokes[i]->dots[j].second;
			}
		}
		int cx = (xmax + xmin) / 2, cy = (ymax + ymin) / 2, r = (xmax + ymax - xmin - ymin) / 4;
		pDC->Ellipse(cx - r, cy - r, cx + r, cy + r);
		Shape* shape = new Circle(make_pair(cx, cy), r);
	}
}

Part* CCircuitPainterView::search_shape_table(Dot p)
{
	for (int i = 0; i < shape_table.size(); i++)
		if (shape_table[i].first->collision_detection(p))
			return shape_table[i].second;
	return NULL;
}

//TODO：分类线程
void CCircuitPainterView::classification()
{
	Part* target = part;	//取队首part
	target->type=tensorflow(part);
	if (target->type == TYPE_SOURCE && target->strokes.size() < 2)
		target->type = TYPE_WIRE;

	CDC* pDC = GetDC();
	CCircuitPainterDoc* pDoc = GetDocument();
	CString temp_str;
	temp_str.Format(name[target->type]);
	pDC->SelectObject(&font);
	pDC->SetBkColor(RGB(200, 200, 200));
	pDC->SetTextColor(RGB(r, g, b));
	pDC->TextOut(30, pDoc->graph->all_parts.size() * FONT_SIZE, temp_str);

	if (target->type == TYPE_WIRE)//如果返回的是导线，
	{
		//其实都不用这么麻烦，下一步改进时可以考虑多种等价情况，把导线中间点也结合到端点上
		Dot start_end[2];
		pair<int, int> unknown[2];
		unknown[0].first = unknown[1].first = unknown[0].second = unknown[1].second = -1;//待定的关键节点，初始为未确定状态
		start_end[0] = target->strokes[0]->dots[0];
		start_end[1] = target->strokes.back()->dots.back();
		for (int index = 0; index < 2; index++)
		{
			Part* connect = search_shape_table(make_pair(start_end[index].first, start_end[index].second));
			if (connect != NULL && connect != target)//在所有点表中查找该笔画起点与终点对应的part，如果找到，
			{
				if (connect->type == TYPE_WIRE)//如果对方也是导线，
				{
					map<Dot, pair<Part*, int>>::iterator key_search = key_point_table.find(make_pair(start_end[index].first, start_end[index].second));
					//if (key_point_table[start_end[index].first][start_end[index].second].first == NULL)//说明在关键点表中没有匹配到，需要新增关键点
					if(key_search==key_point_table.end())
					{

						//新增关键点将它的相邻区域点加入到关键点表
						for (int j = start_end[index].first - 2*RADIUS; j <= start_end[index].first + 2*RADIUS; j++)
							for (int k = start_end[index].second - 2*RADIUS; k <= start_end[index].second + 2*RADIUS; k++)
								if (j >= 0 && j < iScrWidth && k >= 0 && k < iScrHeight)
								{
									//key_point_table[j][k] = make_pair(target, cur_code);
									key_point_table.insert(make_pair(make_pair(j, k), make_pair(target, cur_code)));
									pDC->SetPixel(j, k, RGB(255, 0, 0));
								}

						//处理新关键点
						Node* new_node = new Node(cur_code);
						new_node->m_type = NODE;
						new_node->num_of_neigh = 3;
						new_node->m_neighbors.push_back(connect->ports[0]);
						new_node->m_neighbors.push_back(connect->ports[1]);
						new_node->m_neighbors.push_back(-1);//在target上的节点处于待定状态
						//原导线的连接关系要变
						for (int j = 0; j < 2; j++)
						{
							Node* port_node = key_points[connect->ports[j]];
							for (int k = 0; k < port_node->m_neighbors.size(); k++)
								if (port_node->m_neighbors[k] == connect->ports[!j])
								{
									port_node->m_neighbors[k] = cur_code;
									break;
								}
						}
						unknown[index].first = cur_code;
						unknown[!index].second = cur_code;//此时这个点可能成为另一个端点的邻居，因此将它送入另一个端点的待定区
						cur_code++;
						key_points.push_back(new_node);
					}
					else//说明和这条线的某个关键节点对上了
					{
						//int interface_id = key_point_table[start_end[index].first][start_end[index].second].second;//和哪个关键节点对上了
						int interface_id = key_search->second.second;
						key_points[interface_id]->num_of_neigh++;
						key_points[interface_id]->m_neighbors.push_back(-1);//同理具体和哪个点连着待定
						unknown[index].first = interface_id;
						unknown[!index].second = interface_id;//同理，送入另一个端点的待定区

						if (target->ports[0] == -1)
							target->ports[0] = interface_id;
						else if (target->ports[1] == -1)
							target->ports[1] = interface_id;
						else
							assert(0);

					}
				}
				else if (connect->type == TYPE_SOURCE)
				{
					//新增关键点将它的相邻区域点加入到关键点表
					for (int j = start_end[index].first - 2 * RADIUS; j <= start_end[index].first + 2 * RADIUS; j++)
						for (int k = start_end[index].second - 2 * RADIUS; k <= start_end[index].second + 2 * RADIUS; k++)
							if (j >= 0 && j < iScrWidth && k >= 0 && k < iScrHeight)
							{
								//key_point_table[j][k] = make_pair(target, cur_code);
								key_point_table.insert(make_pair(make_pair(j, k), make_pair(target, cur_code)));
								pDC->SetPixel(j, k, RGB(255, 0, 0));
							}

					//处理新关键点
					Node* new_node;
					if (connect->ports[0] == -1)
					{
						new_node = new SrcNode(true);
						connect->src_node[0] = (SrcNode*)new_node;
						((SrcNode*)new_node)->m_volt = 6;//水
					}
					else if (connect->ports[1] == -1)
					{
						new_node = new SrcNode(false);
						connect->src_node[1] = (SrcNode*)new_node;
						((SrcNode*)new_node)->m_volt = 6;//水
					}
					else
						assert(0);
					new_node->m_id = cur_code;
					new_node->m_type = SRC;
					new_node->num_of_neigh = 1;
					new_node->m_neighbors.push_back(-1);
					
					unknown[index].first = cur_code;
					unknown[!index].second = cur_code;

					if (target->ports[0] == -1)
						target->ports[0] = cur_code;
					else if (target->ports[1] == -1)
						target->ports[1] = cur_code;
					else
						assert(0);

					cur_code++;
					key_points.push_back(new_node);
				}
				else//否则（对方不是导线也不是电源）
				{
					if ((connect->type == TYPE_RES && connect->pin_point.size() >= 3) || (connect->type != TYPE_RES && connect->pin_point.size() >= 2))//如果对方的引脚满了，
						assert(0);
					else//否则，加入该引脚，更新对方的引脚信息和这条导线的信息
					{

						//新增关键点将它的相邻区域点加入到关键点表
						for (int j = start_end[index].first - 2*RADIUS; j <= start_end[index].first + 2*RADIUS; j++)
							for (int k = start_end[index].second - 2*RADIUS; k <= start_end[index].second + 2*RADIUS; k++)
								if (j >= 0 && j < iScrWidth && k >= 0 && k < iScrHeight)
								{
									//key_point_table[j][k] = make_pair(connect, cur_code);
									key_point_table.insert(make_pair(make_pair(j, k), make_pair(connect, cur_code)));
									pDC->SetPixel(j, k, RGB(255, 0, 0));
								}

						//处理新关键点
						Node* new_node = new Node(cur_code);
						new_node->m_type = NODE;
						int code_offset = 1;

						if (connect->type == TYPE_RES && connect->pin_point.size() == 2)//变阻器
						{
							connect->pin_point.push_back(make_pair(start_end[index], cur_code));
							
							int dis1 = pow((connect->pin_point[0].first.first - connect->pin_point[1].first.first), 2) + pow((connect->pin_point[0].first.second - connect->pin_point[1].first.second), 2);
							int dis2 = pow((connect->pin_point[0].first.first - connect->pin_point[2].first.first), 2) + pow((connect->pin_point[0].first.second - connect->pin_point[2].first.second), 2);
							int dis3 = pow((connect->pin_point[1].first.first - connect->pin_point[2].first.first), 2) + pow((connect->pin_point[1].first.second - connect->pin_point[2].first.second), 2);
							
							Node* old_elem = connect->elem_node[0], * new_elem = new ElemNode(), * middle_node, * left_node, * right_node;
							key_points.push_back(new_node);
							key_points.push_back(new_elem);
							new_elem->m_id = cur_code + code_offset;
							code_offset++;
							new_elem->m_type = ELEM;
							new_elem->num_of_neigh = 2;
							((ElemNode*)new_elem)->m_res = 1;//水
							((ElemNode*)new_elem)->elem_type = RES;
							((ElemNode*)old_elem)->m_res = 1;//水

							if (dis1 >= dis2 && dis1 >= dis3)
							{
								left_node = key_points[old_elem->m_neighbors[0]];
								right_node = key_points[old_elem->m_neighbors[1]];
								middle_node = new_node;

								if (right_node->m_neighbors[0] == old_elem->m_id)
									right_node->m_neighbors[0] = new_elem->m_id;
								else if (right_node->m_neighbors[1] == old_elem->m_id)
									right_node->m_neighbors[1] = new_elem->m_id;
								else
									assert(0);

								new_node->num_of_neigh = 3;
								new_node->m_neighbors.push_back(old_elem->m_id);
								new_node->m_neighbors.push_back(new_elem->m_id);
								new_node->m_neighbors.push_back(-1);
							}
							else
							{
								if (dis2 > dis1 && dis2 > dis3)
								{
									left_node = key_points[connect->pin_point[0].second];
									middle_node = key_points[connect->pin_point[1].second];
								}
								else
								{
									left_node = key_points[connect->pin_point[1].second];
									middle_node = key_points[connect->pin_point[0].second];
								}
								right_node = new_node;
								middle_node->num_of_neigh++;
								middle_node->m_neighbors.push_back(new_elem->m_id);

								new_node->num_of_neigh = 2;
								new_node->m_neighbors.push_back(new_elem->m_id);
								new_node->m_neighbors.push_back(-1);
							}

							if (old_elem->m_neighbors[0] == left_node->m_id)
								old_elem->m_neighbors[1] = middle_node->m_id;
							else if (old_elem->m_neighbors[1] == left_node->m_id)
								old_elem->m_neighbors[0] = middle_node->m_id;
							else
								assert(0);

							new_elem->m_neighbors.push_back(middle_node->m_id);
							new_elem->m_neighbors.push_back(right_node->m_id);

							unknown[index].first = cur_code;
							unknown[!index].second = cur_code;
						}
						else
						{
							new_node->num_of_neigh = 2;
							new_node->m_neighbors.push_back(connect->elem_node[0]->m_id);
							new_node->m_neighbors.push_back(-1);
							unknown[index].first = cur_code;
							unknown[!index].second = cur_code;
							connect->elem_node[0]->m_neighbors.push_back(cur_code);
							connect->elem_node[0]->num_of_neigh++;
							connect->pin_point.push_back(make_pair(start_end[index], cur_code));
							key_points.push_back(new_node);
						}

						if (target->ports[0] == -1)
							target->ports[0] = cur_code;
						else if (target->ports[1] == -1)
							target->ports[1] = cur_code;
						else
							assert(0);

						cur_code += code_offset;
					}
				}
			}
			else//否则（没找到对应Part）
			{

				if (target->ports[0] == -1)
					target->ports[0] = cur_code;
				else if (target->ports[1] == -1)
					target->ports[1] = cur_code;
				else
					assert(0);

				//新增关键点将它的相邻区域点加入到关键点表
				for (int j = start_end[index].first - 2*RADIUS; j <= start_end[index].first + 2*RADIUS; j++)
					for (int k = start_end[index].second - 2*RADIUS; k <= start_end[index].second + 2*RADIUS; k++)
						if (j >= 0 && j < iScrWidth && k >= 0 && k < iScrHeight)
						{
							//key_point_table[j][k] = make_pair(part, cur_code);
							key_point_table.insert(make_pair(make_pair(j, k), make_pair(target, cur_code)));
							pDC->SetPixel(j, k, RGB(255, 0, 0));
						}

				Node* new_node = new Node(cur_code);
				new_node->m_type = NODE;
				new_node->num_of_neigh = 1;
				new_node->m_neighbors.push_back(-1);
				unknown[index].first = cur_code;
				unknown[!index].second = cur_code;
				key_points.push_back(new_node);
				cur_code++;
			}
		}
		//处理临时节点空间
		int middle_pin = -1;
		for (map<pair<Part*, int>,Dot>::iterator it = target->temp_pin.begin(); it != target->temp_pin.end(); it++)
		{
			map<Dot, pair<Part*, int>>::iterator search1 = key_point_table.find(make_pair(start_end[0].first, start_end[0].second)), search2 = key_point_table.find(make_pair(start_end[1].first, start_end[1].second));
			if ((search1!=key_point_table.end()&&it->first.second == search1->second.second) || (search2 != key_point_table.end() && it->first.second == search2->second.second))
			//if (it->first.second == key_point_table[start_end[0].first][start_end[0].second].second || it->first.second == key_point_table[start_end[1].first][start_end[1].second].second)
				continue;//说明关键点就是起点和终点，已经在上面处理过这种情况了，之后的情况都需要拆导线
			if (middle_pin == -1)
			{
				middle_pin = it->first.second;//这个节点抢占了导线中间节点的山头
				unknown[0].second = unknown[1].second = middle_pin;//这个时候都已经确定有中间节点了
			}
			key_points[it->first.second]->num_of_neigh += 2;//该关键点的邻接节点要加入这条导线上的两个端点
			key_points[it->first.second]->m_neighbors.push_back(unknown[0].first);
			key_points[it->first.second]->m_neighbors.push_back(unknown[1].first);
		}
		target->temp_pin.clear();
		if (target->ports[0] == -1)
			if (target->ports[1] == unknown[0].second)
				target->ports[0] = unknown[1].second;
			else if (target->ports[1] == unknown[1].second)
				target->ports[0] = unknown[0].second;
			else
				assert(0);
		else if (target->ports[1] == -1)
			if (target->ports[0] == unknown[0].second)
				target->ports[1] = unknown[1].second;
			else if (target->ports[0] == unknown[1].second)
				target->ports[1] = unknown[0].second;
		key_points[unknown[0].first]->m_neighbors.back() = unknown[0].second;
		key_points[unknown[1].first]->m_neighbors.back() = unknown[1].second;
	}
	else if (target->type == TYPE_SOURCE)
	{
		for (map<pair<Part*, int>, Dot>::iterator it = target->temp_pin.begin(); it != target->temp_pin.end(); it++)
		{
			Node* src_node;
			if (key_points[it->first.first->ports[0]]->num_of_neigh > 1)
			{
				src_node = new SrcNode(key_points[it->first.second], true);
				((SrcNode*)src_node)->m_volt = 6;//水
				target->src_node[0] = (SrcNode*)src_node;
			}
			else if (key_points[it->first.first->ports[1]]->num_of_neigh > 1)
			{
				src_node = new SrcNode(key_points[it->first.second], false);
				((SrcNode*)src_node)->m_volt = 6;//水
				target->src_node[1] = (SrcNode*)src_node;
			}
			else
				assert(0);
			
			delete key_points[it->first.second];
			key_points[it->first.second] = src_node;
		}
	}
	else//其他元件
	{
		Node* new_elem = new ElemNode();
		key_points.push_back(new_elem);
		new_elem->m_id = cur_code;
		int code_offset = 1;
		new_elem->m_type = ELEM;
		switch (target->type)
		{
		case TYPE_RES:
			((ElemNode*)new_elem)->elem_type = RES;
			((ElemNode*)new_elem)->m_res = 2;//水
			break;
		case TYPE_VOL:
			((ElemNode*)new_elem)->elem_type = VOL;
			((ElemNode*)new_elem)->m_res = INF;
			break;
		case TYPE_CUR:
			((ElemNode*)new_elem)->elem_type = CUR;
			((ElemNode*)new_elem)->m_res = 0;
			break;
		case TYPE_LGT:
			((ElemNode*)new_elem)->elem_type = LGT;
			((ElemNode*)new_elem)->m_res = 2;//水
			break;
		default:
			assert(0);
		}
		if(target->temp_pin.size()<3)
			for (map<pair<Part*, int>, Dot>::iterator it = target->temp_pin.begin(); it != target->temp_pin.end(); it++)
			{
				new_elem->m_neighbors.push_back(it->first.second);
				new_elem->num_of_neigh++;
				if ((target->type == TYPE_RES && target->pin_point.size() >= 3) || (target->type != TYPE_RES && target->pin_point.size() >= 2))//如果引脚满了，
					assert(0);
				target->pin_point.push_back(make_pair(it->second, it->first.second));
				key_points[it->first.second]->m_neighbors.push_back(cur_code);
				key_points[it->first.second]->num_of_neigh++;
			}
		else
		{
			if (target->type != TYPE_RES || target->temp_pin.size() > 3)
				assert(0);
			for (map<pair<Part*, int>, Dot>::iterator it = target->temp_pin.begin(); it != target->temp_pin.end(); it++)
				target->pin_point.push_back(make_pair(it->second, it->first.second));
			int dis1 = pow((target->pin_point[0].first.first - target->pin_point[1].first.first), 2) + pow((target->pin_point[0].first.second - target->pin_point[1].first.second), 2);
			int dis2 = pow((target->pin_point[0].first.first - target->pin_point[2].first.first), 2) + pow((target->pin_point[0].first.second - target->pin_point[2].first.second), 2);
			int dis3 = pow((target->pin_point[1].first.first - target->pin_point[2].first.first), 2) + pow((target->pin_point[1].first.second - target->pin_point[2].first.second), 2);
			Node* new_new_elem = new ElemNode(), * left_node, * right_node, * middle_node;
			if (dis1 > dis2 && dis1 > dis3)
			{
				left_node = key_points[0];
				right_node = key_points[1];
				middle_node = key_points[2];
			}
			else if (dis2 >= dis1 && dis2 >= dis3)
			{
				left_node = key_points[0];
				right_node = key_points[2];
				middle_node = key_points[1];
			}
			else
			{
				left_node = key_points[2];
				right_node = key_points[1];
				middle_node = key_points[0];
			}
			key_points.push_back(new_new_elem);
			new_new_elem->m_id = cur_code + code_offset;
			code_offset++;
			new_new_elem->m_type = ELEM;
			((ElemNode*)new_new_elem)->elem_type = RES;
			((ElemNode*)new_new_elem)->m_res = 1;//水
			new_new_elem->num_of_neigh = 2;
			new_new_elem->m_neighbors.push_back(middle_node->m_id);
			new_new_elem->m_neighbors.push_back(right_node->m_id);
			((ElemNode*)new_elem)->m_res = 1;//水
			new_elem->num_of_neigh = 2;
			new_elem->m_neighbors.push_back(middle_node->m_id);
			new_elem->m_neighbors.push_back(left_node->m_id);
			left_node->num_of_neigh = 2;
			left_node->m_neighbors.push_back(new_elem->m_id);
			middle_node->num_of_neigh = 3;
			middle_node->m_neighbors.push_back(new_elem->m_id);
			middle_node->m_neighbors.push_back(new_new_elem->m_id);
			right_node->num_of_neigh = 2;
			right_node->m_neighbors.push_back(new_new_elem->m_id);
			target->elem_node[1] = (ElemNode*)new_new_elem;
		}

		target->temp_pin.clear();
		target->elem_node[0] = (ElemNode*)new_elem;
		cur_code+=code_offset;
	}
	//把该笔画所有的点相邻区域的点加入到所有点阵哈希表并映射到相应part
	regulation(target);
	/*
	for (int s = 0; s < target->strokes.size(); s++)
		for (int i = 0; i < target->strokes[s]->dots.size(); i++)
			for (int j = target->strokes[s]->dots[i].first - RADIUS; j <= target->strokes[s]->dots[i].first + RADIUS; j++)
				for (int k = target->strokes[s]->dots[i].second - RADIUS; k <= target->strokes[s]->dots[i].second + RADIUS; k++)
					if (j >= 0 && j < iScrWidth && k >= 0 && k < iScrHeight)
					{
						//all_point_table[j][k] = make_pair(part, target->strokes[s]->dots[i]);
						all_point_table.insert(make_pair(make_pair(j, k), make_pair(target, target->strokes[s]->dots[i])));
						pDC->SetPixel(j, k, color[target->type]);
					}
					*/
	KillTimer(timer_id);
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
	}
	else
		KillTimer(timer_id);
	part->strokes.push_back(stroke);

	//在关键点表中查找该点，如果查到，则加入到该元件（此时该元件还未分类）临时节点空间中，等待识别之后处理

	map<Dot, pair<Part*, int>>::iterator key_search = key_point_table.find(make_pair(point.x, point.y));
	//if (key_point_table[point.x][point.y].first != NULL)
	if(key_search!=key_point_table.end())
		//part->temp_pin.insert(make_pair(key_point_table[point.x][point.y], make_pair(point.x, point.y)));
		part->temp_pin.insert(make_pair(key_search->second, make_pair(point.x, point.y)));
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
		//if (key_point_table[point.x][point.y].first != NULL)
		map<Dot, pair<Part*, int>>::iterator key_search = key_point_table.find(make_pair(point.x, point.y));
		if (key_search != key_point_table.end())
			part->temp_pin.insert(make_pair(key_search->second, make_pair(point.x, point.y)));
			//part->temp_pin.insert(make_pair(key_point_table[point.x][point.y], make_pair(point.x, point.y)));
		last_point = point;
	}
	CView::OnMouseMove(nFlags, point);
}


void CCircuitPainterView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	mouseDown = false;
	//在关键点表中查找该点，如果查到，则加入到该元件（此时该元件还未分类）临时节点空间中，等待识别之后处理
	//if (key_point_table[point.x][point.y].first != NULL)
	//	part->temp_pin.insert(make_pair(key_point_table[point.x][point.y], make_pair(point.x, point.y)));
	map<Dot, pair<Part*, int>>::iterator key_search = key_point_table.find(make_pair(point.x, point.y));
	if (key_search != key_point_table.end())
		part->temp_pin.insert(make_pair(key_search->second, make_pair(point.x, point.y)));
	endTime = clock();
	timer_id = SetTimer(1, MAX_TIMEVAL * CLOCKS_PER_SEC, NULL);
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
	//识别之前
	//CCircuitPainterDoc* pDoc = GetDocument();
	//circuit = new Circuit(pDoc->graph);

	//识别之后
	circuit = new Circuit(key_points);
	vector<vector<int>>routes;
	circuit->dfs(circuit->m_src_p->m_id, circuit->m_src_n->m_id, routes);
	res = circuit->calculate(routes);
	have_res = true;
	Invalidate(false);
}


void CCircuitPainterView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == timer_id)
		classification();
	CView::OnTimer(nIDEvent);
}
