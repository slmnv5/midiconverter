#ifndef NOTECOUNTER_H
#define NOTECOUNTER_H
#include "pch.hpp"
#include "MidiEvent.hpp"

using namespace std;

class NoteCounter {
private:
	MidiEvent prev_cc;
	bool sent_on = false;
	count_map_t count_map;
public:
	NoteCounter() :
			count_map { } {
	}
	NoteCounter(const string &fileName);
	void parseString(const string &s);
	inline bool is_countable(MidiEvent &ev) {
		return count_map.count(ev.v1) > 0;
	}
	int convert_cc_note(const MidiEvent &ev);
	midi_byte_t convert_v1(midi_byte_t v1);
	inline size_t get_size() const {
		return count_map.size();
	}
};

#endif
