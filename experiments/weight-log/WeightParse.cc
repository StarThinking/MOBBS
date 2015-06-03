#include <iostream>
#include <fstream>
#include <vector>
#include "math.h"

using namespace std;

int main()
{
	string filename("weight.log");
	ifstream is(filename.c_str());
	int sum = 0, n = 0, n2 = 0;
	vector<double> weights;
	while(!is.eof()) {
		n2 ++;
		double tmp;
		is >> tmp;
		if(tmp == 0) continue;
		sum += tmp;
		n ++;
		weights.push_back(tmp);
	}

	double avg = sum / n;
	double t1 = 0, t2 = 0;
	for(int i = 0; i < weights.size(); i ++)
	{
		double w = weights[i];
		double tmp = (w - avg) * (w - avg);
		t1 += tmp;
	}
	double var = t1 / n;
	double dev = sqrt(var);
	cout << "sum(active/total): " << sum << "(" << n << "/" << n2 << ")" << endl;
	cout << "avg: " << avg << endl;
	cout << "var: " << var << endl;
	cout << "dev: " << dev << endl;
	is.close();
}
