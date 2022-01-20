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
			if (parts.size() != 2) {
				throw MidiAppError("String must have 2 parts: " + s);
			}

			key = std::atoi(parts[0].c_str());
			value = std::atoi(parts[1].c_str());
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

int NoteCounter::convert_cc_note(const MidiEvent &ev) {
	int result = 0;
	if (!ev.is_similar(prev_cc)) {
		sent_on = false;
		result = 0;
	} else if (prev_cc.v2 > ev.v2 and !sent_on) {
		sent_on = true;
		result = 1;
	} else if (prev_cc.v2 < ev.v2 and sent_on) {
		sent_on = false;
		result = -1;
	} else {
		result = 0;
	}
	prev_cc = ev;
	return result;
}

midi_byte_t NoteCounter::convert_v1(midi_byte_t v1) {
	return count_map.at(v1);
}
