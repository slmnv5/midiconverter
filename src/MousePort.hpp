#ifndef MOUSEPORT_H
#define MOUSEPORT_H

#include "pch.hpp"
#include "MidiEvent.hpp"

std::string findTouchScreenEvent();



class MousePort {
private:
    int fd;
    int absolute_x, absolute_y;

public:
    MousePort();
    virtual ~MousePort() {
    }
    void get_input_event(MidiEvent& ev);

};


#endif