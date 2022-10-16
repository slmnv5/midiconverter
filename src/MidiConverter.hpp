#ifndef MIDICONVERTER_H
#define MIDICONVERTER_H

#include "pch.hpp"
#include "RuleMapper.hpp"
#include "MidiClient.hpp"
#include "MousePort.hpp"

class MidiConverter {
private:
    RuleMapper* rule_mapper;
    MidiClient* midi_client;
    MousePort* mouse_port;

public:
    MidiConverter(RuleMapper* rm, MidiClient* mc, MousePort* mp) :
        rule_mapper(rm), midi_client(mc), mouse_port(mp) {}
    virtual ~MidiConverter() {
    }

    void process_events();
    void process_one_event(MidiEvent& ev);

};

#endif
