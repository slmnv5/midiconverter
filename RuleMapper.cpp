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
		} catch (MidiAppError &e) {
			LogLvl level = e.is_critical() ? LogLvl::ERROR : LogLvl::WARN;
			LOG(level)
					<< "Line: " + to_string(k) + " in " + fileName + " Error: "
							+ e.what();

		} catch (exception &e) {
			LOG(LogLvl::ERROR)
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
			assert(ev1.isNote());
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
	bool is_on = ev.isNoteOn();
	time_point now_moment = the_clock::now();
	bool is_similar = prev_ev.isSimilar(ev);
	bool is_fast = now_moment - prev_moment < millis_600;

	prev_ev = ev;
	prev_moment = now_moment;
	if (!is_similar || !is_fast) {
		LOG(LogLvl::INFO) << "New count start for: " << ev.toString();
		count_on = 0;
		prev_on = false;
	}
	prev_on = is_on;
	if (is_on) {
		count_on++;
		thread(&RuleMapper::send_event_delayed, this, ev, count_on).detach();
	}
}

void RuleMapper::send_event_delayed(const MidiEvent &ev, int cnt_on) {
	std::this_thread::sleep_for(millis_600);
	if (count_on != cnt_on) {
		// new note came, count on changed, return i.e. keep waiting and counting
		LOG(LogLvl::DEBUG) << "Delayed check, count changed: "
				<< ev.toString() << count_on << "/" << cnt_on;
	} else {
		LOG(LogLvl::DEBUG) << "Delayed check, count NOT changed: "
				<< ev.toString() << count_on << "/" << cnt_on;

		midi_byte_t counted_v1 = ev.v1 + count_on + prev_on ? 5 : 0;
		MidiEvent e1 = ev;
		e1.v1 = counted_v1;
		count_on = 0;
		prev_on = false;
		midi_client.send_new(e1);
	}
}
