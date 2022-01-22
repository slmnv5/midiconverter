#include "pch.hpp"
#include "MidiEvent.hpp"
#include "log.hpp"
#include "utils.hpp"

using namespace std;
const midi_byte_t MIDI_MAX = 127;
const midi_byte_t MIDI_MAXCH = 15;
//========== free functions ==================
bool writeMidiEvent(snd_seq_event_t *event, const MidiEvent &ev) {
	// note OFF is note ON with zero velocity
	if (ev.isNote()) {
		event->type = SND_SEQ_EVENT_NOTEON;
		event->data.note.channel = ev.ch;
		event->data.note.note = ev.v1;
		event->data.note.velocity = ev.v2;
		return true;
	}

	else if (ev.evtype == MidiEventType::PROGCHANGE) {
		event->type = SND_SEQ_EVENT_PGMCHANGE;
		event->data.control.channel = ev.ch;
		event->data.control.value = ev.v1;
		return true;
	}

	else if (ev.evtype == MidiEventType::CONTROLCHANGE) {
		event->type = SND_SEQ_EVENT_CONTROLLER;
		event->data.control.channel = ev.ch;
		event->data.control.param = ev.v1;
		event->data.control.value = ev.v2;
		return true;
	}
	return false;
}

bool readMidiEvent(const snd_seq_event_t *event, MidiEvent &ev) {
	if (event->type == SND_SEQ_EVENT_NOTEOFF) {
		ev.evtype = MidiEventType::NOTE;
		ev.ch = event->data.note.channel;
		ev.v1 = event->data.note.note;
		ev.v2 = 0;
		return true;
	}
	if (event->type == SND_SEQ_EVENT_NOTEON) {
		ev.evtype = MidiEventType::NOTE;
		ev.ch = event->data.note.channel;
		ev.v1 = event->data.note.note;
		ev.v2 = event->data.note.velocity;
		return true;
	}
	if (event->type == SND_SEQ_EVENT_PGMCHANGE) {
		ev.evtype = MidiEventType::PROGCHANGE;
		ev.ch = event->data.control.channel;
		ev.v1 = event->data.control.value;
		return true;
	}
	if (event->type == SND_SEQ_EVENT_CONTROLLER) {
		ev.evtype = MidiEventType::CONTROLCHANGE;
		ev.ch = event->data.control.channel;
		ev.v1 = event->data.control.param;
		ev.v2 = event->data.control.value;
		return true;
	}
	return false;
}

//========================================
template<midi_byte_t max>
void MidiRange<max>::init(const string &s) {
	string s1(s);
	remove_spaces(s1);
	if (s1.empty()) {
		lower = 0;
		upper = max_value;
		return;
	}

	vector<string> parts = split_string(s1, ":");
	if (parts.size() == 1) {
		parts.push_back(parts[0]);
	}
	if (parts.size() != 2) {
		throw MidiAppError("ValueRange incorrect format: " + s1);
	}

	try {
		lower = stoi(parts[0]);
		upper = stoi(parts[1]);
	} catch (exception &e) {
		throw MidiAppError("ValueRange incorrect values: " + s1);
	}
}

//======================================

const std::string MidiEvent::all_types("anofcp");
const std::string MidiEventRule::all_types("cps");

MidiEvent::MidiEvent(const string &s1) {
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
	} catch (exception &e) {
		throw MidiAppError("Not valid MidiEvent: " + string(e.what()), true);
	}
	if (!isValid())
		throw MidiAppError("Not valid MidiEvent: " + toString(), true);
}

//========================================================

MidiEventRange::MidiEventRange(const string &s, bool out) :
		isOut(out), evtype(MidiEventType::ANYTHING) {

	vector<string> parts = split_string(s, ",");

	while (parts.size() < 4) {
		parts.push_back("");
	}

	if (!parts[0].empty())
		evtype = static_cast<MidiEventType>(parts[0][0]);

	ch = ChannelRange(parts[1]);
	v1 = ValueRange(parts[2]);
	v2 = ValueRange(parts[3]);
	if (!isValid())
		throw MidiAppError("Not valid MidiEventRange: " + toString(), true);
}

string MidiEventRange::toString() const {
	ostringstream ss;
	ss << static_cast<char>(evtype) << "," << ch.toString() << ","
			<< v1.toString() << "," << v2.toString();
	return ss.str();
}

bool MidiEventRange::match(const MidiEvent &ev) const {
	if (isOut)
		throw MidiAppError("Match used for OUT range");

	return (evtype == ev.evtype || evtype == MidiEventType::ANYTHING)
			&& ch.match(ev.ch) && v1.match(ev.v1) && v2.match(ev.v2);
}

void MidiEventRange::transform(MidiEvent &ev) const {
	if (!isOut)
		throw MidiAppError("Transform used for IN range");
	if (evtype != MidiEventType::ANYTHING)
		ev.evtype = evtype;
	ch.transform(ev.ch);
	v1.transform(ev.v1);
	v2.transform(ev.v2);
}

bool MidiEventRange::isValid() const {
	if (!isOut)

	{
		return ch.isValid() && v1.isValid() && v2.isValid();
	} else {
		return ch.isValidToTransform() && v1.isValidToTransform()
				&& v2.isValidToTransform();
	}
}

//===================================================

MidiEventRule::MidiEventRule(const string &s) {
	string s1(s);

	remove_spaces(s1);
	if (s1.empty()) {
		throw MidiAppError("Empty rule was ignored: " + s);
	}
	vector<string> parts = split_string(s1, "=");
	if (parts.size() != 3) {
		throw MidiAppError("Rule string must have 3 parts: " + s1, true);
	}
	if (parts[2].size() != 1) {
		throw MidiAppError("Rule type must be one character: " + s1, true);
	}

	inEventRange = MidiEventRange(parts[0], false);
	outEventRange = MidiEventRange(parts[1], true);
	rutype = static_cast<MidiRuleType>(parts[2][0]);
	if (!isTypeValid()) {
		throw MidiAppError("Rule type is unknown: " + s1, true);
	}
	if (rutype == MidiRuleType::COUNT) {
		outEventRange.evtype = MidiEventType::NOTE;
		inEventRange.evtype = MidiEventType::NOTE;
		inEventRange.v2 = ValueRange();
	}
}

string MidiEventRule::toString() const {
	ostringstream ss;
	ss << inEventRange.toString() << "=" << outEventRange.toString() << "="
			<< static_cast<char>(rutype);
	return ss.str();
}
