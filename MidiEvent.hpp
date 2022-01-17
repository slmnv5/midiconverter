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
protected:
	void init(const string &s, midi_byte_t max_value);
public:
	midi_byte_t lower, upper;
	ValueRange(midi_byte_t lw, midi_byte_t up) :
			lower(lw), upper(up) {
		if (!isValid()) {
			throw MidiAppError(err_msg);
		}
	}

	ValueRange() :
			ValueRange(0, MIDI_MAX) {
	}

	ValueRange(const string &s) {
		init(s, MIDI_MAX);
		if (!isValid()) {
			throw MidiAppError(err_msg);
		}
	}

	bool isInsideOf(ValueRange other) const {
		return lower >= other.lower && upper <= other.upper;
	}

	string toString() const {
		std::ostringstream ss;
		ss << to_string(lower) << ":" << to_string(upper);
		return ss.str();
	}
	bool isValid() const {
		return (lower >= 0 && lower <= MIDI_MAX)
				&& (upper >= 0 && upper <= MIDI_MAX);
	}
private:
	string err_msg = "Not valid values for ValueRange";

};

class ChannelRange: public ValueRange {
public:
	ChannelRange() :
			ValueRange(0, MIDI_MAXCH) {
	}
	ChannelRange(midi_byte_t lw, midi_byte_t up) :
			ValueRange(lw, up) {
		if (!isValid()) {
			throw MidiAppError("err_msg");
		}
	}
	ChannelRange(const string &s) {
		init(s, MIDI_MAXCH);
		if (!isValid()) {
			throw MidiAppError(err_msg);
		}
	}
	bool isValid() const {
		return (lower >= 0 && lower <= MIDI_MAXCH)
				&& (upper >= 0 && upper <= MIDI_MAXCH);
	}
private:
	string err_msg = "Not valid values for ChannelRange";

};

//==================== enums ===================================

class MidiEvType {
public:
	constexpr static midi_byte_t ANYTHING = 'a';
	constexpr static midi_byte_t NOTEON = 'n';
	constexpr static midi_byte_t NOTEOFF = 'o';
	constexpr static midi_byte_t CONTROLCHANGE = 'c';
	constexpr static midi_byte_t PROGCHANGE = 'p';
	static bool isValid(midi_byte_t ch) {
		return string("anocp").find(ch) != string::npos;
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
		std::ostringstream ss;
		ss << static_cast<char>(evtype) << "," << to_string(ch) << ","
				<< to_string(v1) << "," << to_string(v2);
		return ss.str();
	}
	bool isValid() const;
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
