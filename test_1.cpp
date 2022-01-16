#include "pch.hpp"
#include "MidiEvent.hpp"
#include "MidiRange.hpp"
#include "utils.hpp"

TEST_CASE( "Test split_string 1" , "[all][basic]" ) {

	SECTION( "Section split parts 1" ) {
		auto parts = split_string("n,5,,,", ",");
		REQUIRE(parts.size() == 5);
	}

	SECTION( "Section split parts 2" ) {
		string s("12 - 15");
		auto parts = split_string(s, "=");
		REQUIRE(parts.size() == 1);
	}

	SECTION( "Section split parts 3" ) {
		string s("");
		auto parts = split_string(s, "=");
		REQUIRE(parts.size() == 1);
	}

	SECTION( "Section split parts 4" ) {
		string s("");
		replace_all(s, ">", "=");
		REQUIRE(s.empty() == true);
	}

	SECTION( "Section split parts 5" ) {
		string s(">>>>>>");
		replace_all(s, ">", "");
		REQUIRE(s.empty() == true);
	}

	SECTION( "Section split parts 6" ) {
		string s("some");
		replace_all(s, "", "=");
		REQUIRE(s.empty() == false);
	}

}

TEST_CASE( "Test ValueRange 1" , "[all][basic]" ) {
	ValueRange r0, r1, r2, r3, r4;
	ChannelRange r5;
	r1.init("    1 : 2    ; comment : 1: 2:");
	r2.init("   ; comment : 1: 2:");
	r3.init(to_string(20));
	r4.init(" 1: 222");
	r5.init("1");

	SECTION( "Section range 1" ) {
		REQUIRE((r0.lower == 0 && r0.upper == MIDI_MAX));
		REQUIRE((r1.lower == 1 && r1.upper == 2));
		REQUIRE((r2.lower == 0 && r2.upper == MIDI_MAX));
		REQUIRE((r3.lower == r3.upper));
		REQUIRE((r4.lower == 1 && r4.upper == 222 % (MIDI_MAX + 1)));
		REQUIRE((r5.lower == 1 && r5.upper == 1));
	}

	SECTION( "Section range exception" ) {
		REQUIRE_THROWS_AS(r1.init("  1 :  2:  5"), MidiAppError);
	}
}

TEST_CASE("Test MidiEvent 1", "[all][basic]") {

	SECTION( "Section to string 1" ) {
		MidiEvent ev = MidiEvent("n,10,20,70");
		REQUIRE(ev.toString() == "n,10,20,70");

	}
	SECTION( "Section error 1" ) {
		REQUIRE_THROWS_AS(MidiEvent("n,10,20,70,"), MidiAppError);
		REQUIRE_THROWS_AS(MidiEvent("n,10,"), MidiAppError);
	}
}

TEST_CASE("Test MidiEventRange 1", "[all][basic]") {

	MidiEventRange r1 = MidiEventRange("n,,,,", false);

	SECTION( "Section rule" ) {
		REQUIRE(r1.isOutEvent == false);
		REQUIRE(r1.ch.toString() == "0:15");
	}
}

TEST_CASE("Test MidiEventRule 1", "[all][basic]") {

	MidiEventRule rule = MidiEventRule("n,5,,,>n,2,3,5");

	SECTION( "Section rule" ) {
		REQUIRE(rule.terminate == true);
	}
	SECTION( "Section rule exception" ) {
		REQUIRE_THROWS_AS(MidiEventRule("n,5,,,>n,2:3,3,5"), MidiAppError);
		REQUIRE_THROWS_AS(MidiEventRule("n,5,,,>n,2:3,,5"), MidiAppError);
		REQUIRE_THROWS_AS(MidiEventRule("n,5,,,>n,2,3:7,5"), MidiAppError);
	}

}

TEST_CASE("Test MidiEventRule 2", "[all][basic]") {

	MidiEventRule rule = MidiEventRule("n,5,,,=n,2,3,5");
	MidiEvent e1("n,5,22,33"), e2("n,6,33,22");

	SECTION( "Section rule" ) {
		REQUIRE(rule.terminate == false);
		REQUIRE(rule.inEventRange.match(e1));
		REQUIRE(!rule.inEventRange.match(e2));
	}

	SECTION( "Section rule exception" ) {
		REQUIRE_THROWS_AS(rule.outEventRange.match(e1), MidiAppError);
	}

}
