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
#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/stat.h> 
#include <sys/types.h>
#include <unistd.h>
#include <bits/stdc++.h> //for uppercase and lowercase conversion
#include <regex>
#include <fstream>
#include <cstdint>


class Database {
    public:
        Database();
        ~Database();

        void run(std::istream& file = std::cin);

        bool get_command(std::istream& file); 
        bool table_exists(const std::string& table);
        void create_database(const std::string& database); //creates database w/command CREATE DATABASE <name>
        void drop_database(const std::string& database); 
        void use_database(const std::string& database);    //uses database to apply tables

        void create_table(const std::string& table, const std::string& columns); //should create a table from exsisting database
        void drop_table(const std::string& table); //deletes table form database
        void alter_table(const std::string& table, const std::string& columns);  
        void select_table(const std::string& table);

    private:

        const std::string DATABASE_PATH = "../databases/";

        //regex is used to tokenize user input accordingly
        const std::regex CREATE_DB_REGEX = std::regex("[cC][rR][eE][aA][tT][eE]\\s+[dD][aA][tT][aA][bB][aA][sS][eE]\\s+([\\d\\w\\W\\-_.]+);");
        const std::regex DROP_DB_REGEX = std::regex("[dD][rR][oO][pP]\\s+[dD][aA][tT][aA][bB][aA][sS][eE]\\s+([\\d\\w\\W\\-_.]+);");
        const std::regex USE_DB_REGEX = std::regex("[uU][sS][eE]\\s+([\\d\\w\\W\\-_\\.]+);$");
        
        const std::regex CREATE_TABLE_REGEX = std::regex("[cC][rR][eE][aA][tT][eE]\\s+[tT][aA][bB][lL][eE]\\s+([\\d\\w\\W\\-_.]+)\\s+\\((.*)\\);");
        const std::regex DROP_TABLE_REGEX = std::regex("[dD][rR][oO][pP]\\s+[tT][aA][bB][lL][eE]\\s([\\d\\w\\W\\-_.]+);");
        const std::regex ALTER_TABLE_REGEX = std::regex("[aA][lL][tT][eE][rR]\\s+[tT][aA][bB][lL][eE]\\s+([\\d\\w\\W\\-_\\.]+)\\s+[aA][dD][dD]\\s+(.*);");
        const std::regex SELECT_REGEX = std::regex("[sS][eE][lL][eE][cC][tT]\\s+\\*\\s+[fF][rR][oO][mM]\\s+([\\d\\w\\W\\-_.]+);"); 

        std:: string active_db; //used for database activity
};