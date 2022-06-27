#include <iostream>
#include <vector>
#include "utils.h"
#include "grammar.h"
#include "reg.h"

using namespace std;

int main() {

	//bool invalid_input = true; 
	//string formula = "(F[2,59] G[3,9] (&[(p0vp8),(p1R[1, 3] p9),p2,p3]) R[0,3] (p1 & G[1, 5]p11))";
	//formula = strip_char(formula, ' ');
	//cout << formula << endl;

	///*while (invalid_input) {
	//	cout << "Input MLTL formula: ";
	//	cin >> formula;

	//	if (!Wff_check(formula)) {
	//		cout << formula << endl;
	//		cout << "Not a well formed formula!" << endl;
	//	}
	//	else { invalid_input = false; }
	//}*/

	//cout << Wff_check(formula) << endl;
	//cout << Comp_len(formula) << endl;

	string s = "~p4";
	int n = 6;

	vector<string> v = reg_prop_var(s, n);
	print(v);

    return 0;
}
