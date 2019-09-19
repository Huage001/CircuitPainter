#pragma once

#include<vector>
#include<set>
using namespace std;

#define TYPE_NULL 0
#define TYPE_SOURCE 1
#define TYPE_RES 2
#define TYPE_CUR 3
#define TYPE_VOL 4
#define TYPE_SLD 5
#define TYPE_WIRE 6

enum NodeType { NODE, SRC, ELEM, SLD };

enum ElemType { RES, VOL, CUR, LGT, KASE };

class Node
{
public:
	int m_id, num_of_neigh;
	NodeType m_type;
	vector<int> m_neighbors;
	Node();
	Node(int id);
};

class SrcNode :public Node
{
public:
	bool m_pos;
	double m_volt;
	SrcNode(bool isPos);
};

class ElemNode :public Node
{
public:
	ElemType elem_type;
	double m_volt, m_cur, m_res;
	int in, out;
	ElemNode();
};

class Edge
{
public:
	int m_start, m_end;
	double m_cur;
	Edge(int s, int t);
};

typedef pair<int, int> Dot;

class Stroke
{
public:
	vector<Dot> dots;
};

class Part
{
public:
	int type;
	vector<Stroke*> strokes;
	set<pair<Part*, int>> temp_pin;
	union
	{
		ElemNode* elem_node[2];
		int ports[2];
		SrcNode* src_node[2];
	};
	Part();
	Part* cut(Dot cut_point,int id);
};

class Graph
{
public:
	vector<Part*> all_parts;
	int max_id;
	Graph();
};
