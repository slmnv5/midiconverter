#ifndef MIDIEVENT_H
#define MIDIEVENT_H

#include "pch.hpp"


typedef unsigned char midi_byte_t;
typedef std::map<midi_byte_t, midi_byte_t> count_map_t;
extern const midi_byte_t MIDI_MAX;
extern const midi_byte_t MIDI_MAXCH;

//=============================================================
class MidiAppError : public std::exception {
private:
	const std::string msg;
	const bool critical;
public:
	MidiAppError(const std::string& msg, bool crt = false) noexcept :
		msg(msg), critical(crt) {
	}

	bool is_critical() const {
		return critical;
	}
	const char* what() const noexcept {
		return this->msg.c_str();
	}
};
//=============================================================
template<midi_byte_t max>
class MidiRange {
protected:
	void init(const std::string&);

public:
	static const midi_byte_t max_value = max;
	midi_byte_t lower, upper;

	MidiRange() {
		lower = 0;
		upper = max_value;
	}

	MidiRange(const std::string& s) {
		init(s);
		if (!isValid()) {
			throw MidiAppError(err_msg);
		}
	}

	std::string toString() const {
		std::ostringstream ss;
		ss << std::to_string(lower) << ":" << std::to_string(upper);
		return ss.str();
	}
	inline bool isValid() const {
		return (lower >= 0 && lower <= max_value)
			&& (upper >= 0 && upper <= max_value);
	}
	inline bool isValidToTransform() const {
		return (lower == 0 && upper == max_value)
			|| (lower == upper && (lower >= 0 && lower <= max_value));
	}
	inline bool match(midi_byte_t v) const {
		return lower <= v && v <= upper;
	}
	inline void transform(midi_byte_t& v) const {
		v = lower == upper ? lower : v;
	}

private:
	std::string err_msg = "Not valid values, must be in range: 0-"
		+ std::to_string(max_value);
};

using ValueRange = MidiRange<127>;
using ChannelRange = MidiRange<15>;

//==================== enums ===================================

enum class MidiEventType : midi_byte_t {
	ANYTHING = 'a', NOTE = 'n', CONTROLCHANGE = 'c', PROGCHANGE = 'p'
};

//=============================================================

class MidiEvent {
	const static std::string all_types;
public:
	MidiEvent() :
		evtype(MidiEventType::ANYTHING), ch(0), v1(0), v2(0) {
	}

	MidiEvent(const std::string&);

	MidiEventType evtype;
	midi_byte_t ch; // MIDI channel
	midi_byte_t v1; // MIDI note or cc
	midi_byte_t v2; // MIDI velocity or cc value

	std::string toString() const {
		std::ostringstream ss;
		ss << static_cast<char>(evtype) << "," << std::to_string(ch) << ","
			<< std::to_string(v1) << "," << std::to_string(v2);
		return ss.str();
	}
	inline bool isEqual(const MidiEvent& ev) const {
		return evtype == ev.evtype && ch == ev.ch && v1 == ev.v1 && v2 == ev.v2;
	}
	inline bool isSimilar(const MidiEvent& ev) const {
		return evtype == ev.evtype && ch == ev.ch && v1 == ev.v1;
	}
	inline char typeToChar() const {
		return static_cast<char>(evtype);
	}
	inline bool isTypeValid() const {
		return MidiEvent::all_types.find(typeToChar()) != std::string::npos;
	}
	inline bool isValid() const {
		return isTypeValid() && (ch >= 0 && ch <= MIDI_MAXCH)
			&& (v1 >= 0 && v1 <= MIDI_MAX) && (v2 >= 0 && v2 <= MIDI_MAX);
	}
	inline bool isNote() const {
		return evtype == MidiEventType::NOTE;
	}
	inline bool isNoteOn() const {
		return evtype == MidiEventType::NOTE && v2 > 0;
	}
	inline bool isNoteOff() const {
		return evtype == MidiEventType::NOTE && v2 == 0;
	}
	inline bool isCc() const {
		return evtype == MidiEventType::CONTROLCHANGE;
	}
	inline bool isPc() const {
		return evtype == MidiEventType::PROGCHANGE;
	}
};


class MidiEventRange {
protected:
	MidiEventRange(const std::string& s);
public:
	std::string toString() const;

	MidiEventType evtype;
	ChannelRange ch; // MIDI channel
	ValueRange v1;	 // MIDI note or cc
	ValueRange v2;	 // MIDI velocity or cc value
};

class InMidiEventRange : public MidiEventRange {
public:
	InMidiEventRange(const std::string& s) : MidiEventRange(s) { validate(); }
	bool match(const MidiEvent&) const;
	void validate() const;
};

class OutMidiEventRange : public MidiEventRange {
public:
	OutMidiEventRange(const std::string& s) : MidiEventRange(s) { validate(); }
	void transform(MidiEvent& ev) const;
	void validate() const;
};
//=============================================================
enum class MidiRuleType : midi_byte_t {
	PASS = 'p', STOP = 's', COUNT = 'c', ONCE = 'o'
};

class MidiEventRule {
	const static std::string all_types;
public:
	MidiEventRule(const std::string&);
	std::string toString() const;

	inline char typeToChar() const {
		return static_cast<char>(ruleType);
	}
	inline bool isTypeValid() const {
		return MidiEventRule::all_types.find(typeToChar()) != std::string::npos;
	}
	InMidiEventRange* inEventRange;
	OutMidiEventRange* outEventRange;
	MidiRuleType ruleType;
};

#endif
