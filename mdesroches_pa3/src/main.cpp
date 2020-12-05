// ////////////////////////////////////////////////////////////////////////////////
// Author = Michael DesRoches
// Date = 5/06/20
// Project = PA3
// Class = CS457
///////////////////////////////////////////////////////////////////////////////////
#include "main.h"

using namespace std;

/*---------------------------------------------------------------------------------
 |  Function: main()
 |  Purpose: start function 
 |  Parameters: argc, argv[]
 |  Returns:  int 
 *--------------------------------------------------------------------------------*/
int main(int argc, char const *argv[])
{
	run();
	return 0;
}
/*---------------------------------------------------------------------------------
 |  Function: loadVectorInput()
 |  Purpose: Loads a temp vector to be inserted into the table datavector 
 |  Parameters: input, vectorLoad
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void loadVectorInput(string input, vector<string>& vectorLoad)
{
	int lastPosition = input.find(',');
	vectorLoad.push_back(input.substr(0, lastPosition));
	int newPosition = input.find(',', lastPosition + 1);
	while(newPosition > 0)
	{
		//push into vectorLoad
		vectorLoad.push_back(input.substr(lastPosition + 1, newPosition - lastPosition - 1));

		// look for next ','
		lastPosition = newPosition;
		newPosition = input.find(',', lastPosition + 1);
	}

	vectorLoad.push_back(input.substr(lastPosition + 1, input.length() - lastPosition));
}
/*---------------------------------------------------------------------------------
 |  Function: cleanUp()
 |  Purpose: eliminates carriage returns 
 |  Parameters: input 
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void cleanUp(string& input)
{
	string a;

	if (input.find('\r') == string::npos)
	{
		return;
	}
	else
	{
		for(int i = 0; i < input.size(); i++)
		{
			if (input[i] == '\r')
			{
				continue;
			}
			else
			{
				a.push_back(input[i]);
			}
		}
		input = a;
		return;
	}
}
/*---------------------------------------------------------------------------------
 |  Function: caseChange()
 |  Purpose: changes input to uppercase letters
 |  Parameters: inputString
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void caseChange(string& inputString)
{
	string test = inputString;
	string wordBank[] = {"FROM","CREATE","UPDATE","ALTER","DROP","INSERT","INTO","SET","WHERE","SELECT",
						   "VALUES","USE","DATABASE","TABLE","DELETE",".EXIT", "ON", "LEFT", "OUTER", 
						   "JOIN", "INNER"};
	string wordCheck;
	string cleanedUpString = "";

	vector<string> tokens;
	string token;
	istringstream tokenStream(test);

	while (getline(tokenStream, token, ' '))
	{
		tokens.push_back(token);
		
	}

	size_t wordBankElements = sizeof( wordBank ) / sizeof( wordBank[0] );
	int wordBankSize = static_cast<int>(wordBankElements);

	for(int i=0; i<tokens.size();i++)
	{
		wordCheck = tokens[i];
		for(int iterate =0; iterate < wordCheck.size();iterate++)
		{
			wordCheck[iterate]=toupper(wordCheck[iterate]);
		}
		for(int j=0; j< wordBankSize;j++)
		{
			
			if(wordCheck == wordBank[j])
			{
			
				tokens[i]=wordCheck;
				break;
			}
		}
			//generates real string. 
			if(i == (tokens.size()-1))
			{
				cleanedUpString = cleanedUpString + tokens[i];
			}
			else
			{
				cleanedUpString = cleanedUpString + tokens[i] + ' ';
			}
	}
	inputString = cleanedUpString;
}
/*---------------------------------------------------------------------------------
 |  Function: run()
 |  Purpose: driver function for program 
 |  Parameters: NONE
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void run()
{
	string line = "";
	size_t found;
	string name = "unnamed";
	int errorNum;
	string database = "";
	string _artributes = "";
	vector<Table> vectorForTable;
	int error = 0;
	string append_to_command = "";

	while(line != ".EXIT")
	{
		// manual input 
		getline(cin, line);
		cleanUp(line);

		while(line.find(';') == -1 && line.find('.') != 0 && line != "" && line.find("--") == -1)
		{
			getline(cin, append_to_command); // manual input 
			cleanUp(append_to_command); // for manual input 

			// appends command when running test script 
			if (append_to_command != "")
			{
				line += append_to_command;
			}
			
		}

		// \r removal process 
		cleanUp(line);

		// change to uppercase 
		caseChange(line);

		// ignores comments in test file 
		if(line.find("--") != -1)
		{
			continue;
		}

		// create database handler that creates a folder with given input name
		else if(line.find("CREATE DATABASE") != -1)
		{	
			name = line.substr(16, line.length() - 17);
			errorNum = system(("mkdir " + name).c_str());
			if(errorNum == 0)
			{
				cout << "-- Database " + name + " created." << endl;
			}
			
		}
		// delete database 
		else if(line.find("DROP DATABASE") != -1)
		{
			name = line.substr(14, line.length() - 15);
			errorNum = system(("rmdir " + name).c_str());
			if(errorNum == 0)
			{
				cout << "-- Database " + name + " deleted." << endl;
			}
		}
		// create table handler
		// Creates a Table and assigns name and database associated with it
		else if(line.find("CREATE TABLE") != -1)
		{
			name = line.substr(13, line.find("(") - 13);

			for(int i = 0; i < vectorForTable.size(); i++)
			{
				if(name == vectorForTable[i].getName() && database == vectorForTable[i].getDatabase())
				{
					cout << "-- Failed to create table " + name + " becuase it already exists." << endl;
					error = 1;
					break;
				}
			}
			if(error == 0)
			{
				if(database != "")
				{
					errorNum = system(("touch " + database + "/" + name + ".txt").c_str());
					if(errorNum == 0)
					{
						_artributes = line.substr(line.find("(") + 1, line.length() - 3 - line.find("("));
						Table* pushT = new Table(name, _artributes, database);
						vectorForTable.push_back(*pushT);
						cout << "-- Table " + name + " created." << endl;

					}
				}
				else
				{
					cout << "-- Failed to create " + name + " becuase you are not using a database." << endl;
				}
			}

		}
		// Drop table that deletes table from parent database
		else if(line.find("DROP TABLE") != -1)
		{
			int i;
			name = line.substr(11, line.length() - 12);
			errorNum = system(("rm " + database + "/" + name + ".txt").c_str());
			if (errorNum == 0)
			{
				for (i = 0; i < vectorForTable.size(); ++i)
				{
					if (name == vectorForTable[i].getName() && database == vectorForTable[i].getDatabase())
					{
						vectorForTable.erase(vectorForTable.begin() + i);
					}

				}
				cout << "-- Table " + name + " deleted." << endl;
			}
		}
		// Changes functioning database
		else if(line.find("USE") != -1)
		{
			database = line.substr(4, line.length() - 5);
			errorNum = system(("cd " + database).c_str());
			if(errorNum == 0)
			{
				cout << "-- Using Database " + database + "." << endl;
			}

		}
		// handles left outer join
		else if(line.find("LEFT OUTER JOIN") != -1) //start new
		{
			name = line.substr(line.find("FROM") + 5, line.find("LEFT") - line.find("FROM") - 8);
			string name2 = line.substr(line.find("JOIN") + 5, line.find("ON") - line.find("JOIN") - 8);

			string attr = line.substr(line.find('.') + 1, line.find('=') - line.find('.') - 2);
			string attr2 = line.substr(line.find_last_of('.') + 1, line.find(';') - line.find('=')- 4);

			int Tindex1, Tindex2;
			int attrIndex, attr2Index;

			for (Tindex1 = 0; Tindex1 < vectorForTable.size(); Tindex1++)
			{
				if(name == vectorForTable[Tindex1].getName() && database == vectorForTable[Tindex1].getDatabase())
				{
			 		attrIndex = vectorForTable[Tindex1].getIndex(attr);
			 		break;
			 	}
			 }
			for (Tindex2 = 0; Tindex2 < vectorForTable.size(); Tindex2++)
			{
				if(name2 == vectorForTable[Tindex2].getName() && database == vectorForTable[Tindex2].getDatabase())
				{
		 			attr2Index =  vectorForTable[Tindex2].getIndex(attr2);
		 			break;
			 	}
			}

			string temp1, temp2;
			int k = 0;
			bool Lflag = 0;

			string line3  = vectorForTable[Tindex1].get_artributes();
			int pos = line3.find(',');
			while(pos > 0)
			{
				line3.replace(pos, 2, " | ");
				pos = line3.find(',');
			}

			string line4  = vectorForTable[Tindex2].get_artributes();
			int pos2 = line4.find(',');
			while(pos2 > 0)
			{
				line4.replace(pos2, 2, " | ");
				pos2 = line4.find(',');
			}

			cout << line3 << " | " << line4 << endl;

			for(int i = 0; i < 3 ;i++)
			{
				Lflag = 0;
				temp1 = vectorForTable[Tindex1].getValue(i, attrIndex);

				for(int j = 0; j < 3 ;j++)
				{
					
					temp2 = vectorForTable[Tindex2].getValue(j, attr2Index);

					if(temp1 == temp2)
					{
						Lflag = 1;
						for( k = 0; k < 2; k++)
						{
							cout << vectorForTable[Tindex1].getValue(i,k) << '|';
						}

						for(k = 0; k < 2; k++)
						{
							cout << vectorForTable[Tindex2].getValue(j,k) << '|';
						}

						cout << endl;
					}
				}
				if(Lflag == 0)
				{
					for( k = 0; k < 2; k++)
					{
						cout << vectorForTable[Tindex1].getValue(i,k) << '|';
					}
					cout << endl;
				}
			}
		}
		// Select Query handler that returns the empty table
		else if (line.find("SELECT *") != -1 && (line.find("WHERE") != -1 || line.find("ON") != -1)) //new
		{
			if(line.find("WHERE") != -1)
			{
				name = line.substr(line.find("FROM") + 5, line.find(',') - line.find("FROM") - 7);
				string name2 = line.substr(line.find(',') + 2, line.find("WHERE") - line.find(',') - 5);

				string attr = line.substr(line.find('.') + 1, line.find('=') - line.find('.') - 2);
				string attr2 = line.substr(line.find_last_of('.') + 1, line.find(';') - line.find('=')- 4);
			
				// Sets index's for both tables in vectorForTable
				int Tindex1, Tindex2;
				int attrIndex, attr2Index;


				for (Tindex1 = 0; Tindex1 < vectorForTable.size(); Tindex1++)
				{
					if(name == vectorForTable[Tindex1].getName() && database == vectorForTable[Tindex1].getDatabase())
					{
				 		attrIndex = vectorForTable[Tindex1].getIndex(attr);
				 		break;
				 	}
				 }

				for (Tindex2 = 0; Tindex2 < vectorForTable.size(); Tindex2++)
				{
					if(name2 == vectorForTable[Tindex2].getName() && database == vectorForTable[Tindex2].getDatabase())
					{
			 			attr2Index =  vectorForTable[Tindex2].getIndex(attr2);
			 			break;
				 	}
				}

				string temp1, temp2;
				int k = 0;

				string line3  = vectorForTable[Tindex1].get_artributes();
				int pos = line3.find(',');
				while(pos > 0)
				{
					line3.replace(pos, 2, " | ");
					pos = line3.find(',');
				}

				string line4  = vectorForTable[Tindex2].get_artributes();
				int pos2 = line4.find(',');
				while(pos2 > 0)
				{
					line4.replace(pos2, 2, " | ");
					pos2 = line4.find(',');
				}

				cout << line3 << " | " << line4 << endl;

				for(int i = 0; i < 3 ;i++)
				{
					temp1 = vectorForTable[Tindex1].getValue(i, attrIndex);

					for(int j = 0; j < 3 ;j++)
					{
						temp2 = vectorForTable[Tindex2].getValue(j, attr2Index);

						if(temp1 == temp2)
						{
							for( k = 0; k < 2; k++)
							{
								cout << vectorForTable[Tindex1].getValue(i,k) << '|';
							}

							for(k = 0; k < 2; k++)
							{
								cout << vectorForTable[Tindex2].getValue(j,k) << '|';
							}

							cout << endl;
						}
					}
				} 

			}

			else if(line.find("ON") != -1)
			{
				name = line.substr(line.find("FROM") + 5, line.find("INNER") - line.find("FROM") - 8);
				string name2 = line.substr(line.find("JOIN") + 5, line.find("ON") - line.find("JOIN") - 8);

				string attr = line.substr(line.find('.') + 1, line.find('=') - line.find('.') - 2);
				string attr2 = line.substr(line.find_last_of('.') + 1, line.find(';') - line.find('=')- 4);

				int Tindex1, Tindex2;
				int attrIndex, attr2Index;


				for (Tindex1 = 0; Tindex1 < vectorForTable.size(); Tindex1++)
				{
					if(name == vectorForTable[Tindex1].getName() && database == vectorForTable[Tindex1].getDatabase())
					{
				 		attrIndex = vectorForTable[Tindex1].getIndex(attr);
				 		break;
				 	}
				 }

				for (Tindex2 = 0; Tindex2 < vectorForTable.size(); Tindex2++)
				{
					if(name2 == vectorForTable[Tindex2].getName() && database == vectorForTable[Tindex2].getDatabase())
					{
			 			attr2Index =  vectorForTable[Tindex2].getIndex(attr2);
			 			break;
				 	}
				}

				// compare values in index's for each table
				string temp1, temp2;
				int k = 0;

				string line3  = vectorForTable[Tindex1].get_artributes();
				int pos = line3.find(',');
				while(pos > 0)
				{
					line3.replace(pos, 2, " | ");
					pos = line3.find(',');
				}

				string line4  = vectorForTable[Tindex2].get_artributes();
				int pos2 = line4.find(',');
				while(pos2 > 0)
				{
					line4.replace(pos2, 2, " | ");
					pos2 = line4.find(',');
				}

				cout << line3 << " | " << line4 << endl;

				for(int i = 0; i < 3 ;i++)
				{
					temp1 = vectorForTable[Tindex1].getValue(i, attrIndex);

					for(int j = 0; j < 3 ;j++)
					{
						temp2 = vectorForTable[Tindex2].getValue(j, attr2Index);

						if(temp1 == temp2)
						{
							for( k = 0; k < 2; k++)
							{
								cout << vectorForTable[Tindex1].getValue(i,k) << '|';
							}

							for(k = 0; k < 2; k++)
							{
								cout << vectorForTable[Tindex2].getValue(j,k) << '|';
							}

							cout << endl;
						}
					}
				}
			}
		}  

		// handles Select command that returns the empty table
		else if (line.find("SELECT *") != -1)
		{
			name = line.substr(14, line.length() - 15);
			caseChange(name);
			int i;

			for (i = 0; i < vectorForTable.size(); i++)
			{
				if(name == vectorForTable[i].getName() && database == vectorForTable[i].getDatabase())
				{
					vectorForTable[i].printTable();
					break;
				}
			}
			if(i == vectorForTable.size())
			{
				cout << "-- Table not located in this database." << endl;
			}
		}

		// adds columns to given table 
		else if(line.find("ALTER TABLE") != -1)
		{
			int i;
			name = line.substr(12, line.find("ADD") - 13);
			string addedParameters = line.substr(line.find("ADD") + 4, line.length() - (line.find("ADD") + 5));
			
			for (i = 0; i < vectorForTable.size(); i++)
			{
				if (name == vectorForTable[i].getName() && database == vectorForTable[i].getDatabase())
				{
					vectorForTable[i].set_artributes(vectorForTable[i].get_artributes() + ", " + addedParameters);
					cout << "-- Table " + name + " modified." << endl;
					break;
				}

			}
			if (i == vectorForTable.size())
			{
				cout << "-- Table does not exist in this database." << endl;
				
			}
		}
		// inserts data into table if table exists 
		else if(line.find("INSERT INTO") != -1)
		{
			int i;
			name = line.substr(12, line.find("values") - 13);

			for(i = 0; i < vectorForTable.size(); i++)
			{
				if(vectorForTable[i].getName() == name && vectorForTable[i].getDatabase() == database)
				{
					break;
				}
			}
			// if table exists, inserts input 
			if(i != vectorForTable.size())
			{
				vector<string> test;

				int pos = line.find("(");
				_artributes = line.substr(pos + 1, line.length() - 3 - pos);

				// handles spaces
				_artributes.erase(remove_if(_artributes.begin(), _artributes.end(), ::isspace), _artributes.end());

				loadVectorInput(_artributes, test);
				vectorForTable[i].intoVector(test);

				cout << "-- 1 new record inserted." << endl;
			}
			else
			{
				cout << "-- Invalid table" << endl;
			}
		}
		// Handles Update command - updates tuples based on given parameters 
		else if(line.find("UPDATE") != -1)
		{
			int i = 0;
			name = line.substr(7, line.find("SET") - 8);

			for(i = 0; i < vectorForTable.size(); i++)
			{
				if(vectorForTable[i].getName() == name && vectorForTable[i].getDatabase() == database)
				{
					break;
				}
			}

			// updates if table is found 
			if(i != vectorForTable.size())
			{
				string whereKey = line.substr(line.find("WHERE") + 6, line.find_last_of("=") - 6 - line.find("WHERE"));
				string whereVal = line.substr(line.find_last_of("=") + 2, line.find(";") - 2 - line.find_last_of("="));
				
				string setKey = line.substr(line.find("SET") + 4, line.find("=") - 5 - line.find("SET"));
				string setVal = line.substr(line.find("=") + 2, line.find("WHERE") - 3 - line.find("="));

				int whereIndex = vectorForTable[i].getIndex(whereKey);
				int setIndex = vectorForTable[i].getIndex(setKey);

				cout << vectorForTable[i].update(whereIndex, setIndex, whereVal, setVal) << " record(s) modified." << endl;
			}
			else
			{
				cout << "-- Invalid table" << endl;
			}
		}
		// Delete's given input tuples with the DELETE keyword as command 
		else if(line.find("DELETE") != -1)
		{
			int i = 0;
			name = line.substr(12, line.find("WHERE") - line.find("FROM") - 6);

			for(i = 0; i < vectorForTable.size(); i++)
			{
				if(vectorForTable[i].getName() == name && vectorForTable[i].getDatabase() == database)
				{
					break;
				}
			}
			// deletes table if table is found 
			if(i != vectorForTable.size())
			{
				string whereKey = "";
				string whereVal = "";

				// equal to
				if(line.find("=") != -1)
				{
					whereKey = line.substr(line.find("WHERE") + 6, line.find_last_of("=") - 7 - line.find("WHERE"));
					whereVal = line.substr(line.find_last_of("=") + 2, line.find(";") - 2 - line.find_last_of("="));
					int whereIndex = vectorForTable[i].getIndex(whereKey);
					char op = 'e';
					cout << vectorForTable[i].deleteTable(whereIndex, whereVal, op) << " record(s) deleted." << endl;
				}
				// less than
				else if(line.find("<") != -1)
				{
					whereKey = line.substr(line.find("WHERE") + 6, line.find_last_of("<") - 7 - line.find("WHERE"));
					whereVal = line.substr(line.find_last_of("<") + 2, line.find(";") - 2 - line.find_last_of("<"));	
					int whereIndex = vectorForTable[i].getIndex(whereKey);
					char op = 'l';
					cout << vectorForTable[i].deleteTable(whereIndex, whereVal, op) << " record(s) deleted." << endl;			
				}
				// greater than
				else if(line.find(">") != -1)
				{
					whereKey = line.substr(line.find("WHERE") + 6, line.find_last_of(">") - 7 - line.find("WHERE"));
					whereVal = line.substr(line.find_last_of(">") + 2, line.find(";") - 2 - line.find_last_of(">"));
					int whereIndex = vectorForTable[i].getIndex(whereKey);
					char op = 'g';
					cout << vectorForTable[i].deleteTable(whereIndex, whereVal, op) << " record(s) deleted." << endl;
				}
				else
				{
					cout << "-- Invalid Operator." << endl;
				}
			}
			else
			{
				cout << "-- Invalid table." << endl;
			}
		}
		// Outputs tuples based on given command 
		else if(line.find("SELECT") != -1)
		{
			int i = 0;
			name = line.substr(line.find("FROM") + 5, line.find("WHERE") - line.find("FROM") - 6);

			for(i = 0; i < vectorForTable.size(); i++)
			{
				if(vectorForTable[i].getName() == name && vectorForTable[i].getDatabase() == database)
				{
					break;
				}
			}
			// if the table is found
			if(i != vectorForTable.size())
			{
				if(line.find("!=") != -1)
				{
					string whereKey = line.substr(line.find("WHERE") + 6, line.find_last_of("!=") - 8 - line.find("WHERE"));
					string whereVal = line.substr(line.find_last_of("!=") + 2, line.find(";") - 2 - line.find_last_of("!="));
					int whereIndex = vectorForTable[i].getIndex(whereKey);
					char op = 'n'; // not equal to
					//
					string colNames = line.substr(7, line.find("FROM") - 7);
					vector <int> indices;
					indices.push_back(vectorForTable[i].getIndex(colNames.substr(0, colNames.find(",") - 1)));
					indices.push_back(vectorForTable[i].getIndex(colNames.substr(colNames.find(",") + 1, colNames.length() - colNames.find(",") - 1)));
					vectorForTable[i].q_table(whereIndex,whereVal,indices);
				}
				else // table is not found 
				{
					cout << "-- Invalid operator.";
				}
			}
		}
		error = 0;
	}
	cout << "-- All done." << endl;
}
