#ifndef MIDICLIENT_H
#define MIDICLIENT_H

#include "pch.hpp"


using namespace std;

class KbdPort {

    const char* const evval[3] = {
    "RELEASED",
    "PRESSED ",
    "REPEATED"
    };


    const char* dev = "/dev/input/by-path/platform-i8042-serio-0-event-kbd";
    struct input_event ev;
    
    int fd = -1;



public:
    KbdPort(const string& kbdName) {
        fd = open(kbdName.c_str(), O_RDONLY);
        if (fd == -1) {
            throw MidiAppError("Cannot open keyboard device: " + kbdName);
        }
    }

    virtual ~KbdPort() {
    }

    virtual void start() const {
        ssize_t n;
        while (true) {
            n = read(fd, &ev, sizeof ev);
            if (n == (ssize_t)-1) {
                if (errno == EINTR)
                    continue;
                else
                    break;
            } else if (n != sizeof ev) {
                continue;
            }
        
        
        if (ev.type == EV_KEY && ev.value >= 0 && ev.value <= 2)
            printf("%s 0x%04x (%d)\n", evval[ev.value], (int)ev.code, (int)ev.code);

    
	}

};


#endif
