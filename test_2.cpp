#include "pch.hpp"
#include "MidiEvent.hpp"
#include "MidiRange.hpp"
#include "RuleMapper.hpp"

TEST_CASE( "Test RuleMapper 1" , "[all]" ) {
	MidiEvent e1("c,2,2,3"), e2("n,5,5,11"), e3("n,2,25,11");

	RuleMapper r1 = RuleMapper { };

	r1.parseString("n,,0:20,10:127=n,2,22,22; note rule 1");
	r1.parseString("n,,,=n,2,122,122; note rule 2");
	r1.parseString("c,,,>n,2,122,122; note rule 2");

	SECTION( "Section to string 1" ) {
		REQUIRE(r1.getSize() == 3);
		REQUIRE(
				r1.getRule(0).toString()
						== "n,0:15,0:20,10:127=n,2:2,22:22,22:22");
		REQUIRE(
				r1.getRule(1).toString()
						== "n,0:15,0:127,0:127=n,2:2,122:122,122:122");

		REQUIRE(r1.getRule(0).terminate == false);
		REQUIRE(r1.getRule(1).terminate == false);
		REQUIRE(r1.getRule(2).terminate == true);

		REQUIRE(r1.findMatchingRule(e1, 0) == 2);
		REQUIRE(r1.findMatchingRule(e2, 0) == 0);
		REQUIRE(r1.findMatchingRule(e3, 0) == 1);
	}
}

TEST_CASE("Test RuleMapper 2", "[all]") {
	MidiEvent e1("c,2,2,3"), e2("n,5,5,11"), e3("n,2,25,11");
	RuleMapper r1 = RuleMapper { "rules.txt" };

	SECTION( "Section" ) {
		REQUIRE(r1.getSize() == 4);
		string s1 = r1.getRule(0).toString();
		string s2 = r1.getRule(1).toString();
		REQUIRE(s1 == "n,0:15,0:50,10:127>n,2:2,22:22,22:22");
		REQUIRE(s2 == "n,0:15,0:127,0:127>n,2:2,122:122,122:122");
	}

}
