#ifndef UTILS_H
#define UTILS_H

#include "pch.hpp"
#include "MidiEvent.hpp"

bool writeMidiEvent(snd_seq_event_t* event, const MidiEvent& ev);
bool readMidiEvent(const snd_seq_event_t* event, MidiEvent& ev);

std::vector<std::string> split_string(const std::string& s,
	const std::string& delimiter);
int replace_all(std::string& s, const std::string& del,
	const std::string& repl);
void remove_spaces(std::string& s);
std::string exec_command(const std::string& cmd);



#endif
