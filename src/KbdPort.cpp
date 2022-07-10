#include "pch.hpp"

#include <fcntl.h>
#include <linux/input.h>

#include "utils.hpp"
#include "KbdPort.hpp"
#include "log.hpp"
#include "MidiEvent.hpp"
#include "MidiClient.hpp"


std::string findKbdEvent() {
    const char* cmd = "grep -E 'Handlers|EV=' /proc/bus/input/devices | "
        "grep -B1 'EV=120013' | grep -Eo 'event[0-9]+' | grep -Eo '[0-9]+' | tr -d '\n'";

    FILE* pipe = popen(cmd, "r");
    char buffer[128];
    std::string result = "";
    while (!feof(pipe))
        if (fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    pclose(pipe);
    return result;
}

std::string getInputDevicePath() {
    string tmp = "/dev/input/event" + findKbdEvent();
    LOG(LogLvl::DEBUG) << "Typing keyboard file found: " << tmp;
    return tmp;
}

KbdPort::KbdPort(const char* kbdMapFile) {
    string tmp = getInputDevicePath();
    fd = open(tmp.c_str(), O_RDONLY);
    if (fd == -1) {
        throw MidiAppError("Cannot open typing keyboard file: " + tmp, true);
    }
    parse_file(kbdMapFile);
}


void KbdPort::start(MidiClient* mc) {
    midi_client = mc;
    thread(&KbdPort::readKbd, this).detach();
}

void KbdPort::readKbd() {
    ssize_t n;
    struct input_event kbd_ev;
    LOG(LogLvl::DEBUG) << "Started thread reading typing keyboard";
    while (true) {
        n = read(fd, &kbd_ev, sizeof kbd_ev);
        if (n == (ssize_t)-1) {
            if (errno == EINTR)
                continue;
            else
                break;
        }
        if (n != sizeof kbd_ev)
            continue;
        if (kbd_ev.type != EV_KEY)
            continue;
        if (kbd_ev.value < 0 || kbd_ev.value > 1)
            continue;
        if (kbdMap.find((int)kbd_ev.code) == kbdMap.end())
            continue;

        LOG(LogLvl::DEBUG) << "Typing keyboard code: " << kbd_ev.code;
        MidiEvent ev = MidiEvent();
        ev.evtype = MidiEventType::NOTE;
        ev.v1 = kbdMap.at((int)kbd_ev.code);
        ev.v2 = kbd_ev.value == 0 ? 0 : 100;
        LOG(LogLvl::DEBUG) << "Typing keyboard event: " << ev.toString();
        midi_client->take_in(ev);
    }
}



void KbdPort::parse_string(const string& s1) {
    string s(s1);
    remove_spaces(s);
    if (s.empty()) {
        LOG(LogLvl::DEBUG) << "Line was ignored: " << s1;
        return;
    }
    vector<string> parts = split_string(s, "=");
    if (parts.size() != 2) {
        throw MidiAppError("Keyboard mapping must have 2 parts: " + s, true);
    }

    try {
        int n1 = std::stoi(parts[0]);
        int n2 = std::stoi(parts[1]);
        LOG(LogLvl::DEBUG) << "Mapping typing key code to note: " << n1 << "=" << n2;
        kbdMap[n1] = n2;
    }
    catch (exception& e) {
        throw MidiAppError("Keyboard mapping must have numbers on both sides of '=': " + s, true);
    }
}

void KbdPort::parse_file(const char* kbdMapFile) {
    ifstream f(kbdMapFile);
    string s;
    int k = 0;
    while (getline(f, s)) {
        try {
            k++;
            parse_string(s);
        }
        catch (MidiAppError& e) {
            LogLvl level = e.is_critical() ? LogLvl::ERROR : LogLvl::WARN;
            LOG(level)
                << "Line: " + to_string(k) + " in " + kbdMapFile + " Error: "
                + e.what();
        }
    }
}






