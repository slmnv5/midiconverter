#ifndef MIDIEVENT_H
#define MIDIEVENT_H

#include "pch.hpp"

using namespace std;
typedef unsigned char midi_byte_t;
typedef std::map<midi_byte_t, midi_byte_t> count_map_t;
extern const midi_byte_t MIDI_MAX;
extern const midi_byte_t MIDI_MAXCH;

//=============================================================
class MidiAppError : public std::exception
{
private:
	string msg;

public:
	MidiAppError(const string &msg) noexcept : msg(msg)
	{
	}

	const char *what() const noexcept
	{
		return this->msg.c_str();
	}
};
//=============================================================
class ValueRange
{
protected:
	void init(const string &s, midi_byte_t max_value);

public:
	midi_byte_t lower, upper;
	ValueRange(midi_byte_t lw, midi_byte_t up) : lower(lw), upper(up)
	{
		if (!isValid())
		{
			throw MidiAppError(err_msg);
		}
	}

	ValueRange() : ValueRange(0, MIDI_MAX)
	{
	}

	ValueRange(const string &s)
	{
		init(s, MIDI_MAX);
		if (!isValid())
		{
			throw MidiAppError(err_msg);
		}
	}

	string toString() const
	{
		std::ostringstream ss;
		ss << to_string(lower) << ":" << to_string(upper);
		return ss.str();
	}
	bool isValid() const
	{
		return (lower >= 0 && lower <= MIDI_MAX) && (upper >= 0 && upper <= MIDI_MAX);
	}

private:
	string err_msg = "Not valid values for ValueRange";
};
//=============================================================
class ChannelRange : public ValueRange
{
public:
	ChannelRange() : ValueRange(0, MIDI_MAXCH)
	{
	}
	ChannelRange(midi_byte_t lw, midi_byte_t up) : ValueRange(lw, up)
	{
		if (!isValid())
		{
			throw MidiAppError("err_msg");
		}
	}
	ChannelRange(const string &s)
	{
		init(s, MIDI_MAXCH);
		if (!isValid())
		{
			throw MidiAppError(err_msg);
		}
	}
	bool isValid() const
	{
		return (lower >= 0 && lower <= MIDI_MAXCH) && (upper >= 0 && upper <= MIDI_MAXCH);
	}

private:
	string err_msg = "Not valid values for ChannelRange";
};

//==================== enums ===================================

enum class MidiEventType : midi_byte_t
{
	ANYTHING = 'a',
	NOTEON = 'n',
	NOTEOFF = 'o',
	CONTROLCHANGE = 'c',
	PROGCHANGE = 'p'
};

//=============================================================

class MidiEvent
{
public:
	MidiEvent() : evtype(MidiEventType::ANYTHING), ch(0), v1(0), v2(0)
	{
	}
	MidiEvent(MidiEventType evtp, midi_byte_t chan, midi_byte_t val1,
			  midi_byte_t val2) : evtype(evtp), ch(chan), v1(val1), v2(val2)
	{
		if (!isValid())
			throw MidiAppError("Not valid MidiEvent: " + toString());
	}

	MidiEvent(const string &);

	MidiEventType evtype;
	midi_byte_t ch; // MIDI channel
	midi_byte_t v1; // MIDI note or cc
	midi_byte_t v2; // MIDI velocity or cc value

	string toString() const
	{
		std::ostringstream ss;
		ss << static_cast<char>(evtype) << "," << to_string(ch) << ","
		   << to_string(v1) << "," << to_string(v2);
		return ss.str();
	}
	inline bool isEqual(const MidiEvent &other) const
	{
		return evtype == other.evtype && ch == other.ch && v1 == other.v1 && v2 == other.v2;
	}
	inline char typeToChar() const
	{
		return static_cast<char>(evtype);
	}
	inline bool isTypeValid() const
	{
		static const std::string s("anocp");
		return (s.find(typeToChar()) != std::string::npos);
	}
	inline bool isValid() const
	{
		return isTypeValid() && (ch >= 0 && ch <= MIDI_MAXCH) && (v1 >= 0 && v1 <= MIDI_MAX) && (v2 >= 0 && v2 <= MIDI_MAX);
	}
	inline bool isNote() const
	{
		return isNoteOn() || isNoteOff();
	}
	inline bool isNoteOn() const
	{
		return evtype == MidiEventType::NOTEON;
	}
	inline bool isNoteOff() const
	{
		return evtype == MidiEventType::NOTEOFF;
	}
	inline bool isCc() const
	{
		return evtype == MidiEventType::CONTROLCHANGE;
	}
	inline bool isPc() const
	{
		return evtype == MidiEventType::PROGCHANGE;
	}
};
//============== free functions ==============================
bool writeMidiEvent(snd_seq_event_t *event, const MidiEvent &ev);
bool readMidiEvent(const snd_seq_event_t *event, MidiEvent &ev);
//=============================================================

class MidiEventRange
{
public:
	MidiEventRange() : evtype(MidiEventType::ANYTHING)
	{
	}
	MidiEventRange(const string &s, bool isOutEvent);
	string toString() const;
	bool match(const MidiEvent &) const;
	void transform(MidiEvent &ev) const;
	bool isValid() const;

	bool isOut = false;
	MidiEventType evtype;
	ChannelRange ch; // MIDI channel
	ValueRange v1;	 // MIDI note or cc
	ValueRange v2;	 // MIDI velocity or cc value
};

//=============================================================
enum class MidiRuleType : midi_byte_t
{
	PASS = 'p',
	STOP = 's',
	COUNT = 'c'
};

class MidiEventRule
{
public:
	MidiEventRule(const string &s);
	string toString() const;

	bool count = false;
	MidiEventRange inEventRange;
	MidiEventRange outEventRange;
	MidiRuleType rutype;
};

#endif
