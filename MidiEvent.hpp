#ifndef MIDIEVENT_H
#define MIDIEVENT_H

#include "pch.hpp"

using namespace std;
typedef unsigned char midi_byte_t;
typedef std::map<midi_byte_t, midi_byte_t> count_map_t;
extern const midi_byte_t MIDI_MAX;
extern const midi_byte_t MIDI_MAXCH;

class MidiAppError: public std::exception {
private:
	string msg;
public:
	MidiAppError(const string &msg) noexcept :
			msg(msg) {
	}

	const char* what() const noexcept {
		return this->msg.c_str();
	}
};

class ValueRange {
public:
	midi_byte_t max_value, lower, upper;

	ValueRange(midi_byte_t max, midi_byte_t low, midi_byte_t up) :
			max_value(max), lower(low), upper(up) {
	}

	ValueRange() :
			ValueRange(MIDI_MAX, 0, MIDI_MAX) {
	}

	void init(const string &s);
	bool isValid() const;
	bool isInsideOf(ValueRange other) const {
		return lower >= other.lower && upper <= other.upper;
	}

	int size() const {
		return upper - lower + 1;
	}

	string toString() const {
		stringstream ss;
		ss << static_cast<int>(lower) << ":" << static_cast<int>(upper);
		return ss.str();
	}
};

class ChannelRange: public ValueRange {
public:
	ChannelRange() :
			ValueRange(MIDI_MAXCH, 0, MIDI_MAXCH) {
	}
};

//==================== enums ===================================

class MidiEvType {
public:
	constexpr static midi_byte_t ANYTHING = 'a';
	constexpr static midi_byte_t NOTEON = 'n'; // on
	constexpr static midi_byte_t NOTEOFF = 'o'; // stop
	constexpr static midi_byte_t CONTROLCHANGE = 'c';
	constexpr static midi_byte_t PROGCHANGE = 'p';
	static bool valid(midi_byte_t ch) {
		vector<midi_byte_t> valid_ch { 'a', 'n', 'o', 's', 'c', 'p' };
		return std::find(valid_ch.begin(), valid_ch.end(), ch) != valid_ch.end();
	}
};

//=============================================================

class MidiEvent {
public:
	MidiEvent() :
			evtype(MidiEvType::ANYTHING), ch(0), v1(0), v2(0) {
	}

	MidiEvent(const string&);
	midi_byte_t evtype;
	midi_byte_t ch; // MIDI channel
	midi_byte_t v1; // MIDI note or cc
	midi_byte_t v2; // MIDI velocity or cc value

	inline bool is_similar(const MidiEvent &other) const {
		return ch == other.ch && v1 == other.v1;
	}
	string toString() const {
		ostringstream ss(ios::binary);
		ss << evtype << "," << to_string(ch) << "," << to_string(v1) << ","
				<< to_string(v2);
		return ss.str();
	}
};

//============== free functions ==============================

void writeMidiEvent(snd_seq_event_t *event, const MidiEvent &ev);
bool readMidiEvent(const snd_seq_event_t *event, MidiEvent &ev);

//=============================================================

class MidiEventRange {
public:
	MidiEventRange() :
			evtype('a') {
	}
	MidiEventRange(const string &s, bool isOutEvent);
	string toString() const;
	bool match(const MidiEvent&) const;
	void transform(MidiEvent &ev) const;
	bool isValid() const;

	bool isOutEvent = false;
	midi_byte_t evtype;
	ChannelRange ch; // MIDI channel
	ValueRange v1; // MIDI note or cc
	ValueRange v2; // MIDI velocity or cc value

};

//=============================================================
class MidiEventRule {
public:
	MidiEventRule(const string &s);
	string toString() const;

	bool terminate = false;
	MidiEventRange inEventRange;
	MidiEventRange outEventRange;
};

#endif
