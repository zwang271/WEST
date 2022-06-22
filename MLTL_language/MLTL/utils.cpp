#include<string>

using namespace std;

string string_intersect(string w_1, string w_2, int n){
string w = "";
if (w_1.length() > w_2.length()) {
	for (int i = 0; i < w_1.length() - w_2.length(); i++) {
		w_2 += string(n, 's') + ',';
	}
}
else if (w_1.length() < w_2.length()) {
	for (int i = 0; i < w_2.length() - w_1.length(); i++) {
		w_1 += string(n, 's') + ',';
	}
}

return w_1;
}