#ifndef RULEMAPPER_H
#define RULEMAPPER_H
#include "pch.hpp"
#include "MidiEvent.hpp"
#include "log.hpp"
using namespace std;

class RuleMapper {
public:
	RuleMapper() :
			rules { } {
	}
	RuleMapper(const string &fileName);

	int findMatchingRule(const MidiEvent&, int startPos = 0) const;
	void parseString(const string&);
	bool applyRules(MidiEvent &ev) const;
	MidiEventRule& getRule(int i) {
		return rules[i];
	}
	size_t getSize() const {
		return rules.size();
	}
	const string toString() const;

private:
	vector<MidiEventRule> rules;

};

#endif
