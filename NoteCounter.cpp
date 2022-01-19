#include "pch.hpp"
#include "NoteCounter.hpp"
#include "utils.hpp"
#include "log.hpp"

using namespace std;

NoteCounter::NoteCounter(const string &fileName) {
	ifstream f(fileName);
	string s;
	int k = 0;
	midi_byte_t key, value;
	while (getline(f, s)) {
		try {
			k++;
			remove_spaces(s);
			if (s.empty())
				continue;
			vector<string> parts = split_string(s, "-");
			key = stoi(parts[0]);
			value = stoi(parts[1]);
			count_map[key] = value;
		} catch (exception &e) {
			LOG(LogLvl::ERROR)
					<< "Line: " + to_string(k) + " in " + fileName + " Error: "
							+ e.what();
		}
	}
	f.close();
}

void NoteCounter::parseString(const string &s) {
	string s1 = s.substr(0, s.find(';'));
	remove_spaces(s1);
	vector<string> parts = split_string(s, "-");
	if (parts.size() != 2)
		throw MidiAppError("NoteCounter string must have 2 parts: " + s1);
	midi_byte_t key = stoi(parts[0]);
	midi_byte_t value = stoi(parts[1]);
	count_map[key] = value;
}

bool NoteCounter::is_countable_note(MidiEvent &ev) {
	bool is_cc = ev.evtype == MidiEvType::CONTROLCHANGE;
	bool is_on = ev.evtype == MidiEvType::NOTEON;
	bool is_off = ev.evtype == MidiEvType::NOTEOFF;
	bool is_counted = count_map.count(ev.v1) > 0;
	bool type_ok = is_cc || is_on || is_off;
	if (type_ok && is_counted) {
		if (is_cc) {
			LOG(LogLvl::DEBUG) << "Trying to convert CC to note: "
					<< ev.toString();
			return convert_cc_note(ev);
		} else {
			return true;
		}
	} else
		return false;
}

bool NoteCounter::convert_cc_note(MidiEvent &ev) {
	if (ev.is_similar(last_cc)) {
		last_cc = ev;
		LOG(LogLvl::DEBUG) << "Creating note from event: " << ev.toString();
		if (last_cc.v2 < ev.v2) {
			ev.evtype = MidiEvType::NOTEON;
			ev.v2 = 101;
		} else {
			ev.evtype = MidiEvType::NOTEOFF;
			ev.v2 = 0;
		}
		return true;
	}

	last_cc = ev;
	return false;
}

midi_byte_t NoteCounter::convert_v1(midi_byte_t v1) {
	return count_map.at(v1);
}
