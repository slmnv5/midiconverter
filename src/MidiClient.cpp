#include "pch.hpp"
#include "MidiClient.hpp"
#include "MidiEvent.hpp"


//========== free functions ==================
bool writeMidiEvent(snd_seq_event_t* event, const MidiEvent& ev) {
	// note OFF is note ON with zero velocity
	if (ev.isNote()) {
		event->type = SND_SEQ_EVENT_NOTEON;
		event->data.note.channel = ev.ch;
		event->data.note.note = ev.v1;
		event->data.note.velocity = ev.v2;
		return true;
	}

	else if (ev.evtype == MidiEventType::PROGCHANGE) {
		event->type = SND_SEQ_EVENT_PGMCHANGE;
		event->data.control.channel = ev.ch;
		event->data.control.value = ev.v1;
		return true;
	}

	else if (ev.evtype == MidiEventType::CONTROLCHANGE) {
		event->type = SND_SEQ_EVENT_CONTROLLER;
		event->data.control.channel = ev.ch;
		event->data.control.param = ev.v1;
		event->data.control.value = ev.v2;
		return true;
	}
	return false;
}

bool readMidiEvent(const snd_seq_event_t* event, MidiEvent& ev) {
	if (event->type == SND_SEQ_EVENT_NOTEOFF) {
		ev.evtype = MidiEventType::NOTE;
		ev.ch = event->data.note.channel;
		ev.v1 = event->data.note.note;
		ev.v2 = 0;
		return true;
	}
	if (event->type == SND_SEQ_EVENT_NOTEON) {
		ev.evtype = MidiEventType::NOTE;
		ev.ch = event->data.note.channel;
		ev.v1 = event->data.note.note;
		ev.v2 = event->data.note.velocity;
		return true;
	}
	if (event->type == SND_SEQ_EVENT_PGMCHANGE) {
		ev.evtype = MidiEventType::PROGCHANGE;
		ev.ch = event->data.control.channel;
		ev.v1 = event->data.control.value;
		return true;
	}
	if (event->type == SND_SEQ_EVENT_CONTROLLER) {
		ev.evtype = MidiEventType::CONTROLCHANGE;
		ev.ch = event->data.control.channel;
		ev.v1 = event->data.control.param;
		ev.v2 = event->data.control.value;
		return true;
	}
	return false;
}

//========================================


void MidiClient::open_alsa_connection(const char* clientName) {
	const string clName = string(clientName).substr(0, 15);
	const string inPortName = clName + "_in";
	const string outPortName = clName + "_out";

	if (snd_seq_open(&seq_handle, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0)
		throw MidiAppError("Error opening ALSA seq_handle");

	snd_seq_set_client_name(seq_handle, clName.c_str());
	client = snd_seq_client_id(seq_handle);

	inport = snd_seq_create_simple_port(seq_handle, inPortName.c_str(),
		SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE,
		SND_SEQ_PORT_TYPE_APPLICATION);
	if (inport < 0)
		throw MidiAppError("Error creating seq_handle IN port");

	outport = snd_seq_create_simple_port(seq_handle, outPortName.c_str(),
		SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ,
		SND_SEQ_PORT_TYPE_APPLICATION);
	if (outport < 0)
		throw MidiAppError("Error creating seq_handle OUT port");

	LOG(LogLvl::INFO) << "MIDI ports created: IN=" << client << ":" << inport << " OUT="
		<< client << ":" << outport;
}


int MidiClient::get_input_event(MidiEvent& ev) const {
	snd_seq_event_t* event = nullptr;
	int result = snd_seq_event_input(seq_handle, &event);
	if (result < 0) {
		LOG(LogLvl::WARN) << "Possible loss of MIDI event";
		return -1;
	}
	if (!readMidiEvent(event, ev)) {
		LOG(LogLvl::WARN) << "Unknown MIDI event send as is, type: " << event->type;
		send_event(event);
		return -1;
	}
	return 1;
}


void MidiClient::send_event(snd_seq_event_t* event) const {
	snd_seq_ev_set_direct(event);
	snd_seq_ev_set_subs(event);
	snd_seq_ev_set_source(event, outport);
	snd_seq_event_output_direct(seq_handle, event);
}

void MidiClient::make_and_send(const MidiEvent& ev) const {
	snd_seq_event_t event;
	snd_seq_ev_clear(&event);
	if (!writeMidiEvent(&event, ev)) {
		LOG(LogLvl::ERROR) << "Failed to write event: " << ev.toString();
	};
	send_event(&event);
}
