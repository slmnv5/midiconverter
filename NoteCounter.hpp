#ifndef NOTECOUNTER_H
#define NOTECOUNTER_H
#include "pch.hpp"
#include "MidiEvent.hpp"

using namespace std;

class NoteCounter {
private:
	MidiEvent last_cc;
	count_map_t count_map;
	bool convert_cc_note(MidiEvent &ev);
public:
	NoteCounter(const string &fileName);
	bool need_count(MidiEvent &ev);
	midi_byte_t convert_v1(midi_byte_t v1);
	int get_size() const {
		return count_map.size();
	}

};

#endif
