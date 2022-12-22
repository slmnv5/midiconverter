#include "utils.hpp"

bool writeMidiEvent(snd_seq_event_t* event, const MidiEvent& ev) {
	// note OFF is note ON with zero velocity
	if (ev.isNote()) {
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

bool readMidiEvent(const snd_seq_event_t* event, MidiEvent& ev) {
	if (event->type == SND_SEQ_EVENT_NOTEOFF) {
		ev.evtype = MidiEventType::NOTE;
		ev.ch = event->data.note.channel;
		ev.v1 = event->data.note.note;
		ev.v2 = 0;
		return true;
	}
	if (event->type == SND_SEQ_EVENT_NOTEON) {
		ev.evtype = MidiEventType::NOTE;
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

std::vector<std::string> split_string(const std::string& s, const std::string& delimiter) {
	std::vector<std::string> tokens;
	auto start = 0U;
	auto stop = s.find(delimiter);
	while (stop != std::string::npos) {
		tokens.push_back(s.substr(start, stop - start));
		start = stop + delimiter.length();
		stop = s.find(delimiter, start);
	}
	tokens.push_back(s.substr(start, stop));
	return tokens;
}

int replace_all(std::string& s, const std::string& del, const std::string& repl) {
	std::string::size_type delsz = del.size();
	if (delsz == 0)
		return 0;
	int count = 0;
	std::string::size_type n = 0;
	while ((n = s.find(del, n)) != std::string::npos) {
		s.replace(n, delsz, repl);
		n += repl.size();
		count++;
	}
	return count;
}

void remove_spaces(std::string& s) {
	s = s.substr(0, s.find(";"));
	replace_all(s, " ", "");
	replace_all(s, "\n", "");
	replace_all(s, "\t", "");
}

std::string exec_command(const std::string& cmd) {
	char buffer[128];
	std::string result = "";
	FILE* pipe = popen(cmd.c_str(), "r");
	if (!pipe)
		throw std::runtime_error("popen() failed!");
	try {
		while (fgets(buffer, sizeof buffer, pipe) != NULL) {
			result += buffer;
		}
	}
	catch (std::exception& e) {
		pclose(pipe);
		throw e;
	}
	pclose(pipe);
	return result;
}


