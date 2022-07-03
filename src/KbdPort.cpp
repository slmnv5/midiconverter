




#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <string.h>
#include "pch.hpp"
#include "utils.hpp"
#include "KbdPort.hpp"
#include "MidiEvent.hpp"


KbdPort::KbdPort(const std::string& kbdName) {
        fd = open(kbdName.c_str(), O_RDONLY);
        if (fd == -1) {
            throw MidiAppError("Cannot open keyboard device: " + kbdName);
        }
    }


KbdPort::parseString(const string& s1) {
    string s(s1);

    remove_spaces(s);
    if (s.empty()) {
        throw MidiAppError("Keyboard mapping was ignored: " + s);
    }
    vector<string> parts = split_string(s, "=");
    if (parts.size() != 2) {
        throw MidiAppError("Keyboard mapping must have 2 parts: " + s, true);
    }

    try {
		int n1 = stoi(parts[1]);
		int n2 = stoi(parts[2]);
		this.m[n1] = m2;
	} catch (exception &e) {
		throw MidiAppError("Keyboard mapping must have numbers on both sides of '=': " + s, true);
    }


}




}
