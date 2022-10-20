#include "pch.hpp"
#include "MidiClient.hpp"
#include "MidiEvent.hpp"
#include "utils.hpp"
#include <fcntl.h>
#include <linux/input.h>


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


void MidiClient::open_alsa_connection(const char* clientName, const char* sourceName) {
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
		throw MidiAppError("Error creating virtual IN port", true);


	outport = snd_seq_create_simple_port(seq_handle, outPortName.c_str(),
		SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ,
		SND_SEQ_PORT_TYPE_APPLICATION);
	if (outport < 0)
		throw MidiAppError("Error creating virtual OUT port", true);

	LOG(LogLvl::INFO) << "MIDI ports created: IN=" << client << ":" << inport << " OUT="
		<< client << ":" << outport;

	if (nullptr == sourceName)
		return;

	int cli_id = -1;
	int cli_port = -1;
	if (find_midi_source(sourceName, cli_id, cli_port) < 0) {
		throw MidiAppError("Error finding source hardware port: " + string(sourceName), true);
	}

	subscribe(cli_id, cli_port);
}


bool MidiClient::get_input_event(MidiEvent& ev) {
	snd_seq_event_t* event = nullptr;
	int result = snd_seq_event_input(seq_handle, &event);
	if (result < 0) {
		LOG(LogLvl::WARN) << "Possible loss of MIDI event";
		return false;
	}
	if (!readMidiEvent(event, ev)) {
		LOG(LogLvl::WARN) << "Unknown MIDI event send as is, type: " << event->type;
		send_event(event);
		return false;
	}
	return true;
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

int MidiClient::find_midi_source(const std::string& name_part, int& cli_id, int& cli_port) const {
	snd_seq_client_info_t* cinfo;
	snd_seq_port_info_t* pinfo;

	snd_seq_client_info_alloca(&cinfo);
	snd_seq_port_info_alloca(&pinfo);

	snd_seq_client_info_set_client(cinfo, -1);
	while (snd_seq_query_next_client(seq_handle, cinfo) >= 0) {
		int cl = snd_seq_client_info_get_client(cinfo);
		std::string cl_name(snd_seq_client_info_get_name(cinfo));
		if (cl_name.find(name_part) == std::string::npos)
			continue;

		snd_seq_port_info_set_client(pinfo, cl);
		snd_seq_port_info_set_port(pinfo, -1);
		unsigned int capability = 0;
		while (snd_seq_query_next_port(seq_handle, pinfo) >= 0) {
			// port must understand MIDI messages
			capability = SND_SEQ_PORT_TYPE_MIDI_GENERIC;
			if ((snd_seq_port_info_get_type(pinfo)
				& capability) != capability)
				continue;
			// we need both READ and SUBS_READ
			capability = SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ;
			if ((snd_seq_port_info_get_capability(pinfo)
				& capability) != capability)
				continue;
			cli_id = snd_seq_port_info_get_client(pinfo);
			cli_port = snd_seq_port_info_get_port(pinfo);
			LOG(LogLvl::INFO) << "Found source: " << name_part << " -- " << cli_id << ":" << cli_port;
			return 0;
		}
	}
	return -1;
}

void MidiClient::subscribe(const int& cli_id, const int& cli_port) const {
	if (snd_seq_connect_from(seq_handle, inport, cli_id, cli_port) < 0) {
		throw new MidiAppError("Cannot connect to port: " + to_string(cli_id) + ":" + to_string(cli_port));
	}
	LOG(LogLvl::INFO) << "Connected to source: " << cli_id << ":" << cli_port;
}




