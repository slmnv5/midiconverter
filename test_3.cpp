#include "pch.hpp"
#include "MidiEvent.hpp"
#include "NoteCounter.hpp"

TEST_CASE("Test NoteCounter 1", "[all]") {
	NoteCounter c1 { };
	c1.parseString("60 - 80");
	c1.parseString("62 - 90");

	SECTION( "Section size check" ) {
		REQUIRE(c1.get_size() == 2);
		REQUIRE(c1.convert_v1(60) == 80);
		REQUIRE(c1.convert_v1(62) == 90);
	}

	MidiEvent e1("n,60,22,33"), e2("n,59,33,22"), e3("o,60,33,22"), e4(
			"c,12,33,22"), e5("c,100,33,22");
	SECTION( "Section need count" ) {
		REQUIRE(e1.v1 == 60);
		REQUIRE(c1.need_count(e1) == true);
		REQUIRE(c1.need_count(e2) == false);
		REQUIRE(c1.need_count(e3) == true);
		REQUIRE(c1.need_count(e4) == true);
		REQUIRE(c1.need_count(e5) == false);

	}

}
