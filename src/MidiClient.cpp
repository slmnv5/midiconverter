#include "pch.hpp"
#include "MidiEvent.hpp"
#include "RuleMapper.hpp"
#include "MidiClient.hpp"

#include <alsa/asoundlib.h>
#include "log.hpp"

using namespace std;

void MidiClient::open_alsa_connection(const char* clientName) {
	const string clName = string(clientName).substr(0, 15);
	const string inPortName = clName + "_in";
	const string outPortName = clName + "_out";

	if (snd_seq_open(&seq_handle, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0)
		throw MidiAppError("Error opening ALSA seq_handle");

	snd_seq_set_client_name(seq_handle, clName.c_str());
	client = snd_seq_client_id(seq_handle);

	outport = snd_seq_create_simple_port(seq_handle, outPortName.c_str(),
		SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ,
		SND_SEQ_PORT_TYPE_APPLICATION);
	if (outport < 0)
		throw MidiAppError("Error creating seq_handle OUT port");


	inport = snd_seq_create_simple_port(seq_handle, inPortName.c_str(),
		SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE,
		SND_SEQ_PORT_TYPE_APPLICATION);
	if (inport < 0)
		throw MidiAppError("Error creating seq_handle IN port");

	cout << "MIDI ports created: IN=" << client << ":" << inport << "   OUT="
		<< client << ":" << outport << endl;
}

void MidiClient::process_events() {
	snd_seq_event_t* event = nullptr;
	MidiEvent ev;
	long k = 0;
	while (true) {
		int result = snd_seq_event_input(seq_handle, &event);
		if (result < 0) {
			LOG(LogLvl::WARN) << "Possible loss of MIDI events! " << result;
			continue;
		}

		if (!readMidiEvent(event, ev)) {
			LOG(LogLvl::DEBUG) << "Unknown MIDI message sent as is, type: "
				<< to_string(event->type);
			send_old_event(event, outport);
		}
		else {
			process_one_event(event, ev);
		}
	}
}

void MidiClient::send_old_event(snd_seq_event_t* event, int port) const {
	snd_seq_ev_set_direct(event);
	// snd_seq_ev_set_dest(event, 64, 0) or send to subscribers of source port
	snd_seq_ev_set_subs(event);
	snd_seq_ev_set_source(event, port);
	snd_seq_event_output_direct(seq_handle, event);
}

void MidiClient::send_new_event(const MidiEvent& ev, int port) const {
	snd_seq_event_t* event = new snd_seq_event_t();
	snd_seq_ev_clear(event);
	if (!writeMidiEvent(event, ev)) {
		snd_seq_free_event(event);
		LOG(LogLvl::ERROR) << "Failed to write event: " << ev.toString();
		return;
	};
	send_old_event(event, port);
}
//===============================================================
void MidiConverter::process_one_event(snd_seq_event_t* event, MidiEvent& ev) {
	if (rule_mapper.applyRules(ev)) {
		LOG(LogLvl::INFO) << "Send transformed event: " << ev.toString();
		writeMidiEvent(event, ev);
		send_old_event(event, outport);
	}
	else {
		snd_seq_ev_clear(event);
		LOG(LogLvl::DEBUG) << "Cleared non matching event: "
			<< ev.toString();
	}
}

//===============================================================
