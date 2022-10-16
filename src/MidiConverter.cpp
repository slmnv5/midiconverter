#include "pch.hpp"
#include "MidiEvent.hpp"
#include "RuleMapper.hpp"
#include "MidiClient.hpp"
#include "MidiConverter.hpp"

#include <alsa/asoundlib.h>

void MidiConverter::process_events() {
    MidiEvent ev;
    while (true) {
        if (midi_client->get_input_event(ev)) {
            LOG(LogLvl::DEBUG) << "Got midi msg: " << ev.toString();
            process_one_event(ev);
        }
    }
}



void MidiConverter::process_one_event(MidiEvent& ev) {
    if (rule_mapper->applyRules(ev)) {
        LOG(LogLvl::INFO) << "Send mapped event: " << ev.toString();
        midi_client->make_and_send(ev);
    }
}

