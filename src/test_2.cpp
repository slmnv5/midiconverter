#include "pch.hpp"
#include "MidiEvent.hpp"
#include "RuleMapper.hpp"
#include "MidiClient.hpp"
#include "catch.hpp"

TEST_CASE("Test RuleMapper 1", "[all]") {
	MidiClient c1("abc", nullptr);
	RuleMapper r1("", &c1);
	r1.parseString("n,,0:20,10:127=n,2,22,22=  p ; note rule 1");
	r1.parseString("n,,,=n,2,122,  =p; note rule 2");
	r1.parseString("c,,,=n,2,122,0 =p; note rule 2");

	MidiEvent e1("c,2,2,3"), e2("n,5,5,11"), e3("n,2,25,11");
	SECTION("Section to string 1") {
		REQUIRE(r1.getSize() == 3);
		string s0 = r1.getRule(0).toString();
		REQUIRE(s0 == "n,0:15,0:20,10:127=n,2:2,22:22,22:22=p");
		string s1 = r1.getRule(1).toString();
		REQUIRE(s1 == "n,0:15,0:127,0:127=n,2:2,122:122,0:127=p");
		string s2 = r1.getRule(2).toString();
		REQUIRE(s2 == "c,0:15,0:127,0:127=n,2:2,122:122,0:0=p");

		REQUIRE(r1.findMatchingRule(e1, 0) == 2);
		REQUIRE(r1.findMatchingRule(e2, 0) == 0);
		REQUIRE(r1.findMatchingRule(e3, 0) == 1);
	}
}

TEST_CASE("Test RuleMapper 2", "[all]") {
	MidiClient c1("abc", nullptr);
	RuleMapper r1("", &c1);
	r1.parseString("n,,,=n,,,0=p");

	MidiEvent e1("n,2,2,3"), e2("n,5,5,11"), e3("c,2,25,11");
	SECTION("Section to string 1") {
		REQUIRE(r1.getSize() == 1);
		string s0 = r1.getRule(0).toString();
		REQUIRE(s0 == "n,0:15,0:127,0:127=n,0:15,0:127,0:0=p");

		REQUIRE(r1.findMatchingRule(e1, 0) == 0);
		REQUIRE(r1.findMatchingRule(e2, 0) == 0);
		REQUIRE(r1.findMatchingRule(e3, 0) == -1);
	}
}
