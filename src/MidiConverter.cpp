
#include "MidiConverter.hpp"


void MidiConverter::process_events() {
    MidiEvent ev;
    snd_seq_event_t* event;
    const MidiClient* midi_client = rule_mapper->get_midi_client();
    while (true) {
        event = midi_client->get_input_event();
        if (nullptr != event && readMidiEvent(event, ev)) {
            LOG(LogLvl::DEBUG) << "Got midi msg: " << ev.toString();
            process_one_event(ev);
        }
        else {
            LOG(LogLvl::WARN) << "Unknown MIDI event";
        }
    }
}




void MidiConverter::process_one_event(MidiEvent& ev) {
    if (rule_mapper->applyRules(ev)) {
        LOG(LogLvl::INFO) << "Send mapped event: " << ev.toString();
        rule_mapper->make_and_send(ev);
    }
}

