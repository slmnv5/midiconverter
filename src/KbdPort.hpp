#ifndef KBDPORT_H
#define KBDPORT_H

#include "pch.hpp"
#include "MidiEvent.hpp"
#include "MidiClient.hpp"

class KbdPort {

private:
    std::map<int, midi_byte_t> kbdMap;
    std::string dev;
    MidiClient midi_client;
    int fd;

public:
    KbdPort(const std::string& kbdName, const std::string& fileName, const MidiClient& mc);
    virtual ~KbdPort() {}
    virtual void start();
private:
    void parse_string(const string& s);
    void parse_file(const string& fileName);

};

#endif