#include "pch.hpp"

#include <fcntl.h>
#include <linux/input.h>

#include "utils.hpp"
#include "MousePort.hpp"
#include "MidiEvent.hpp"




MousePort::MousePort() {

    absolute_x = 0;
    absolute_y = 0;
    string tmp = "/dev/input/event" + findTouchScreenEvent();
    fd = open(tmp.c_str(), O_RDONLY);
    if (fd == -1) {
        throw MidiAppError("Cannot open touch screen file: " + tmp, true);
    }
    thread(&MousePort::run, this).detach();

}


void MousePort::run() {
    struct input_event ie;


    while (read(fd, &ie, sizeof(struct input_event)) != -1)
    {

        if (ie.type != EV_REL && ie.type != EV_ABS && ie.type != EV_KEY) {
            continue;
        }
        printf("-------type %d\tcode %d\tvalue %d\n", ie.type, ie.code, ie.value);
        printf("\n");

    }
}

bool MousePort::get_input_event(MidiEvent& ev) {

    ev.evtype = MidiEventType::NOTE;
    ev.v1 = absolute_x;
    ev.v2 = absolute_y;


    printf("Absolute coords from TOP_LEFT= %i %i; %i\n", absolute_x, absolute_y, bLeft);
    return true;
}

