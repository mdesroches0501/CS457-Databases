// ///////////////////////////////////////////////////////////////////////////
// Author = Michael DesRoches
// Date = 5/06/20
// Project = PA3
// Class = CS457
//////////////////////////////////////////////////////////////////////////////
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

using namespace std;

// this class holds the values for tables that are created
class Table {

public:
	Table(string name, string _artributes, string database);
	~Table();
	string getName();
	string get_artributes();
	string getDatabase();
	void set_artributes(string a);
	void printTable();

	void q_table(int whereIndex, string whereVal, vector<int> indices);
	void intoVector(vector<string> input);
	int getIndex(string colName);
	int update(int whereIndex, int setIndex, string whereVal, string setVal);
	int deleteTable(int whereIndex, string whereVal, char op);
	void fileWrite();

	string getValue(int row, int col); 

private:
	string table_name;
	string table_parameters;
	string DB;
	vector <vector<string> > dataVect;
};