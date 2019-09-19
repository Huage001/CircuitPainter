#include "stdafx.h"
#include "Struct.h"

Node::Node()
{
	m_type = NODE;
	num_of_neigh = 0;
}

Node::Node(int id)
{
	m_type = NODE;
	num_of_neigh = 0;
	m_id = id;
}

SrcNode::SrcNode(bool isPos)
{
	m_type = SRC;
	m_pos = isPos;
}

ElemNode::ElemNode()
{
	m_type = ELEM;
}

Edge::Edge(int s, int t)
{
	m_start = s;
	m_end = t;
}

Part::Part()
{
	type = TYPE_NULL;
	ports[0] = ports[1] = -1;
}

Graph::Graph()
{
	max_id = 0;
}

Part* Part::cut(Dot cut_point,int id)
{
	Part* new_part = NULL;
	for (int i = 0; i < strokes.size(); i++)
	{
		vector<Dot>::iterator pos = find(strokes[i]->dots.begin(), strokes[i]->dots.end(), cut_point);//查割点的位置
		if (pos != strokes[i]->dots.end())//在当前笔画找到割点
		{
			new_part = new Part();
			new_part->type = TYPE_WIRE;
			Stroke* new_stroke = new Stroke();
			for (vector<Dot>::iterator it = pos; it != strokes[i]->dots.end(); it++)//用当前笔画割点之后的点生成新的笔画
				new_stroke->dots.push_back(*it);
			new_part->strokes.push_back(new_stroke);//将新的笔画加入到新部件
			for (int j = i + 1; j < strokes.size(); j++)//将剩下的笔画加入到新部件
				new_part->strokes.push_back(strokes[j]);
			int times = strokes.size() - i - 1;//总共需要从以前的部件中删除这么多数量的笔画
			while (times--)//删除割点之后的笔画
				strokes.pop_back();
			strokes[i]->dots.erase(pos, strokes[i]->dots.end());//删除割点之后的点
			break;
		}
	}
	return new_part;
}
