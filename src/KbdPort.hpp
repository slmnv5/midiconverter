#ifndef KBDPORT_H
#define KBDPORT_H

#include "pch.hpp"
//#include "MidiEvent.hpp"
#include "MidiClient.hpp"

class KbdPort {

private:
    std::map<int, midi_byte_t> kbdMap;
    std::string dev;
    int fd;
    MidiClient* midi_client;

public:
    KbdPort(const char* kbdFile, const char* kbdMapFile);
    virtual ~KbdPort() {}
    void start(MidiClient* mc);
private:
    void parse_string(const string& s);
    void parse_file(const char* kbdMapFile);
    void readKbd();
};

#endif