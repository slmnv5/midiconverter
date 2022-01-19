#include "pch.hpp"
#include "RuleMapper.hpp"
#include "log.hpp"

using namespace std;

RuleMapper::RuleMapper(const string &fileName) {
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

bool RuleMapper::applyRules(MidiEvent &ev) const {
// returns true if matching rule found
	bool changed = false;
	for (size_t i = 0; i < getSize(); i++) {
		const MidiEventRule &oneRule = rules[i];
		const MidiEventRange &inEvent = oneRule.inEventRange;
		if (!inEvent.match(ev))
			continue;

		LOG(LogLvl::DEBUG) << "Found match for event: " << ev.toString()
				<< ", in rule: " << oneRule.toString();

		changed = true;
		const MidiEventRange &outEvent = oneRule.outEventRange;
		outEvent.transform(ev);
		if (oneRule.terminate)
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
