#include "simulation.h"


void right_or_PT1(int iterations) {
	int n = 1;
	vector<string> v = { "s", "0" };
	for (int i = 0; i < iterations; ++i) {
		if (i % 2 == 0) {
			v[0] += ",0";
			v[1] += ",s";
		}
		else {
			v[0] += ",s";
			v[1] += ",0";
		}

		print(v);
		cout << i << "\t input length: " << v[0].length() << "\t output vector size: " << simplify(v, n).size() << endl;
		print(simplify(v, n));
		cout << endl << endl;
	}
}
