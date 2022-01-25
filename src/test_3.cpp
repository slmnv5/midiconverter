#include "pch.hpp"
#include "MidiEvent.hpp"
#include "catch.hpp"

TEST_CASE("Test 2", "[all]") {
	SECTION("Section 3") {
		MidiEvent e1("n,1,60,33"), e2("n,1,59,33"), e4("c,1,12,33"), e5(
				"c,1,100,33");
		REQUIRE_THROWS_AS(MidiEvent("o,1,60,33"), MidiAppError);
	}
}
