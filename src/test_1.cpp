#include "pch.hpp"
#include "MidiEvent.hpp"
#include "utils.hpp"
#include "catch.hpp"

TEST_CASE("Test LOG 1", "[all][basic]") {

	SECTION("Section print 1") {

		LOG(LogLvl::DEBUG) << "TEST0";
		LOG(LogLvl::INFO) << "TEST1";
		LOG(LogLvl::WARN) << "TEST2";
		LOG(LogLvl::ERROR) << "TEST3";

		REQUIRE(Log::ReportingLevel() == LogLvl::DEBUG);
		REQUIRE(static_cast<LogLvl>(2) == LogLvl::WARN);
		REQUIRE(Log::toString(static_cast<LogLvl>(3)) == "ERROR");
	}

	SECTION("Section print 2") {
		LOG::ReportingLevel() = LogLvl::WARN;
		LOG(LogLvl::DEBUG) << "TEST0";
		LOG(LogLvl::INFO) << "TEST1";
		LOG(LogLvl::WARN) << "TEST2";
		LOG(LogLvl::ERROR) << "TEST3";
		LOG::ReportingLevel() = LogLvl::DEBUG;
	}
}

TEST_CASE("Test split_string 1", "[all][basic]") {

	SECTION("Section split parts 1") {
		auto parts = split_string("n,5,,,", ",");
		REQUIRE(parts.size() == 5);
	}

	SECTION("Section split parts 2") {
		string s(" 12 – 40;  may send 12, 41, 42, 43, 44, 45, 46, 47  ");
		remove_spaces(s);
		auto parts = split_string(s, "=");
		REQUIRE(parts.size() == 1);
	}

	SECTION("Section split parts 3") {
		string s("");
		auto parts = split_string(s, "=");
		REQUIRE(parts.size() == 1);
	}

	SECTION("Section split parts 4") {
		string s("");
		replace_all(s, ">", "=");
		REQUIRE(s.empty() == true);
	}

	SECTION("Section split parts 5") {
		string s(">>>>>>");
		replace_all(s, ">", "");
		REQUIRE(s.empty() == true);
	}

	SECTION("Section split parts 6") {
		string s("some");
		replace_all(s, "", "=");
		REQUIRE(s.empty() == false);
	}
}

TEST_CASE("Test ValueRange 1", "[all][basic]") {

	SECTION("Section range 1") {
		ValueRange r0;
		REQUIRE(r0.toString() == "0:127");
		REQUIRE(r0.lower == 0);
		REQUIRE(r0.upper == MIDI_MAX);

		ValueRange r1("1 : 2 ");
		REQUIRE(r1.toString() == "1:2");
		REQUIRE(r1.lower == 1);
		REQUIRE(r1.upper == 2);

		ValueRange r2("   ; comment : 1: 2:");
		REQUIRE(r2.lower == 0);
		REQUIRE(r2.upper == MIDI_MAX);

		ValueRange r3(to_string(20));
		REQUIRE(r3.lower == r3.upper);
		REQUIRE(r3.lower == 20);

		REQUIRE_THROWS_AS(ValueRange("  1 : 222 "), MidiAppError);
		REQUIRE_THROWS_AS(ValueRange("  1 :  2:  5"), MidiAppError);
	}
}

TEST_CASE("Test ChannelRange 1", "[all][basic]") {

	SECTION("Section range 1") {
		ChannelRange r0;
		REQUIRE(r0.toString() == "0:15");
		REQUIRE(r0.lower == 0);
		REQUIRE(r0.upper == MIDI_MAXCH);

		REQUIRE_THROWS_AS(ChannelRange(" 1: 22"), MidiAppError);
		REQUIRE_THROWS_AS(ChannelRange("3:"), MidiAppError);

		ChannelRange r2(" 3  : 8 ");
		REQUIRE(r2.lower == 3);
		REQUIRE(r2.upper == 8);

		ChannelRange r3("3");
		REQUIRE(r3.lower == 3);
		REQUIRE(r3.upper == 3);
	}
}

TEST_CASE("Test MidiEvent 1", "[all][basic]") {

	SECTION("Section to string 1") {
		MidiEvent ev = MidiEvent("n,10,20,70");
		REQUIRE(ev.toString() == "n,10,20,70");
	}
	SECTION("Section error 1") {
		REQUIRE_THROWS_AS(MidiEvent("n,10,20,70,"), MidiAppError);
		REQUIRE_THROWS_AS(MidiEvent("n,10,"), MidiAppError);
	}
}

TEST_CASE("Test MidiEvent 2", "[all][basic]") {
	SECTION("Section range 1") {

		REQUIRE_THROWS_AS(MidiEvent("k,2,2,3"), MidiAppError);
		REQUIRE_THROWS_AS(MidiEvent("n,233,2,3"), MidiAppError);
		REQUIRE_THROWS_AS(MidiEvent(",2,2,3"), MidiAppError);
		REQUIRE_THROWS_AS(MidiEvent("a,992,2,3"), MidiAppError);
	}
}

