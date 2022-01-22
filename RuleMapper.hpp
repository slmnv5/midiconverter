#ifndef RULEMAPPER_H
#define RULEMAPPER_H
#include "pch.hpp"
#include "MidiEvent.hpp"
#include "log.hpp"
using namespace std;

class MidiClient;

class RuleMapper {

	typedef std::chrono::steady_clock the_clock;
	typedef the_clock::time_point time_point;
	typedef std::chrono::milliseconds millis;
	millis millis_600 { 600 };
public:
	RuleMapper(const string &fileName, const MidiClient&);

	int findMatchingRule(const MidiEvent&, int startPos = 0) const;
	void parseString(const string&);
	bool applyRules(MidiEvent &ev);
	MidiEventRule& getRule(int i) {
		return rules[i];
	}
	size_t getSize() const {
		return rules.size();
	}
	const string toString() const;

private:
	const MidiClient &midi_client;
	time_point prev_moment = the_clock::now();
	MidiEvent prev_ev;
	int count_on = 0;
	int count_off = 0;
	bool sent_on = false;

	vector<MidiEventRule> rules;

	void count_event(const MidiEvent &ev);
	void send_event_delayed(const MidiEvent &ev, int cnt_on, int cnt_off);

};

#endif
