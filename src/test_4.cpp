#include "pch.hpp"
#include "MidiEvent.hpp"
#include "utils.hpp"
#include "catch.hpp"

TEST_CASE("Test MidiEventRange", "[all][basic]") {
	SECTION("Section rule") {
		InMidiEventRange r1("n,,,");
		REQUIRE(r1.ch.toString() == "0:15");
	}
}

TEST_CASE("Test MidiEventRule 1", "[all][basic]") {

	SECTION("Section rule") {
		MidiEventRule r1("  n,  5,     ,  =n,  2   ,3,5=s ; comment");
		REQUIRE(r1.toString() == "n,5:5,0:127,0:127=n,2:2,3:3,5:5=s");
		REQUIRE_THROWS_AS(MidiEventRule("n,5,,=n,2:3,3,5=s"), MidiAppError);
		REQUIRE_THROWS_AS(MidiEventRule("n,5,,=n,2:3,,5,s"), MidiAppError);
		REQUIRE_THROWS_AS(MidiEventRule("n,5,,=n,2,3:7,5=p"), MidiAppError);

		MidiEventRule r2("n,5,,=n,,,3=c;count rule is correct");
		REQUIRE_THROWS_AS(MidiEventRule("n,5,,=ca"), MidiAppError);
		REQUIRE_THROWS_AS(MidiEventRule("c,5,,=c,,,=n"), MidiAppError);
		REQUIRE_THROWS_AS(MidiEventRule("n,5,,=c,,,=n"), MidiAppError);
		REQUIRE_THROWS_AS(MidiEventRule("n,5,,1:127=c,,,=n"), MidiAppError);

	}
}

TEST_CASE("Test MidiEventRule 2", "[all][basic]") {
	SECTION("Section rule") {

		MidiEventRule rule("n,5,,=n,2,3,5=p; comment ");
		MidiEvent e1("n,5,22,33"), e2("n,6,33,22");

		REQUIRE(rule.inEventRange->match(e1));
		REQUIRE(!rule.inEventRange->match(e2));
	}
}
