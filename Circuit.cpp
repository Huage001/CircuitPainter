#include "stdafx.h"
#include "equation.h"
#include "Struct.h"
#include "Circuit.h"
#include<stack>
#include<string>
#include<vector>
#include<cmath>
using namespace std;

const double INF = 99999999, MIN = 1e-6;

Circuit::Circuit(Graph* gph)
{
	num_of_node = gph->max_id + 1;
	m_nodes.resize(num_of_node, NULL);
	for (vector<Part*>::iterator it = gph->all_parts.begin(); it != gph->all_parts.end(); ++it)
	{
		switch ((*it)->type)
		{
		case TYPE_NULL:
			break;
		case TYPE_WIRE:
			for (int i = 0; i < 2; i++)
			{
				if (m_nodes[(*it)->ports[i]] == NULL)
					m_nodes[(*it)->ports[i]] = new Node((*it)->ports[i]);
				if (m_nodes[(*it)->ports[i]]->m_type != SRC)
				{
					m_nodes[(*it)->ports[i]]->num_of_neigh++;
					m_nodes[(*it)->ports[i]]->m_neighbors.push_back((*it)->ports[!i]);
				}
			}
			break;
		case TYPE_SOURCE:
			for (int i = 0; i < 2; i++)
			{
				m_nodes[(*it)->src_node[i]->m_id] = (*it)->src_node[i];
				/*
				for (int j = 0; j < (*it)->src_node[i]->num_of_neigh; j++)
				{
					if (m_nodes[(*it)->src_node[i]->m_neighbors[j]] == NULL)
						m_nodes[(*it)->src_node[i]->m_neighbors[j]] = new Node((*it)->src_node[i]->m_neighbors[j]);
					m_nodes[(*it)->src_node[i]->m_neighbors[j]]->num_of_neigh++;
					m_nodes[(*it)->src_node[i]->m_neighbors[j]]->m_neighbors.push_back((*it)->src_node[i]->m_id);
				}
				*/
			}
			m_src_p = (*it)->src_node[0];
			m_src_n = (*it)->src_node[1];
			break;
		case TYPE_SLD:
			for (int i = 0; i < 2; i++)
			{
				m_nodes[(*it)->elem_node[i]->m_id] = (*it)->elem_node[i];
				if (m_nodes[(*it)->elem_node[i]->in] == NULL)
					m_nodes[(*it)->elem_node[i]->in] = new Node((*it)->elem_node[i]->in);
				if(m_nodes[(*it)->elem_node[i]->out]==NULL)
					m_nodes[(*it)->elem_node[i]->out] = new Node((*it)->elem_node[i]->out);
				m_nodes[(*it)->elem_node[i]->in]->num_of_neigh++;
				m_nodes[(*it)->elem_node[i]->in]->m_neighbors.push_back((*it)->elem_node[i]->m_id);
				m_nodes[(*it)->elem_node[i]->out]->num_of_neigh++;
				m_nodes[(*it)->elem_node[i]->out]->m_neighbors.push_back((*it)->elem_node[i]->m_id);
			}
			break;
		default:
			m_nodes[(*it)->elem_node[0]->m_id] = (*it)->elem_node[0];
			if (m_nodes[(*it)->elem_node[0]->in] == NULL)
				m_nodes[(*it)->elem_node[0]->in] = new Node((*it)->elem_node[0]->in);
			if (m_nodes[(*it)->elem_node[0]->out] == NULL)
				m_nodes[(*it)->elem_node[0]->out] = new Node((*it)->elem_node[0]->out);
			m_nodes[(*it)->elem_node[0]->in]->num_of_neigh++;
			m_nodes[(*it)->elem_node[0]->in]->m_neighbors.push_back((*it)->elem_node[0]->m_id);
			m_nodes[(*it)->elem_node[0]->out]->num_of_neigh++;
			m_nodes[(*it)->elem_node[0]->out]->m_neighbors.push_back((*it)->elem_node[0]->m_id);
		}
	}
	for (int i = 0; i < num_of_node; i++)
		for (int j = 0; j < m_nodes[i]->num_of_neigh; j++)
		{
			Edge* edge = new Edge(i, m_nodes[i]->m_neighbors[j]);
			m_edges[make_pair(i, m_nodes[i]->m_neighbors[j])] = edge;
			m_edges[make_pair(m_nodes[i]->m_neighbors[j], i)] = edge;
		}
}

