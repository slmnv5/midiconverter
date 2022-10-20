#ifndef MIDICONVERTER_H
#define MIDICONVERTER_H

#include "pch.hpp"
#include "RuleMapper.hpp"
#include "MidiClient.hpp"

class MidiConverter {
private:
    RuleMapper* rule_mapper;
    MidiClient* midi_client;

public:
    MidiConverter(RuleMapper* rm, MidiClient* mc) :
        rule_mapper(rm), midi_client(mc) {}
    virtual ~MidiConverter() {
    }

    void process_events();
    void process_one_event(MidiEvent& ev);

};

#endif
