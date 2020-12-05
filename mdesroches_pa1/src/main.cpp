/*=============================================================================
 |   Assignment:  Project Assignment 1
 |
 |       Author:  Michael Bradley DesRoches
 |     Language:  C++ GNU 
 |   To Compile:  access build folder
 |                "cmake .." then "make" then "./Database"
 |                MORE DETAILED explination in README
 |
 |        Class:  CS 457 - Databases
 |                
 |   Instructor:  NAME OF YOUR COURSE'S INSTRUCTOR
 |     Due Date:  20 Feb 20 1159 P.m.
 |
 +-----------------------------------------------------------------------------
 |
 |  Description: A basic database management system tool;
 |      SOLVE.
 |
 |  Input:  requires either lowercase input or capitalized, doeasn't matter.
 |
 |  Output: ouputs any erors with commands and database into the databases
 |          folder as a directory where tables are stored in those directories. 
 |   
 |  Algorithm:  NONE
 |
 |  Required Features Not Included:  DESCRIBE HERE ANY REQUIREMENTS OF
 |      THE ASSIGNMENT THAT THE PROGRAM DOES NOT ATTEMPT TO SOLVE.
 |
 |   Known Bugs: 
 |    
 *===========================================================================*/
#include "main.h"

Database::Database() {
    //constructor
}

Database::~Database(){
    //destructor
}
/*---------------------------------------------------------------------------------
 |  Function: create_table()
 |
 |  Purpose:  creates a table as a file under a database directory
 |
 |  Parameters: table - the name of the table to create givin user's third arg
 |              columns - creates columns within text file with parameter given 
 |                        by user in parenthesis
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void Database::create_table(const std::string& table, const std::string& columns){

    std::string path = DATABASE_PATH + active_db + "/" + table;

    if(active_db.empty())
        std::cout << "No active Database " << std::endl;
    
    else{
        std::ifstream ifile(path);
        if(!table_exists(table)){
            std::cout << "Creating Table " << std::endl;
            std::ofstream outfile (path);
            outfile << columns << std::endl;
            outfile.close();
        }
        else
            std::cout << "Error: "<< table << " already exists" << std::endl;
    }
}

/*---------------------------------------------------------------------------------
 |  Function: table_exists()
 |
 |  Purpose:  check to see if table already exists
 |
 |  Parameters: table - the name of the table to check for existance
 |             
 |  Returns:  True if table exists, false if the table does not exist
 *--------------------------------------------------------------------------------*/
bool Database::table_exists(const std::string& table){

    std::string path = DATABASE_PATH + active_db + "/" + table;

    struct stat buffer;
    return (stat (path.c_str(), &buffer) == 0);
}

