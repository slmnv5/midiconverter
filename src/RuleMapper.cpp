#include "pch.hpp"
#include "MidiEvent.hpp"
#include "RuleMapper.hpp"
#include "MidiClient.hpp"
#include "utils.hpp"
#include "log.hpp"

using namespace std;

const int RuleMapper::sleep_ms = 600;

RuleMapper::RuleMapper(const string& fileName, const MidiClient& mc) :
	midi_client(mc) {
	ifstream f(fileName);
	string s;
	int k = 0;
	while (getline(f, s)) {
		try {
			k++;
			rules.push_back(MidiEventRule(s));
		}
		catch (MidiAppError& e) {
			LogLvl level = e.is_critical() ? LogLvl::ERROR : LogLvl::WARN;
			LOG(level)
				<< "Line: " << k << " in " << fileName << " Error: "
				<< e.what();

		}
		catch (exception& e) {
			LOG(LogLvl::ERROR)
				<< "Line: " << k << " in " << fileName << " Error: "
				<< e.what();
		}
	}
	f.close();
}

void RuleMapper::parseString(const string& s1) {
	string s(s1);
	remove_spaces(s);
	if (!s.empty())
		rules.push_back(MidiEventRule(s));
}

int RuleMapper::findMatchingRule(const MidiEvent& ev, int startPos) const {
	for (size_t i = startPos; i < getSize(); i++) {
		const MidiEventRule& oneRule = rules[i];
		if (oneRule.inEventRange.match(ev))
			return i;
	}
	return -1;
}

bool RuleMapper::applyRules(MidiEvent& ev) {
	// returns true if matching rule found
	bool is_found = false;
	for (size_t i = 0; i < getSize(); i++) {
		const MidiEventRule& oneRule = rules[i];
		const MidiEventRange& inEvent = oneRule.inEventRange;
		is_found = inEvent.match(ev);
		if (!is_found)
			continue;

		LOG(LogLvl::DEBUG) << "Found match for event: " << ev.toString()
			<< ", in rule: " << oneRule.toString();
		if (oneRule.ruleType == MidiRuleType::KILL) {
			LOG(LogLvl::DEBUG) << "Rule type KILL got event: " << ev.toString();
			prev_ev = ev;
			return  false;
		}
		else if (oneRule.ruleType == MidiRuleType::ONCE) {
			if (ev.isEqual(prev_ev)) {
				LOG(LogLvl::DEBUG) << "Rule type ONCE ignores the same event: " << ev.toString();
				return  false;
			}
			LOG(LogLvl::DEBUG) << "Rule type ONCE executed for event: " << ev.toString();
			prev_ev = ev;
			oneRule.outEventRange.transform(ev);
			continue;
		}
		else if (oneRule.ruleType == MidiRuleType::STOP) {
			LOG(LogLvl::DEBUG) << "Rule STOP executed for event: " << ev.toString();
			prev_ev = ev;
			oneRule.outEventRange.transform(ev);
			return true;
		}
		else if (oneRule.ruleType == MidiRuleType::PASS) {
			LOG(LogLvl::DEBUG) << "Rule PASS executed for event: " << ev.toString();
			prev_ev = ev;
			oneRule.outEventRange.transform(ev);
			continue;
		}
		else if (oneRule.ruleType == MidiRuleType::COUNT) {
			LOG(LogLvl::DEBUG) << "Rule COUNT executed for event: " << ev.toString();
			prev_ev = ev;
			MidiEvent ev_count = ev;
			oneRule.outEventRange.transform(ev_count);
			update_count(ev_count);
			bool send_it = count_on == 1 && count_off == 0; // send only 1-st ON for original ev
			if (ev.isNoteOn()) {
				thread(&RuleMapper::count_and_send, this, ev_count, count_on).detach();
			}
			return send_it;
		}
		else {
			throw MidiAppError("Unknown rule type: " + oneRule.toString());
		}
	}
	return is_found;
}
void RuleMapper::update_count(const MidiEvent& ev) {
	// if we got another note number, restart count
	if (!prev_count_ev.isSimilar(ev)) {
		LOG(LogLvl::DEBUG) << "New count event, count reset: "
			<< ev.toString();
		count_on = count_off = 0;
		prev_count_ev = ev;
	}

	if (ev.isNoteOn()) {
		count_on++;
	}
	else if (count_on > 0) {
		count_off++;
	}
}

void RuleMapper::count_and_send(const MidiEvent& ev, int cnt_on) {
	std::this_thread::sleep_for(
		std::chrono::milliseconds(RuleMapper::sleep_ms));
	if (count_on != cnt_on) {
		LOG(LogLvl::DEBUG) << "Delayed check, count changed: " << count_on
			<< " vs. " << cnt_on;
	}
	else if (!prev_count_ev.isSimilar(ev)) {
		LOG(LogLvl::DEBUG) << "Delayed check, new note: " << ev.toString()
			<< " vs. " << prev_count_ev.toString();
	}
	else {
		midi_byte_t counted_v1 = ev.v1 + count_on
			+ (count_on > count_off ? 5 : 0);
		if (counted_v1 > ValueRange::max_value) {
			counted_v1 %= ValueRange::max_value;
		}
		MidiEvent e1 = ev;
		e1.v1 = counted_v1;
		count_on = count_off = 0;
		LOG(LogLvl::INFO) << "Delayed check, send counted note: "
			<< e1.toString();
		try {
			midi_client.make_and_send(nullptr, e1);
		}
		catch (exception& e) {
			LOG(LogLvl::ERROR) << "Thread to cont events has error: " << e.what();
		}
	}
}

string RuleMapper::toString() const {
	ostringstream ss;
	for (size_t i = 0; i < getSize(); i++) {
		ss << "#" << i << '\t' << rules[i].toString() << endl;
	}
	return ss.str();
}
