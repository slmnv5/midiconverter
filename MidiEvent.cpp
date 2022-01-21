#include "pch.hpp"

#include "MidiEvent.hpp"
#include "utils.hpp"
#include "log.hpp"

using namespace std;
const midi_byte_t MIDI_MAX = 127;
const midi_byte_t MIDI_MAXCH = 15;
//========== free functions ==================
bool writeMidiEvent(snd_seq_event_t *event, const MidiEvent &ev) {

	if (ev.evtype == MidiEventType::NOTEOFF) {
		event->type = SND_SEQ_EVENT_NOTEOFF;
		event->data.note.channel = ev.ch;
		event->data.note.note = ev.v1;
		event->data.note.velocity = ev.v2;
		return true;
	}

	if (ev.evtype == MidiEventType::NOTEON) {
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
		ev.evtype = MidiEventType::NOTEOFF;
		ev.ch = event->data.note.channel;
		ev.v1 = event->data.note.note;
		ev.v2 = event->data.note.velocity;
		return true;
	}
	if (event->type == SND_SEQ_EVENT_NOTEON) {
		ev.evtype = MidiEventType::NOTEON;
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

void ValueRange::init(const string &s, midi_byte_t max_value) {
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
		throw MidiAppError("Not valid MidiEvent: " + string(e.what()));
	}
	if (!isValid())
		throw MidiAppError("Not valid MidiEvent: " + toString());

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
	if (!this->isValid())
		throw MidiAppError("Not valid MidiEventRange: " + this->toString());
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
			&& ev.ch >= ch.lower && ev.ch <= ch.upper && ev.v1 >= v1.lower
			&& ev.v1 <= v1.upper && ev.v2 >= v2.lower && ev.v2 <= v2.upper;
}

void MidiEventRange::transform(MidiEvent &ev) const {
	if (!isOut)
		throw MidiAppError("Transform used for IN range");
	ev.evtype = evtype;
	if (ch.lower == ch.upper)
		ev.ch = ch.lower;
	if (v1.lower == v1.upper)
		ev.v1 = v1.lower;
	if (v2.lower == v2.upper)
		ev.v2 = v2.lower;
}

bool MidiEventRange::isValid() const {
	bool ok = ch.isValid() && v1.isValid() && v2.isValid();
	if (!isOut) {
		return ok;
	} else {
		return (ch.lower == ch.upper) && (v1.lower == v1.upper)
				&& (v2.lower == v2.upper) && ok;
	}
}

//===================================================

MidiEventRule::MidiEventRule(const string &s) {
	string s1(s);
	remove_spaces(s1);
	vector<string> parts = split_string(s1, "=");
	if (parts.size() != 3) {
		throw MidiAppError("Rule string must have 3 parts: " + s1);
	}
	if (parts[2].size() != 1) {
		throw MidiAppError("Rule type must be one character: " + s1);
	}

	inEventRange = MidiEventRange(parts[0], false);
	outEventRange = MidiEventRange(parts[1], true);
	rutype = static_cast<MidiRuleType>(parts[2][0]);
}

string MidiEventRule::toString() const {
	ostringstream ss;
	ss << inEventRange.toString() << "=" << outEventRange.toString()
			<< static_cast<char>(rutype);
	return ss.str();
}
