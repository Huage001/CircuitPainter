#include "stdafx.h"
#include "equation.h"

vector<double> solve(double * a, double * b, int eq, int n)
{
	MatrixXd A(eq, n);
	for (int i = 0; i < eq; i++)
		for (int j = 0; j < n; j++)
			A(i, j) = get_i_j(a, i, j, n);
	VectorXd B(eq);
	for (int i = 0; i < eq; i++)
		B(i) = b[i];
	VectorXd ANS = A.colPivHouseholderQr().solve(B);
	vector<double>ans;
	for (int i = 0; i < n; i++)
		ans.push_back(ANS(i));
	return ans;
}

