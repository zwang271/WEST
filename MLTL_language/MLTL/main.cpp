#include <iostream>
#include <vector>
#include "utils.h"
#include "grammar.h"
#include "nnf_grammar.h"
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

	// string s = "F[2,4]p1";
	// string s = "F[2,4]~p1";
	string s = "G[3,5]~p0";
	int n = 4;

	//vector<string> v = right_or(reg(s, n), 0, right_or_aux(reg(s, n), n), n);
	vector<string> v = reg(s, n);
	print(v);


    return 0;
}
