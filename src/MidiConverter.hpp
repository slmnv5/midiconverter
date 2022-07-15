#ifndef MIDICONVERTER_H
#define MIDICONVERTER_H

#include "pch.hpp"
#include "RuleMapper.hpp"
#include "MidiClient.hpp"
#include "KbdPort.hpp"

class MidiConverter {
private:
    RuleMapper* rule_mapper;
    MidiClient* midi_client;
    KbdPort* kbd_port;

public:
    MidiConverter(RuleMapper* rm, MidiClient* mc, KbdPort* kp) :
        rule_mapper(rm), midi_client(mc), kbd_port(kp) {}
    virtual ~MidiConverter() {
    }

    void process_events();
    void process_one_event(MidiEvent& ev);

};

#endif
