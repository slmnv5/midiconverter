#include "pch.hpp"
#include "MidiEvent.hpp"
#include "NoteCounter.hpp"
#include "catch.hpp"

TEST_CASE("Test NoteCounter 1", "[all]") {
	NoteCounter c1 { };
	c1.parseString("60 - 80");
	c1.parseString("12 - 90");

	SECTION( "Section size check" ) {
		REQUIRE(c1.get_size() == 2);
		REQUIRE(c1.convert_v1(60) == 80);
		REQUIRE(c1.convert_v1(12) == 90);
	}

	SECTION( "Section need count" ) {
		MidiEvent e1("n,1,60,33"), e2("n,1,59,33"), e3("o,1,60,33"), e4(
				"c,1,12,33"), e5("c,1,100,33");

		REQUIRE(e1.v1 == 60);
		REQUIRE(c1.is_countable_note(e1) == true);
		REQUIRE(c1.is_countable_note(e2) == false);
		REQUIRE(c1.is_countable_note(e3) == true);
		REQUIRE(c1.is_countable_note(e4) == false);
		REQUIRE(c1.is_countable_note(e5) == false);

	}

	SECTION( "Section need count, same CC " ) {
		MidiEvent e1("c,1,12,33"), e2("c,1,12,34"), e3("c,1,12,35"), e4(
				"c,1,12,36"), e5("c,1,0,37");

		REQUIRE(c1.is_countable_note(e1) == false);
		REQUIRE(c1.is_countable_note(e2) == true);
		REQUIRE(c1.is_countable_note(e3) == true);
		REQUIRE(c1.is_countable_note(e4) == true);
		REQUIRE(c1.is_countable_note(e5) == false);

	}

}
