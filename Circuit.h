#pragma once
#include<vector>
#include<map>
#include<string>
#include<cassert>
#include "Struct.h"
using namespace std;

class Circuit
{
public:
	int num_of_node;
	vector<Node*> m_nodes;
	map<pair<int, int>, Edge*>m_edges;
	SrcNode *m_src_p, *m_src_n;
	Circuit(Graph* gph);
	void dfs(int s, int t, vector<vector<int>>& routes);
	vector<string> calculate(vector<vector<int>>& routes);
};