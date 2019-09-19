#pragma once
#include<vector>
#include<Eigen/Dense>
using namespace Eigen;
using namespace std;

#define get_i_j(name,i,j,n) name[i*n+j]

vector<double> solve(double *a, double *b, int eq, int n);