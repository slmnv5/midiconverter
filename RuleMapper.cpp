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
			if (prev_orig_ev.isEqual(ev)) {
				LOG(LogLvl::DEBUG) << "Same orig. event ignored";
			} else {
				ev1 = ev;
				oneRule.outEventRange.transform(ev1);
				assert(ev1.isNote());
				update_count(ev1);
				thread(&RuleMapper::count_and_send, this, ev1, count_on,
						count_off).detach();
			}
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
void RuleMapper::update_count(const MidiEvent &ev) {
	// if we got another note number, restart count
	if (!prev_count_ev.isSimilar(ev)) {
		LOG(LogLvl::DEBUG) << "New count event, count reset: "
				<< ev.toString();
		count_on = count_off = 0;
		prev_count_ev = ev;
	}

	if (ev.isNoteOn()) {
		count_on++;
	} else if (count_on > 0) {
		count_off++;
	}
}

void RuleMapper::count_and_send(const MidiEvent &ev, int cnt_on, int cnt_off) {
	std::this_thread::sleep_for(millis_600);
	if (count_on != cnt_on || count_off != cnt_off
			|| !prev_count_ev.isSimilar(ev)) {
		// new note came, count changed, keep waiting, counting
		LOG(LogLvl::DEBUG) << "Delayed check, count changed: " << count_on
				<< "/" << count_off << " vs. " << cnt_on << "/" << cnt_off;
	} else {
		midi_byte_t counted_v1 = ev.v1 + count_on
				+ (count_on > count_off ? 5 : 0);
		MidiEvent e1 = ev;
		e1.v1 = counted_v1;
		count_on = count_off = 0;
		LOG(LogLvl::INFO)
				<< "Delayed check, count NOT changed, send counted note: "
				<< e1.toString();
		midi_client.send_new(e1);
	}
}

const string RuleMapper::toString() const {
	ostringstream ss;
	for (size_t i = 0; i < getSize(); i++) {
		ss << "#" << i << '\t' << (rules[i]).toString() << endl;
	}
	return ss.str();
}
