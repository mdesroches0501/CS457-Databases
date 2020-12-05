// //////////////////////////////////////////////////////////////////
// Author = Michael DesRoches
// Date = 5/06/20
// Project = PA3
// Class = CS457
/////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <cctype>
#include <sstream>
#include <algorithm>

#include "table.h"


using namespace std;

void cleanUp(string& input);
void caseChange(string& input);
string toUPPER(string input);
void loadVectorInput(string input, vector<string>& vectorLoad);
void run();