///////////////////////////////////////// README ///////////////////////////////////////
Author = Michael DesRoches
Date = 5/06/20
Project = PA3
Class = CS457

    In this assignment you will write a program that allows a database user to make queries 
over multiple tables. That is, you will implement table joins using techniques we discussed 
in the class (e.g., nested loop). This assignment assumes the metadata and data manipulations 
on singular tables are already available, which should have been provided (implemented) in 
the first two programming assignments.

/////////////////////////////// [RUNNING INSTRUCTIONS] /////////////////////////////////

First BUILD then RUN - assignment turned in with BUILD step already completed. If there 
                    is an issue, please delete build directory and complete BUILD steps

BUILD: inside project directory
mkdir build
cd build
cmake ..
make -j4

RUN: inside build directory
./Database < ../tests/PA3_test.sql

///////////////////////////////////////////////////////////////////////////////////////