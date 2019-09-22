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

SrcNode::SrcNode(Node* node,bool isPos)
{
	m_id = node->m_id;
	m_neighbors = node->m_neighbors;
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

