#include "pch.hpp"
#include "MidiEvent.hpp"
#include "utils.hpp"

using namespace std;
const midi_byte_t MIDI_MAX = 127;
const midi_byte_t MIDI_MAXCH = 15;
template<midi_byte_t max>
void MidiRange<max>::init(const string& s1) {
	string s(s1);
	remove_spaces(s);
	if (s.empty()) {
		lower = 0;
		upper = max_value;
		return;
	}

	vector<string> parts = split_string(s, ":");
	if (parts.size() == 1) {
		parts.push_back(parts[0]);
	}
	if (parts.size() != 2) {
		throw MidiAppError("ValueRange incorrect format: " + s);
	}

	try {
		lower = stoi(parts[0]);
		upper = stoi(parts[1]);
	}
	catch (exception& e) {
		throw MidiAppError("ValueRange incorrect values: " + s);
	}
}

//======================================

const std::string MidiEvent::all_types("ancp");
const std::string MidiEventRule::all_types("cpsko");

MidiEvent::MidiEvent(const string& s1) {
	string s(s1);
	remove_spaces(s);
	vector<string> parts = split_string(s, ",");

	if (parts.size() != 4) {
		throw MidiAppError("Not valid MidiEvent, must have 4 parts: " + s);
	}

	if (parts[0].size() != 1) {
		throw MidiAppError("MidiEvent, type must be single character: " + s);
	}

	try {
		evtype = static_cast<MidiEventType>(parts[0][0]);
		ch = stoi(parts[1]);
		v1 = stoi(parts[2]);
		v2 = stoi(parts[3]);
	}
	catch (exception& e) {
		throw MidiAppError("Not valid MidiEvent: " + string(e.what()), true);
	}
	if (!isValid())
		throw MidiAppError("Not valid MidiEvent: " + toString(), true);
}

//========================================================

MidiEventRange::MidiEventRange(const string& s1) {
	string s(s1);
	remove_spaces(s);
	vector<string> parts = split_string(s, ",");

	while (parts.size() != 4) {
		throw MidiAppError("MidiEventRange must have 4 parts: " + s, true);
	}

	if (!parts[0].empty())
		evtype = static_cast<MidiEventType>(parts[0][0]);

	ch = ChannelRange(parts[1]);
	v1 = ValueRange(parts[2]);
	v2 = ValueRange(parts[3]);
}

string MidiEventRange::toString() const {
	ostringstream ss;
	ss << static_cast<char>(evtype) << "," << ch.toString() << ","
		<< v1.toString() << "," << v2.toString();
	return ss.str();
}

bool InMidiEventRange::match(const MidiEvent& ev) const {
	return (evtype == ev.evtype || evtype == MidiEventType::ANYTHING)
		&& ch.match(ev.ch) && v1.match(ev.v1) && v2.match(ev.v2);
}

void OutMidiEventRange::transform(MidiEvent& ev) const {
	if (evtype != MidiEventType::ANYTHING)
		ev.evtype = evtype;
	ch.transform(ev.ch);
	v1.transform(ev.v1);
	v2.transform(ev.v2);
}

void InMidiEventRange::validate() const {
	if (ch.isValid() && v1.isValid() && v2.isValid())
		return;
	throw MidiAppError("Not valid MidiEventRange: " + this->toString(), true);
}

void OutMidiEventRange::validate() const {
	if (ch.isValidToTransform() && v1.isValidToTransform()
		&& v2.isValidToTransform())
		return;

	throw MidiAppError("Not valid MidiEventRange: " + this->toString(), true);
}

//===================================================

MidiEventRule::MidiEventRule(const string& s1) {
	string s(s1);
	remove_spaces(s);
	if (s.empty()) {
		throw MidiAppError("Rule string is empty");
	}
	vector<string> parts = split_string(s, "=");
	if (parts.size() < 2 || parts.size() > 3) {
		throw MidiAppError("Rule string must have 2 or 3 parts: " + s, true);
	}
	string tmp = parts[parts.size()-1];
	if (tmp.size() != 1) {
		throw MidiAppError("Rule type must be one character: " + s, true);
	}
	ruleType = static_cast<MidiRuleType>(tmp[0]);

	inEventRange = new InMidiEventRange(parts[0]);
	if (parts.size() == 3) {
		outEventRange = new OutMidiEventRange(parts[1]);
	} else {
		outEventRange = nullptr;
	}
	if (!isTypeValid()) {
		throw MidiAppError("Rule type is unknown: " + s, true);
	}
	if (ruleType == MidiRuleType::COUNT) {
		if (inEventRange->v2.lower >= 10)
			throw MidiAppError(
				"Count rule - input value range must be >= 10: " + s, true);

	}
}

std::string MidiEventRule::toString() const {
	ostringstream ss;
	ss << inEventRange->toString() << "=" << outEventRange->toString() << "="
		<< static_cast<char>(ruleType);
	return ss.str();
}
