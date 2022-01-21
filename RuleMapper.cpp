#include "pch.hpp"
#include "MidiEvent.hpp"
#include "RuleMapper.hpp"
#include "MidiClient.hpp"
#include "log.hpp"

using namespace std;

RuleMapper::RuleMapper(const string &fileName, const MidiClient &mc) :
		midi_client(mc) {
	ifstream f(fileName);
	string s;
	int k = 0;
	while (getline(f, s)) {
		try {
			k++;
			rules.push_back(MidiEventRule(s));
		} catch (exception &e) {
			LOG(LogLvl::WARN)
					<< "Line: " + to_string(k) + " in " + fileName + " Error: "
							+ e.what();
		}
	}
	f.close();
}
void RuleMapper::parseString(const string &s) {
	MidiEventRule rule { s };
	rules.push_back(rule);
}

int RuleMapper::findMatchingRule(const MidiEvent &ev, int startPos) const {
	for (size_t i = startPos; i < getSize(); i++) {
		const MidiEventRule &oneRule = rules[i];
		if (oneRule.inEventRange.match(ev))
			return i;
	}
	return -1;
}

bool RuleMapper::applyRules(MidiEvent &ev) {
	// returns true if matching rule found
	bool changed = false;
	bool stop = false;
	MidiEvent ev1;

	for (size_t i = 0; i < getSize(); i++) {
		const MidiEventRule &oneRule = rules[i];
		const MidiEventRange &inEvent = oneRule.inEventRange;
		if (!inEvent.match(ev))
			continue;

		LOG(LogLvl::INFO) << "Found match for event: " << ev.toString()
				<< ", in rule: " << oneRule.toString();

		changed = true;
		switch (oneRule.rutype) {
		case MidiRuleType::STOP:
			oneRule.outEventRange.transform(ev);
			stop = true;
			break;
		case MidiRuleType::PASS:
			oneRule.outEventRange.transform(ev);
			break;
		case MidiRuleType::COUNT:
			ev1 = ev;
			oneRule.outEventRange.transform(ev1);
			count_event(ev1);
			stop = true;
			break;
		default:
			throw MidiAppError("Unknown rule type: " + oneRule.toString());
		}

		if (stop)
			break;
	}
	return changed;
}

const string RuleMapper::toString() const {
	ostringstream ss;
	for (size_t i = 0; i < getSize(); i++) {
		ss << "#" << i << '\t' << (rules[i]).toString() << endl;
	}
	return ss.str();
}

void RuleMapper::count_event(const MidiEvent &ev) {
	if (!similar_and_fast(ev)) {
		if (ev.isNoteOn()) {
			LOG(LogLvl::INFO) << "New note, reset count for: "
					<< ev.toString();
			prev_ev = ev;
			count_on = count_off = 0;
		}
	}

	if (ev.isNoteOn()) {
		if (count_off >= count_on) {
			count_on++;
			thread(&RuleMapper::send_event_delayed, this, ev, count_on).detach();
		}
	} else {
		if (count_off < count_on) {
			count_off++;
		}
	}
}

bool RuleMapper::similar_and_fast(const MidiEvent &ev) {
	// true if event is similar to latest note and came fast
	time_point now = the_clock::now();
	millis delta = std::chrono::duration_cast<millis>(now - prev_moment);
	prev_moment = now;
	return prev_ev.isEqual(ev) && delta < millis_600;
}

void RuleMapper::send_event_delayed(const MidiEvent &ev, int cnt_on) {
	std::this_thread::sleep_for(millis_600);
	if (count_on != cnt_on) {
		// new note came, count on changed, keep waiting
		LOG(LogLvl::DEBUG) << "Delayed check, count changed: "
				<< ev.toString() << ", on: " << count_on << ", off:"
				<< count_off << ", prev_on: " << cnt_on;
		return;
	}
	LOG(LogLvl::DEBUG) << "Delayed check, count NOT changed: "
			<< ev.toString() << ", on: " << count_on << ", off:" << count_off
			<< ", prev_on: " << cnt_on;

	midi_byte_t counted_v1 = ev.v1 + count_on + (count_on > count_off ? 5 : 0);
	MidiEvent e1 = ev;
	e1.v1 = counted_v1;
	count_on = count_off = 0;
	midi_client.send_new(e1);
}
