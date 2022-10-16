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
    char x, y;


    while (read(fd, &ie, sizeof(struct input_event)) != -1)
    {
        unsigned char* ptr = (unsigned char*)&ie;
        bLeft = ptr[0] & 0x1;

        x = (char)ptr[1];
        y = (char)ptr[2];
        printf("rx: %d  ry: %d\n", x, y);
        printf("ax: %d  ay: %d\n", absolute_x, absolute_y);

        absolute_x += x;
        absolute_y -= y;

    }
}

bool MousePort::get_input_event(MidiEvent& ev) {

    ev.evtype = MidiEventType::NOTE;
    ev.v1 = absolute_x;
    ev.v2 = absolute_y;


    printf("Absolute coords from TOP_LEFT= %i %i; %i\n", absolute_x, absolute_y, bLeft);
    return true;
}

