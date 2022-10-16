#ifndef MOUSEPORT_H
#define MOUSEPORT_H

#include "pch.hpp"
#include "MidiEvent.hpp"



class MousePort {
private:
    int fd;
    int absolute_x, absolute_y;
    bool bLeft;

public:
    MousePort();
    virtual ~MousePort() {
    }
    virtual bool get_input_event(MidiEvent& ev);
private:
    void run();

};


#endif