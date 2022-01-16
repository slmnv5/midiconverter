#include "pch.hpp"

#include "MidiEvent.hpp"
#include "utils.hpp"

using namespace std;
const midi_byte_t MIDI_MAX = 127;
const midi_byte_t MIDI_MAXCH = 15;
//========== free functions ==================
void writeMidiEvent(snd_seq_event_t *event, const MidiEvent &ev) {

	if (ev.evtype == MidiEvType::NOTEOFF) {
		event->type = SND_SEQ_EVENT_NOTEOFF;
		event->data.note.channel = ev.ch;
		event->data.note.note = ev.v1;
		event->data.note.velocity = ev.v2;
		return;
	}

	if (ev.evtype == MidiEvType::NOTEON) {
		event->type = SND_SEQ_EVENT_NOTEON;
		event->data.note.channel = ev.ch;
		event->data.note.note = ev.v1;
		event->data.note.velocity = ev.v2;
		return;
	}

	else if (ev.evtype == MidiEvType::PROGCHANGE) {
		event->type = SND_SEQ_EVENT_PGMCHANGE;
		event->data.control.channel = ev.ch;
		event->data.control.value = ev.v1;
		return;
	}

	else if (ev.evtype == MidiEvType::CONTROLCHANGE) {
		event->type = SND_SEQ_EVENT_CONTROLLER;
		event->data.control.channel = ev.ch;
		event->data.control.param = ev.v1;
		event->data.control.value = ev.v2;
		return;
	}

}

bool readMidiEvent(const snd_seq_event_t *event, MidiEvent &ev) {
	if (event->type == SND_SEQ_EVENT_NOTEOFF) {
		ev.evtype = MidiEvType::NOTEOFF;
		ev.ch = event->data.note.channel;
		ev.v1 = event->data.note.note;
		ev.v2 = event->data.note.velocity;
		return true;
	}
	if (ev.evtype == SND_SEQ_EVENT_NOTEON) {
		ev.evtype = MidiEvType::NOTEON;
		ev.ch = event->data.note.channel;
		ev.v1 = event->data.note.note;
		ev.v2 = event->data.note.velocity;
		return true;
	}
	if (ev.evtype == SND_SEQ_EVENT_PGMCHANGE) {
		ev.evtype = MidiEvType::PROGCHANGE;
		ev.ch = event->data.control.channel;
		ev.v1 = event->data.control.value;
		return true;
	}
	if (ev.evtype == SND_SEQ_EVENT_CONTROLLER) {
		ev.evtype = MidiEvType::CONTROLCHANGE;
		ev.ch = event->data.control.channel;
		ev.v1 = event->data.control.param;
		ev.v2 = event->data.control.value;
		return true;
	}
	return false;

}

//========================================

void ValueRange::init(const string &s) {
	string s1 = s.substr(0, s.find(";"));
	replace_all(s1, " ", "");
	replace_all(s1, "\t", "");
	if (s1.empty()) {
		lower = 0;
		upper = max_value;
	} else {
		vector<string> parts = split_string(s1, ":");
		if (parts.size() == 1) {
			lower = stoi(parts[0]);
			upper = lower;
		} else if (parts.size() == 2) {
			lower = stoi(parts[0]);
			upper = stoi(parts[1]);
		} else {
			throw MidiAppError(string(__func__) + "  Incorrect format: " + s1);
		}
	}
	lower = lower % (max_value + 1);
	upper = upper % (max_value + 1);

}

bool ValueRange::isValid() const {
	return (lower >= 0 && lower <= max_value)
			&& (upper >= 0 && upper <= max_value);
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
		evtype = static_cast<midi_byte_t>(parts[0][0]);
		ch = stoi(parts[1]);
		v1 = stoi(parts[2]);
		v2 = stoi(parts[3]);
	} catch (exception &e) {
		throw MidiAppError("Not valid MidiEvent: " + string(e.what()));
	}

}

//========================================================

MidiEventRange::MidiEventRange(const string &s, bool out) :
		isOutEvent(out), evtype(MidiEvType::ANYTHING) {

	vector<string> parts = split_string(s, ",");

	while (parts.size() < 4) {
		parts.push_back("");
	}

	if (!parts[0].empty())
		evtype = static_cast<midi_byte_t>(parts[0][0]);

	ch.init(parts[1]);
	v1.init(parts[2]);
	v2.init(parts[3]);
	if (!this->isValid())
		throw MidiAppError("Not valid MidiEventRange: " + this->toString());
}

string MidiEventRange::toString() const {
	stringstream ss;
	ss << static_cast<char>(evtype) << "," << ch.toString() << ","
			<< v1.toString() << "," << v2.toString();
	return ss.str();
}

bool MidiEventRange::match(const MidiEvent &ev) const {
	if (isOutEvent)
		throw MidiAppError(string(__func__) + "  Not used for OUT event");

	return (evtype == ev.evtype || evtype == MidiEvType::ANYTHING)
			&& ev.ch >= ch.lower && ev.ch <= ch.upper && ev.v1 >= v1.lower
			&& ev.v1 <= v1.upper && ev.v2 >= v2.lower && ev.v2 <= v2.upper;
}

void MidiEventRange::transform(MidiEvent &ev) const {
	if (!isOutEvent)
		throw MidiAppError(string(__func__) + "  Not used for IN event");
	ev.evtype = evtype;
	if (ch.lower == ch.upper)
		ev.ch = ch.lower;
	if (v1.lower == v1.upper)
		ev.v1 = v1.lower;
	if (v2.lower == v2.upper)
		ev.v2 = v2.lower;
}

bool MidiEventRange::isValid() const {
	if (!isOutEvent) {
		return (ch.lower >= 0 && ch.upper <= MIDI_MAXCH)
				&& (v1.lower >= 0 && v1.upper <= MIDI_MAX)
				&& (v2.lower >= 0 && v2.upper <= MIDI_MAX);
	} else {
		return (ch.lower == ch.upper
				|| (ch.lower == 0 && ch.upper == MIDI_MAXCH))
				&& (v1.lower == v1.upper
						|| (v1.lower == 0 && v1.upper == MIDI_MAX))
				&& (v2.lower == v2.upper
						|| (v2.lower == 0 && v2.upper == MIDI_MAX));
	}
}

//===================================================

MidiEventRule::MidiEventRule(const string &s) {
	string s1 = s.substr(0, s.find(";"));
	terminate = (replace_all(s1, ">", "=") > 0);
	replace_all(s1, "\t", "");
	replace_all(s1, " ", "");

	vector<string> parts = split_string(s1, "=");
	if (parts.size() != 2) {
		throw MidiAppError("Rule string must have 2 parts: " + s1);
	}

	inEventRange = MidiEventRange(parts[0], false);
	outEventRange = MidiEventRange(parts[1], true);

}

string MidiEventRule::toString() const {
	stringstream ss;
	ss << inEventRange.toString() << (terminate ? ">" : "=")
			<< outEventRange.toString();
	return ss.str();
}
