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
#include "database.h"

/*---------------------------------------------------------------------------------
 |  Function: main()
 |
 |  Purpose:  driver function
 |
 |  Parameters: arguments through command line
 |
 |  Returns:  0
 *--------------------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
  Database::Initialize();
  Database database;

  if (argc > 1) {
    std::ifstream file(argv[1]);
    database.run(file);
    file.close();
  }

  return 0;
}