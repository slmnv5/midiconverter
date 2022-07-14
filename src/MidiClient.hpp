#ifndef MIDICLIENT_H
#define MIDICLIENT_H
#include "pch.hpp"

#include "MidiClient.hpp"
#include "MidiEvent.hpp"



//============== free functions ==============================
bool writeMidiEvent(snd_seq_event_t* event, const MidiEvent& ev);
bool readMidiEvent(const snd_seq_event_t* event, MidiEvent& ev);
//=============================================================



class MidiClient {
protected:
	int client = -1;
	int inport = -1;
	int outport = -1;
	snd_seq_t* seq_handle = nullptr;


public:
	MidiClient(const char* clientName) {
		open_alsa_connection(clientName);
	}
	virtual ~MidiClient() {
	}
	void make_and_send(snd_seq_event_t*, const MidiEvent& ev) const;
	int get_input_event(MidiEvent& ev) const;
private:
	void send_event(snd_seq_event_t* event) const;
	void open_alsa_connection(const char* clientName);
};

#endif
