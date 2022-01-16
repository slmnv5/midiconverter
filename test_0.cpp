// In a Catch project with multiple files, dedicate one file to compile the
// source code of Catch itself and reuse the resulting object file for linking.
// Let Catch provide main():
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

// ^^^
// Normally no TEST_CASEs in this file.

// #Compile & run:
// CATCH_SINGLE_INCLUDE=/usr/include/catch2
// g++ -std=c++11 -Wall -I$CATCH_SINGLE_INCLUDE -c test_main.cpp
// g++ -std=c++11 -Wall -I$CATCH_SINGLE_INCLUDE -o test_exe test_main.o test_1.cpp
// ./test_exe --success

