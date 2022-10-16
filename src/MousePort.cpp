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
        bLeft = (ptr[0] & 0x1) > 0;
        bool bMiddle = (ptr[0] & 0x4) > 0;
        bool bRight = (ptr[0] & 0x2) > 0;

        x = (char)ptr[1];
        y = (char)ptr[2];

        absolute_x += x;
        absolute_y -= y;

        printf("====bLEFT:%d, bMIDDLE: %d, bRIGHT: %d, rx: %d  ry: %d\n", bLeft, bMiddle, bRight, x, y);

        printf("-------time %ld.%06ld\ttype %d\tcode %d\tvalue %d\n",
            ie.time.tv_sec, ie.time.tv_usec, ie.type, ie.code, ie.value);


        for (size_t i = 0; i < sizeof(ie); i++)
            printf("%02X ", *ptr++);
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

