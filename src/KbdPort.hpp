#ifndef KBDPORT_H
#define KBDPORT_H

#include "pch.hpp"
#include "MidiEvent.hpp"
#include "MidiClient.hpp"

class KbdPort {

private:
    std::map<int, midi_byte_t> kbdMap;
    std::string dev;
    MidiClient& midi_client;

public:
    KbdPort(const char* kbdFile, const char* kbdMapFile, MidiClient& mc);
    virtual ~KbdPort() {}
private:
    void parse_string(const string& s);
    void parse_file(const char* kbdMapFile);
    void start(int fd);

};

#endif