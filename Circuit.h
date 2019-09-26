#pragma once
#include<vector>
#include<map>
#include<string>
#include<cassert>
#include "Struct.h"
using namespace std;

const double INF = 99999999, MIN = 1e-6;

class Circuit
{
public:
	int num_of_node;
	vector<Node*> m_nodes;
	map<pair<int, int>, Edge*>m_edges;
	SrcNode *m_src_p, *m_src_n;
	Circuit(Graph* gph);
	Circuit(vector<Node*> nodes);
	void dfs(int s, int t, vector<vector<int>>& routes);
	vector<string> calculate(vector<vector<int>>& routes);
};