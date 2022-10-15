#ifndef MOUSEPORT_H
#define MOUSEPORT_H

#include "pch.hpp"
#include "MidiEvent.hpp"

std::string findMouseEvent();
std::string getInputDevicePath();


class MousePort {
private:
    static const char* dev;
    int fd;
    int absolute_x, absolute_y;

public:
    MousePort();
    virtual ~MousePort() {
    }
    void get_input_event(MidiEvent& ev);

};

const char* MousePort::dev = "/dev/input/mouse0";
#endif