/*---------------------------------------------------------------------------------
 |  Function: create_database()
 |
 |  Purpose:  creates database named by third arg from user. Checks for duplicate 
 |            file before creating as a directory
 |
 |  Parameters: database - the name of the database to create
 |             
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void Database::create_database(const std::string& database){

    std::string path = DATABASE_PATH + database;

    if (mkdir(path.c_str(), 0777) == -1) 
        std::cerr << "Error :  " << strerror(errno) << std::endl; 
  
    else
        std::cout << "Creating Database: " << database << std::endl;
}

/*---------------------------------------------------------------------------------
 |  Function: run()
 |
 |  Purpose:  runs get_command() on while true
 |
 |  Parameters: file - argc[2] from command line
 |             
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void Database::run(std::istream& file) {
    while (get_command(file));
}

/*---------------------------------------------------------------------------------
 |  Function: drop_database()
 |
 |  Purpose:  deletes existing database 
 |
 |  Parameters: database - database to be deleted if it exists or there are no 
 |                         tables inside of directory
 |             
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void Database::drop_database(const std::string& database){

    std::string path = DATABASE_PATH + database;

    std::cout << "Attempting to drop database " << path << std::endl;
    if (rmdir(path.c_str()) == -1) {
        std::cerr << "Error: " << strerror(errno) << std::endl;
    } else {
        std::cout << "Dropping database: " << database << std::endl;
    }
}

/*---------------------------------------------------------------------------------
 |  Function: drop_table()
 |
 |  Purpose:  checks if in active database. If so, removes files("tables")
 |
 |  Parameters: table - the name of the table to check for existance
 |             
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void Database::drop_table(const std::string& table){

    std::string path = DATABASE_PATH + active_db + "/" + table;

    if(active_db.empty())
        std::cout << "No active Database " << std::endl;

    else{
        if(remove(path.c_str()) != 0 )
            perror( "Error deleting file " );
        else
            puts( "File successfully deleted" );
    }
}

/*---------------------------------------------------------------------------------
 |  Function: use_database()
 |
 |  Purpose:  sets active database if it exists for table access
 |
 |  Parameters: database - the name of the database to set as "active"
 |             
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void Database::use_database(const std::string& database){

    std::string path = DATABASE_PATH + database;

    if (mkdir(path.c_str(), 0777) == -1){
        active_db = database; 
        std::cout << "Using Database " << active_db << std::endl;
    }
    else{
        std::cout << "Error: Database does not exist " << std::endl;
        rmdir(path.c_str());
    }
       
}

/*---------------------------------------------------------------------------------
 |  Function: alter_table()
 |
 |  Purpose:  check if inside active database and if table exists. If so, 
 |            appends existing table with new contents given by user
 |
 |  Parameters: table - the name of the table to edit
 |              comlumns - the information to append to table
 |             
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void Database::alter_table(const std::string& table, const std::string& columns) {

    std::string path = DATABASE_PATH + active_db + "/" + table;

    if(active_db.empty())
        std::cout << "No active Database " << std::endl;

    else{
        if(table_exists(table)){

            std::cout << "adding: " << columns << " to " << "table\n";

            std::ofstream outfile;

            outfile.open(path, std::ios_base::app); // appends
            outfile << columns; 
        }
        else
        {
            std::cout << table << " does not exist " << std::endl;
        }
        
    }
}

/*---------------------------------------------------------------------------------
 |  Function: select table()
 |
 |  Purpose:  check to see if table already exists and theres an active database. 
 |            If so, selects table and prints information in file to terminal
 |
 |  Parameters: table - the name of the table to check for existance
 |             
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void Database::select_table(const std::string& table) {

    std::string path = DATABASE_PATH + active_db + "/" + table;

    if(active_db.empty())
        std::cout << "No active Database " << std::endl;
    else{
        if(table_exists(table)){

            std::string word;
            std::fstream file;
            file.open(path.c_str());

            while(file >> word){
                std::cout << word << " ";
            }
            std::cout << std::endl;
        }
        else
        {
            std::cout << table << " does not exist " << std::endl;
        } 
    }
}

/*---------------------------------------------------------------------------------
 |  Function: get_command()
 |
 |  Purpose:  takes uses input and switches accordingly depending on user input.
 |
 |  Parameters: file - used for arguments passed through command line (driver file)
 |             
 |  Returns:  TRUE - while command ".EXIT" does not exist
 |            FALSE - when command ".EXIT" is executed by user
 *--------------------------------------------------------------------------------*/
bool Database::get_command(std::istream& file){

    
    std::cout << "-->  ";

    std::string line;
    std::getline(file, line);

    std::cmatch match;

    if (std::regex_match(line, std::regex("\\.[eE][xX][iI][tT]\\s*"))) {
        return false;
    }

    if (std::regex_match(line.c_str(), match, CREATE_DB_REGEX)) {
        create_database(match[1]);
    } else if (std::regex_match(line.c_str(), match, DROP_DB_REGEX)) {
        drop_database(match[1]);
    } else if (std::regex_match(line.c_str(), match, USE_DB_REGEX)) {
        use_database(match[1]);
    } else if (std::regex_match(line.c_str(), match, CREATE_TABLE_REGEX)) {
        create_table(match[1], match[2]);
    } else if (std::regex_match(line.c_str(), match, DROP_TABLE_REGEX)) {
        drop_table(match[1]);
    } else if (std::regex_match(line.c_str(), match, ALTER_TABLE_REGEX)) {
        alter_table(match[1], match[2]);
    } else if (std::regex_match(line.c_str(), match, SELECT_REGEX)) {
        select_table(match[1]);
    } else {
        std::cout << "Incorrect user input: Please try again ... " << std::endl;
    }
    return true;
}

/*---------------------------------------------------------------------------------
 |  Function: main()
 |
 |  Purpose:  driver function 
 |
 |  Parameters: arguments through command line 
 |             
 |  Returns:  0
 *--------------------------------------------------------------------------------*/
int main(int argc, char * argv[]){

    Database database;

    if (argc > 1) {
        std::ifstream file(argv[1]);
        database.run(file);
    } else {
        database.run();
    }   
    return 0;
}