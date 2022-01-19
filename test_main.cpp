// custom main() and we need this in 1-st line:
#define CATCH_CONFIG_RUNNER

#include "catch.hpp"
#include "log.hpp"

int main(int argc, char *argv[]) {
	LOG::ReportingLevel() = LogLvl::DEBUG;

	int result = Catch::Session().run(argc, argv);

	return (result < 0xff ? result : 0xff);
}

// Normally no TEST_CASEs in this file.
// #Compile & run:
// g++ -c test_main.cpp
// g++ -o test_exe test_main.o test_1.cpp
// run as: ./test_exe --success

