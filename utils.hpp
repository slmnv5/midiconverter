#ifndef UTILS_H
#define UTILS_H

#include "pch.hpp"
#include <alsa/asoundlib.h>
#include "MidiEvent.hpp"
#include <chrono>

using namespace std;

//==================== utility functions ===================================

vector<string> split_string(const string &s, const string &delimiter);

int replace_all(string &s, const string &del, const string &repl);

string exec_command(const string &cmd);

#endif
