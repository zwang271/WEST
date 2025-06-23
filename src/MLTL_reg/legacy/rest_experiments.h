#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include "utils.h"
#include "rest.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include <vector>
#include <chrono>

using namespace std;

double run_experiment(vector<string> rest_comp);

vector<string> rest_regex(int size, vector<int> array_one, vector<int> array_zero);

int main();