void Circuit::dfs(int s, int t, vector<vector<int>>& routes)
{
	stack<int>stk;
	bool *inStk = new bool[num_of_node]();
	bool **visit = new bool*[num_of_node];
	for (int i = 0; i < num_of_node; i++)
		visit[i] = new bool[m_nodes[i]->num_of_neigh]();
	stk.push(t);
	inStk[t] = true;
	while (!stk.empty())
	{
		int v = stk.top();
		if (v == s)
		{
			vector<int> route;
			while (!stk.empty())
			{
				int tmp = stk.top();
				route.push_back(tmp);
				stk.pop();
			}
			for (int i = route.size() - 1; i >= 0; i--)
				stk.push(route[i]);
			routes.push_back(route);
			stk.pop();
			inStk[v] = false;
			continue;
		}
		bool find = false;
		for (int i = 0; i < m_nodes[v]->num_of_neigh; i++)
			if (!inStk[m_nodes[v]->m_neighbors[i]] && !visit[v][i])
			{
				find = true;
				stk.push(m_nodes[v]->m_neighbors[i]);
				inStk[m_nodes[v]->m_neighbors[i]] = true;
				visit[v][i] = true;
				break;
			}
		if (!find)
		{
			stk.pop();
			inStk[v] = false;
			for (int i = 0; i < m_nodes[v]->num_of_neigh; i++)
				visit[v][i] = false;
		}
	}
	delete[]inStk;
	for (int i = 0; i < num_of_node; i++)
		delete[]visit[i];
	delete[]visit;
} 

vector<string> Circuit::calculate(vector<vector<int>>& routes)
{
	int num_of_eq = routes.size() + num_of_node - 2, num_of_res = 0;
	map<int, int>elem_volt_to_idx;
	map<int, int>elem_cur_to_idx;
	map<Edge*, int>node_cur_to_idx;
	vector<string>sign;
	int cnt = 0;
	for (int i = 0; i < num_of_node; i++)
		if (m_nodes[i]->m_type == ELEM)
		{
			sign.push_back(to_string(i) + "号节点的电压");
			elem_volt_to_idx[i] = cnt++;
			num_of_res++;
			sign.push_back(to_string(i) + "号节点的电流");
			elem_cur_to_idx[i] = cnt++;
			num_of_res++;
		}
	for (map<pair<int, int>, Edge*>::iterator it = m_edges.begin(); it != m_edges.end(); it++)
		if (it->first.first < it->first.second)
			if (m_nodes[it->first.first]->m_type != ELEM && m_nodes[it->first.second]->m_type != ELEM)
			{
				sign.push_back(to_string(it->first.first) + "号节点到" + to_string(it->first.second) + "号节点的电流");
				node_cur_to_idx[it->second] = cnt++;
			}
	int num_of_x = sign.size();
	double *matrix = new double[num_of_eq*num_of_x]();
	double *result = new double[num_of_eq]();
	size_t i = 0;
	for (; i < routes.size(); i++)
		for (size_t j = 0; j < routes[i].size(); j++)
			if (m_nodes[routes[i][j]]->m_type == ELEM)
			{
				result[i] = m_src_p->m_volt;
				get_i_j(matrix, i, elem_volt_to_idx[routes[i][j]], num_of_x) = routes[i][j - 1] == ((ElemNode*)m_nodes[routes[i][j]])->in ? 1 : -1;
			}
	for (int j = 0; j < num_of_node; j++)
		if (m_nodes[j]->m_type == ELEM)
		{
			get_i_j(matrix,i,elem_volt_to_idx[j], num_of_x) = 1;
			get_i_j(matrix,i,elem_cur_to_idx[j], num_of_x) = -((ElemNode*)m_nodes[j])->m_res;
			i++;
		}
		else if (m_nodes[j]->m_type == NODE)
		{
			for (int k = 0; k < m_nodes[j]->num_of_neigh; k++)
				if (m_nodes[m_nodes[j]->m_neighbors[k]]->m_type == ELEM)
					get_i_j(matrix,i,elem_cur_to_idx[m_nodes[j]->m_neighbors[k]], num_of_x) = (j == ((ElemNode*)m_nodes[m_nodes[j]->m_neighbors[k]])->in) ? 1 : -1;
				else
					get_i_j(matrix,i,node_cur_to_idx[m_edges[make_pair(j, m_nodes[j]->m_neighbors[k])]], num_of_x) = (j < m_nodes[j]->m_neighbors[k]) ? 1 : -1;
			i++;
		}
	vector<double> solution = solve(matrix, result, num_of_eq, num_of_x);
	vector<string> output;
	output.resize(num_of_x);
	for (int i = 0; i < num_of_x; i++)
	{ 
		if (i < num_of_res&&solution[i] < 0)
			solution[i] = -solution[i];
		if (fabs(solution[i]) < MIN)
			solution[i] = 0;
		output[i] = sign[i] + ":" + to_string(solution[i]);
	}
	delete[]matrix;
	delete[]result;
	return output;
}