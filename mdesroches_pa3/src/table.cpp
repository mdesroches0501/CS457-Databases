// /////////////////////////////////////////////////////////////////////////////////
// Author = Michael DesRoches
// Date = 5/06/20
// Project = PA3
// Class = CS457
///////////////////////////////////////////////////////////////////////////////////
#include "table.h"

using namespace std;	

/*---------------------------------------------------------------------------------
 |  Function: Table()
 |  Purpose: constructor
 |  Parameters: name, _artributes, database
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
Table::Table(string name, string _artributes, string database)
{
	table_name = name;
	table_parameters = _artributes;
	DB = database;
}
/*---------------------------------------------------------------------------------
 |  Function: ~Table()
 |  Purpose: destructor
 |  Parameters: NONE
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
Table::~Table()
{
	table_name = "";
	table_parameters = "";
	DB = "";
}
/*---------------------------------------------------------------------------------
 |  Function: getName()
 |  Purpose: initializes the name of the table 
 |  Parameters: NONE
 |  Returns:  the name of the table 
 *--------------------------------------------------------------------------------*/
string Table::getName()
{
	return table_name;
}
/*---------------------------------------------------------------------------------
 |  Function: get_artributes()
 |  Purpose: to return the parameters of a given input 
 |  Parameters: none 
 |  Returns:  table_parameters
 *--------------------------------------------------------------------------------*/
string Table::get_artributes()
{
	return table_parameters;
}
/*---------------------------------------------------------------------------------
 |  Function: getDatabase()
 |  Purpose: initializes the name of a database 
 |  Parameters: NONE
 |  Returns:  the name of the database 
 *--------------------------------------------------------------------------------*/
string Table::getDatabase()
{
	return DB;
}
/*---------------------------------------------------------------------------------
 |  Function: printTable()
 |  Purpose: outputs attributes in a pipe seperated format
 |  Parameters: NONE
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void Table::printTable()
{

	string line  = table_parameters;
	int pos = line.find(',');
	while(pos > 0)
	{
		line.replace(pos, 2, " | ");
		pos = line.find(',');
	}
	cout << line << endl;

	for(int i = 0; i < dataVect.size(); i++)
	{
		cout << dataVect[i][0] << '|' << dataVect[i][1] << endl;
	}
}
/*---------------------------------------------------------------------------------
 |  Function: set_artributes()
 |  Purpose: sets the table attributes 
 |  Parameters: a
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void Table::set_artributes(string a)
{
	table_parameters = a;
}
/*---------------------------------------------------------------------------------
 |  Function: intoVector()
 |  Purpose: inserts input string into vector 
 |  Parameters: input 
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void Table::intoVector(vector<string> input)
{
	dataVect.push_back(input);
}
/*---------------------------------------------------------------------------------
 |  Function: getIndex()
 |  Purpose: to get assosiated integer in relation to position in table 
 |  Parameters: colName
 |  Returns:  the location in table 
 *--------------------------------------------------------------------------------*/
int Table::getIndex(string colName)
{
	int pos = table_parameters.find(colName);
	int commaPos = table_parameters.find(",");
	int index = 0;
	while(pos > commaPos && commaPos != -1)
	{
		index++;
		commaPos = table_parameters.find(",", commaPos + 1);
	}

	return index;
}
/*---------------------------------------------------------------------------------
 |  Function: update()
 |  Purpose: updates table from given input 
 |  Parameters: whereIndex, setIndex, whereVal, setVal
 |  Returns:  position where input is to be updated 
 *--------------------------------------------------------------------------------*/
int Table::update(int whereIndex, int setIndex, string whereVal, string setVal)
{
	int counter = 0;

	for (int i = 0; i < dataVect.size(); i++)
	{
		if(dataVect[i][whereIndex] == whereVal)
		{
			dataVect[i][setIndex] = setVal; 
			counter++;
		}
	}

	return counter;
}
/*---------------------------------------------------------------------------------
 |  Function: deleteTable()
 |  Purpose: deletes table with given input 
 |  Parameters: whereIndex, whereVal, op
 |  Returns:  counter
 *--------------------------------------------------------------------------------*/
int Table::deleteTable(int whereIndex, string whereVal, char op)
{
	int counter = 0;

	if(op == 'e')
	{	
		for (int i = 0; i < dataVect.size(); i++)
		{
			if(dataVect[i][whereIndex] == whereVal)
			{
				dataVect.erase(dataVect.begin() + i);
				counter++;
			}
		}
	}
	if(op == 'g')
	{
		for (int i = 0; i < dataVect.size(); i++)
		{
			if(stoi(dataVect[i][whereIndex]) > stoi(whereVal))
			{
				dataVect.erase(dataVect.begin() + i);
				counter++;
			}
		}
	}
	if(op == 'l')
	{
		for (int i = 0; i < dataVect.size(); i++)
		{
			if(stoi(dataVect[i][whereIndex]) < stoi(whereVal))
			{
				dataVect.erase(dataVect.begin() + i);
				counter++;
			}
		}
	}

	return counter;	
}
/*---------------------------------------------------------------------------------
 |  Function: q_table()
 |  Purpose: query's the tale 
 |  Parameters: whereIndex, whereVal, vector 
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void Table::q_table(int whereIndex, string whereVal, vector<int> indices)
{

	string line  = table_parameters;
	int pos = line.find(',');
	while(pos > 0)
	{
		line.replace(pos, 2, " | ");
		pos = line.find(',');
	}

	// 
	line = line.substr(line.find("|") + 2, line.length() - line.find("|") - 1);
	//
	cout << line << endl;


	for (int i = 0; i < dataVect.size(); i++)
	{
		if(dataVect[i][whereIndex] != whereVal)
		{
			cout << dataVect[i][indices[0]] << '|' << dataVect[i][indices[1]] << endl;
		}
	}
}
/*---------------------------------------------------------------------------------
 |  Function: fileWrite()
 |  Purpose: writes to a file 
 |  Parameters: NONE
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void Table::fileWrite()
{
	ofstream fout;
	fout.clear();
	fout.open(DB + '/' + table_name + ".txt");

	string line  = table_parameters;
	int pos = line.find(',');
	while(pos > 0)
	{
		line.replace(pos, 2, " | ");
		pos = line.find(',');
	}
	fout << line << endl;

	for(int i = 0; i < dataVect.size(); i++)
	{
		fout << dataVect[i][0] << '|' << dataVect[i][1] << '|' << dataVect[i][2] << endl;
	}
	
	fout.close();
}
/*---------------------------------------------------------------------------------
 |  Function: getValue()
 |  Purpose: to initialize the size of the table 
 |  Parameters: row, col
 |  Returns:  the vector 
 *--------------------------------------------------------------------------------*/
string Table::getValue(int row, int col)
{
	return dataVect[row][col];
}