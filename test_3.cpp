#include "pch.hpp"
#include "MidiEvent.hpp"
#include "NoteCounter.hpp"
#include "catch.hpp"

TEST_CASE("Test NoteCounter 1", "[all]") {
	NoteCounter c1("count.txt");

	SECTION( "Section load file 1" ) {
		REQUIRE(c1.get_size() > 2);
		REQUIRE(c1.convert_v1(60) == 80);
	}
}

TEST_CASE("Test NoteCounter 2", "[all]") {

	SECTION( "Section size check" ) {
		NoteCounter c1;
		c1.parseString("60 - 80");
		c1.parseString("12 - 90");

		REQUIRE(c1.get_size() == 2);
		REQUIRE(c1.convert_v1(60) == 80);
		REQUIRE(c1.convert_v1(12) == 90);
	}

	SECTION( "Section countable" ) {
		NoteCounter c1;
		c1.parseString("60 - 80");
		c1.parseString("12 - 90");

		MidiEvent e1("n,1,60,33"), e2("n,1,59,33"), e3("o,1,60,33"), e4(
				"c,1,12,33"), e5("c,1,100,33");

		REQUIRE(e1.v1 == 60);
		REQUIRE(c1.is_countable(e1) == true);
		REQUIRE(c1.is_countable(e2) == false);
		REQUIRE(c1.is_countable(e3) == true);
		REQUIRE(c1.is_countable(e4) == true);
		REQUIRE(c1.is_countable(e5) == false);

	}

	SECTION( "Section convert to note 1" ) {
		NoteCounter c1;
		c1.parseString("60 - 80");
		c1.parseString("12 - 90");

		MidiEvent e1("c,1,12,33"), e2("c,1,12,34"), e3("c,1,12,35"), e4(
				"c,1,12,36"), e5("c,1,0,37");

		REQUIRE(c1.convert_cc_note(e1) == 0);
		REQUIRE(c1.last_cc == e1);
		REQUIRE(c1.sent_on == false);

		REQUIRE(c1.convert_cc_note(e2) == 0);
		REQUIRE(c1.last_cc == e2);
		REQUIRE(c1.sent_on == false);

		REQUIRE(c1.convert_cc_note(e3) == 0);
		REQUIRE(c1.last_cc == e3);
		REQUIRE(c1.sent_on == false);

		REQUIRE(c1.convert_cc_note(e4) == 0);
		REQUIRE(c1.last_cc == e4);
		REQUIRE(c1.sent_on == false);

		REQUIRE(c1.convert_cc_note(e5) == 0);
		REQUIRE(c1.last_cc == e5);
		REQUIRE(c1.sent_on == false);

		REQUIRE(c1.convert_cc_note(e4) == 1);
		REQUIRE(c1.last_cc == e4);
		REQUIRE(c1.sent_on == true);

		REQUIRE(c1.convert_cc_note(e3) == 0);
		REQUIRE(c1.last_cc == e3);
		REQUIRE(c1.sent_on == true);

		REQUIRE(c1.convert_cc_note(e2) == 0);
		REQUIRE(c1.convert_cc_note(e1) == 0);
	}

	SECTION( "Section convert to note 2" ) {
		NoteCounter c1;
		c1.parseString("60 - 80");
		c1.parseString("12 - 90");
		MidiEvent e1("c,1,12,33"), e2("c,1,12,34"), e3("c,1,12,35"), e4(
				"c,1,12,36"), e5("c,1,0,37");

		REQUIRE(c1.convert_cc_note(e2) == false);
		REQUIRE(c1.convert_cc_note(e1) == true);
		REQUIRE(c1.convert_cc_note(e3) == false);
		REQUIRE(c1.convert_cc_note(e2) == true);
		REQUIRE(c1.convert_cc_note(e5) == true);

	}

}
