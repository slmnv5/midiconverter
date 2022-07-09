#ifndef KBDPORT_H
#define KBDPORT_H

#include "pch.hpp"
//#include "MidiEvent.hpp"
#include "MidiClient.hpp"

class KbdPort {

private:
    std::map<int, midi_byte_t> kbdMap;
    std::string dev;


public:
    KbdPort(const char* kbdFile, const char* kbdMapFile);
    virtual ~KbdPort() {}
private:
    void parse_string(const string& s);
    void parse_file(const char* kbdMapFile);
    void start(int fd, MidiClient& mc);

};

#endif