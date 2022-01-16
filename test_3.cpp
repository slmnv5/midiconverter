#include "pch.hpp"

#include "NoteCounter.hpp"

TEST_CASE("Test NoteCOunter 1", "[all]") {
	NoteCounter c1("count.txt");

	SECTION( "Section" ) {
		REQUIRE(c1.get_size() == 6);
	}

}
