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
	MidiClient(const char* clientName, const char* sourceName) {
		open_alsa_connection(clientName, sourceName);
	}
	virtual ~MidiClient() {
	}
	void make_and_send(const MidiEvent& ev) const;
	virtual bool get_input_event(MidiEvent& ev);
protected:
	void send_event(snd_seq_event_t* event) const;
	virtual void open_alsa_connection(const char* clientName, const char* sourceName);
private:
	int find_midi_source(const std::string& name_part, int& cli_id, int& cli_port) const;
	void subscribe(const int& id, const int& port) const;
};


class MidiKbdClient : public MidiClient {
private:
	std::map<int, int> kbdMap;
	int fd;

public:
	MidiKbdClient(const char* clientName, const char* sourceName);
	virtual ~MidiKbdClient() {
	}
	bool get_input_event(MidiEvent& ev);
private:
	void parse_string(const std::string& s);
	void parse_file(const char* kbdMapFile);

};






#endif
